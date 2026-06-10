#include "FactoredOutputSet.h"

#include <numeric>
#include <print>

FactoredOutputSet::FactoredOutputSet(const Network& network, uint8_t n)
	: clusters(n), wireToCluster(n)
{
	// Initialize clusters
	for (uint8_t k = 0; k < n; k++)
		clusters[k] = { 0, 1ULL << k };

	// Initialize mapping from wire -> cluster
	std::iota(wireToCluster.begin(), wireToCluster.end(), 0);

	// Apply all comparators to the output set
	std::vector<CE> worklist{ network };
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
		ApplyCE(i, j);
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

void FactoredOutputSet::CombineClusters(uint8_t clusterIdx1, uint8_t clusterIdx2)
{
	std::vector<uint64_t>& cluster1 = clusters[clusterIdx1];
	std::vector<uint64_t>& cluster2 = clusters[clusterIdx2];

	// Create the new cluster as the cartesian product of the two source clusters
	std::vector<uint64_t> newCluster;
	newCluster.reserve(cluster1.size() * cluster2.size());
	for (uint64_t pattern1 : cluster1)
		for (uint64_t pattern2 : cluster2)
			newCluster.push_back(pattern1 | pattern2);

	// Store the new cluster into the main clusters vector
	std::swap(cluster1, newCluster);
	cluster2.clear();

	// Replace all occurences of wireToCluster[j] with the new combined cluster
	for (uint8_t& clusterIdx : wireToCluster)
		if (clusterIdx == clusterIdx2)
			clusterIdx = clusterIdx1;
}

void FactoredOutputSet::ApplyCE(uint8_t i, uint8_t j)
{
	std::vector<uint64_t>& cluster = clusters[wireToCluster[i]];

	// Initialize storage for the new cluster
	std::vector<uint64_t> newCluster;
	newCluster.reserve(cluster.size());

	// Initialize hasPattern vector
	size_t patternSpaceSize = 1ULL << wireToCluster.size();
	thread_local std::vector<bool> hasPattern(patternSpaceSize, false);
	if (hasPattern.size() < patternSpaceSize)
		hasPattern.resize(patternSpaceSize, false);

	// Apply the comparator to every pattern in the cluster
	uint64_t ceMask = 1ULL << i | 1ULL << j;
	uint64_t flipMask = 1ULL << i;
	for (uint64_t pattern : cluster)
	{
		if ((pattern & ceMask) == flipMask)
			pattern ^= ceMask;

		if (!hasPattern[pattern])
			newCluster.push_back(pattern);
		hasPattern[pattern] = true;
	}
	std::swap(cluster, newCluster);

	// Reset the elements of hasPattern which were modified
	for (uint64_t pattern : newCluster)
		hasPattern[pattern] = false;
}

void FactoredOutputSet::ReorderWorklist(std::vector<CE>& worklist)
{
	// If a comparator can be applied now (no earlier comparators overlap it) and acts within a cluser,
	// Move it to the front of the worklist
	std::vector<CE> l1, l2;
	uint64_t usedChannels = 0;
	for (auto [i, j] : worklist)
	{
		uint64_t ceMask = 1ULL << i | 1ULL << j;
		if (!(usedChannels & ceMask) && wireToCluster[i] == wireToCluster[j])
			l1.push_back({ i, j });
		else
			l2.push_back({ i, j });
		usedChannels |= ceMask;
	}

	worklist = l1;
	worklist.insert(worklist.end(), l2.begin(), l2.end());
}