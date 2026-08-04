#include "FactoredOutputSet.h"

#include <numeric>
#include <cassert>
#include <bit>
#include <algorithm>
#include <set>

FactoredOutputSet::BitVec::BitVec(size_t size)
	: packs((size + 63) / 64, 0) {}

size_t FactoredOutputSet::BitVec::Size() const
{
	return packs.size() * 64;
}

void FactoredOutputSet::BitVec::Resize(size_t size)
{
	packs.resize((size + 63) / 64, 0);
}

bool FactoredOutputSet::BitVec::operator[](size_t idx) const
{
	size_t packIdx = idx / 64;
	size_t packOff = idx % 64;
	return packs[packIdx] & (1ULL << packOff);
}

void FactoredOutputSet::BitVec::SetBit(size_t idx)
{
	size_t packIdx = idx / 64;
	size_t packOff = idx % 64;
	packs[packIdx] |= (1ULL << packOff);
}

void FactoredOutputSet::BitVec::ClearBitLazy(size_t idx)
{
	packs[idx / 64] = 0;
}

FactoredOutputSet::FactoredOutputSet(const Network& network, uint8_t n)
	: clusters(n), wireToCluster(n)
{
	assert(!network.IsGeneralized());

	// Initialize clusters
	for (uint8_t k = 0; k < n; k++)
		clusters[k] = { 0, 1ULL << k };

	// Initialize mapping from wire -> cluster
	std::iota(wireToCluster.begin(), wireToCluster.end(), 0);

	// Apply all comparators to the output set
	ApplyCEs(network);
}

static inline void Dfs(uint8_t u, const std::vector<std::vector<bool>>& dependent, std::vector<bool>& visited, std::vector<uint8_t>& component)
{
	visited[u] = true;
	component.push_back(u);

	for (uint8_t v = 0; v < (uint8_t)visited.size(); v++)
		if (dependent[u][v] && !visited[v])
			Dfs(v, dependent, visited, component);
}

static inline std::vector<std::vector<uint8_t>> GetComponents(const std::vector<std::vector<bool>>& dependent)
{
	uint8_t n = (uint8_t)dependent.size();
	std::vector<bool> visited(n, false);
	std::vector<std::vector<uint8_t>> components;

	for (uint8_t i = 0; i < n; i++)
	{
		if (visited[i]) continue;
		components.emplace_back();
		Dfs(i, dependent, visited, components.back());
	}

	return components;
}

FactoredOutputSet::FactoredOutputSet(const std::vector<uint64_t>& outputs, uint8_t n)
	: clusters(n), wireToCluster(n)
{
	// Build dependency graph
	std::vector<std::vector<bool>> dependent(n, std::vector<bool>(n, false));
	for (uint8_t i = 0; i < n; i++)
	{
		for (uint8_t j = i + 1; j < n; j++)
		{
			// For all outputs, extract bits i and j and track observed patterns
			uint64_t ijMask = (1ULL << i) | (1ULL << j);
			uint64_t ijSeen = 0;
			for (uint64_t x : outputs)
				ijSeen |= (1ULL << _pext_u64(x, ijMask));

			// Packed LUT of independent masks
			static constexpr uint16_t IndependentMasks =
				(1 << 0b0001) | (1 << 0b0010) | (1 << 0b0100) | (1 << 0b1000) | // 1x1
				(1 << 0b0101) | (1 << 0b1010) |                                 // 1x2
				(1 << 0b0011) | (1 << 0b1100) |                                 // 2x1
				(1 << 0b1111);                                                  // 2x2

			// Add a dependency edge if the bits are not independent
			if ((IndependentMasks & (1 << ijSeen)) == 0)
			{
				dependent[i][j] = true;
				dependent[j][i] = true;
			}
		}
	}
	
	// Get dependent components (partitions)
	auto partitions = GetComponents(dependent);

	for (size_t partitionIdx = 0; partitionIdx < partitions.size(); partitionIdx++)
	{
		// Compute the partition mask
		const auto& partition = partitions[partitionIdx];
		uint64_t partitionMask = 0;
		for (uint8_t i : partition) partitionMask |= (1ULL << i);

		// Track which patterns occur over this partition
		std::set<uint64_t> cluster;
		for (uint64_t x : outputs) cluster.insert(x & partitionMask);
		clusters[partitionIdx].assign_range(cluster);

		// Update wireToCluster
		for (uint8_t i : partition) wireToCluster[i] = partitionIdx;
	}
}

size_t FactoredOutputSet::Size() const
{
	// The total set size is the product of the sizes of all the clusters
	size_t numOutputs = 1;
	for (const auto& cluster : clusters)
		if (!cluster.empty())
			numOutputs *= cluster.size();
	return numOutputs;
}

std::vector<uint64_t> FactoredOutputSet::ToVector() const&
{
	size_t numOutputs = Size();

	// The first cluster will always be non-empty, copy its elements
	std::vector<uint64_t> outputs;
	outputs.reserve(numOutputs);
	outputs.insert(outputs.begin(), clusters[0].begin(), clusters[0].end());

	for (size_t clusterIdx = 1; clusterIdx < clusters.size(); clusterIdx++)
	{
		const std::vector<uint64_t>& cluster = clusters[clusterIdx];

		// Skip empty clusters
		if (cluster.empty()) continue;

		size_t clusterSize1 = outputs.size();
		for (size_t pi2 = 1; pi2 < cluster.size(); pi2++)
			for (size_t pi1 = 0; pi1 < clusterSize1; pi1++)
				outputs.push_back(outputs[pi1] | cluster[pi2]);
	}

	return outputs;
}

std::vector<uint64_t> FactoredOutputSet::ToVector() &&
{
	size_t numOutputs = Size();

	// The first cluster will always be non-empty, re-use it as the return value
	std::vector<uint64_t>& outputs = clusters[0];
	outputs.reserve(numOutputs);

	for (size_t clusterIdx = 1; clusterIdx < clusters.size(); clusterIdx++)
	{
		const std::vector<uint64_t>& cluster = clusters[clusterIdx];

		// Skip empty clusters
		if (cluster.empty()) continue;

		size_t clusterSize1 = outputs.size();
		for (size_t pi2 = 1; pi2 < cluster.size(); pi2++)
			for (size_t pi1 = 0; pi1 < clusterSize1; pi1++)
				outputs.push_back(outputs[pi1] | cluster[pi2]);
	}

	return std::move(outputs);
}

bool FactoredOutputSet::IsFactored() const
{
	for (size_t i = 1; i < clusters.size(); i++)
		if (!clusters[i].empty()) return true;
	return false;
}

bool FactoredOutputSet::IsRedundant() const
{
	return isRedundant;
}

void FactoredOutputSet::ApplyCE(uint8_t i, uint8_t j)
{
	if (wireToCluster[i] != wireToCluster[j])
		CombineClusters(wireToCluster[i], wireToCluster[j]);

	DoApplyCE(i, j);
}

void FactoredOutputSet::ApplyCEs(const Network& ces)
{
	std::vector<CE> worklist{ ces };
	while (!worklist.empty())
	{
		// Pop the first comparator from the worklist
		auto [i, j] = worklist[0];
		worklist.erase(worklist.begin());

		// Comparators can only be applied within a cluster
		// If this comparator spans two clusters, combine them
		if (wireToCluster[i] != wireToCluster[j])
		{
			CombineClusters(wireToCluster[i], wireToCluster[j]);
			ReorderWorklist(worklist);
		}

		// Apply the comparator
		DoApplyCE(i, j);
	}
}

void FactoredOutputSet::SwapBits(uint8_t i, uint8_t j)
{
	if (wireToCluster[i] != wireToCluster[j])
		CombineClusters(wireToCluster[i], wireToCluster[j]);

	std::vector<uint64_t>& cluster = clusters[wireToCluster[i]];
	uint64_t leftMask = 1ULL << i;
	uint64_t rightMask = 1ULL << j;
	uint64_t stationaryMask = ~(leftMask | rightMask);
	uint64_t shift = j - i;

	for (uint64_t& pattern : cluster)
		pattern = (pattern & stationaryMask)
			| (pattern & leftMask) << shift
			| (pattern & rightMask) >> shift;
}

void FactoredOutputSet::CombineClusters(uint8_t clusterIdx1, uint8_t clusterIdx2)
{
	if (clusterIdx1 > clusterIdx2) std::swap(clusterIdx1, clusterIdx2);

	std::vector<uint64_t>& cluster1 = clusters[clusterIdx1];
	std::vector<uint64_t>& cluster2 = clusters[clusterIdx2];
	size_t cluster1Size = cluster1.size();
	size_t cluster2Size = cluster2.size();

	cluster1.resize(cluster1.size() * cluster2.size());
	size_t writeIdx = cluster1Size;
	for (size_t p2i = 1; p2i < cluster2Size; p2i++)
		for (size_t p1i = 0; p1i < cluster1Size; p1i++)
			cluster1[writeIdx++] = cluster1[p1i] | cluster2[p2i];

	cluster2.clear();

	// Replace all occurences of wireToCluster[j] with the new combined cluster
	for (uint8_t& clusterIdx : wireToCluster)
		if (clusterIdx == clusterIdx2)
			clusterIdx = clusterIdx1;
}

void FactoredOutputSet::DoApplyCE(uint8_t i, uint8_t j)
{
	std::vector<uint64_t>& cluster = clusters[wireToCluster[i]];

	// Initialize hasPattern vector
	size_t patternSpaceSize = 1ULL << wireToCluster.size();
	thread_local BitVec hasPattern{ patternSpaceSize };
	if (hasPattern.Size() < patternSpaceSize)
		hasPattern.Resize(patternSpaceSize);

	// Apply the comparator to every pattern in the cluster
	uint64_t ceWidth = j - i;
	uint64_t jMask = 1ULL << j;
	size_t writeIdx = 0;
	for (size_t patternIdx = 0; patternIdx < cluster.size(); patternIdx++)
	{
		uint64_t pattern = cluster[patternIdx];

		// Build a mask containing 1s at bits i and j if a swap should occur
		uint64_t swapMask = (pattern << ceWidth) & ~pattern;
		swapMask &= jMask;
		swapMask |= swapMask >> ceWidth;

		// Apply the swap mask and insert
		pattern ^= swapMask;
		if (!hasPattern[pattern])
			cluster[writeIdx++] = pattern;
		hasPattern.SetBit(pattern);
	}

	if (writeIdx == cluster.size())
		isRedundant = true;
	cluster.resize(writeIdx);

	// Reset the elements of hasPattern which were modified
	for (uint64_t pattern : cluster)
		hasPattern.ClearBitLazy(pattern);
}

void FactoredOutputSet::ReorderWorklist(std::vector<CE>& worklist)
{
	uint64_t usedChannels = 0;
	size_t writeIdx = 0;
	for (size_t ceIdx = 0; ceIdx < worklist.size(); ceIdx++)
	{
		// If a comparator can be applied now (no earlier comparators overlap it) and acts within a cluser, it has priority
		auto [lo, hi] = worklist[ceIdx];
		uint64_t ceMask = 1ULL << lo | 1ULL << hi;
		bool isPriority = !(usedChannels & ceMask) && wireToCluster[lo] == wireToCluster[hi];
		usedChannels |= ceMask;

		if (!isPriority) continue;

		// Move priority elements backward towards writeIdx
		for (size_t i = ceIdx; i > writeIdx; i--)
			std::swap(worklist[i - 1], worklist[i]);

		writeIdx++;
	}
}