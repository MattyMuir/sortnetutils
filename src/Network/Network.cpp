#include "Network.h"

#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <bit>

#include "LayeredNetwork.h"
#include "IndexedNetwork.h"
#include "../Outputs/FactoredOutputSet.h"
#include "../Outputs/OutputSet.h"

Network::Network(const LayeredNetwork& network)
{
	reserve(network.NetworkSize());
	for (const Network& layer : network)
		*this += layer;
}

Network Network::operator+(const Network& other) const
{
	Network ret{ *this };
	ret.insert(ret.end(), other.begin(), other.end());
	return ret;
}

Network& Network::operator+=(const Network& other)
{
	insert(end(), other.begin(), other.end());
	return *this;
}

uint64_t Network::operator()(uint64_t x) const
{
	for (auto [lo, hi] : *this)
	{
		uint64_t loMask = 1ULL << lo;
		uint64_t hiMask = 1ULL << hi;
		if ((x & loMask) && (~x & hiMask))
			x ^= loMask | hiMask;
	}
	return x;
}

uint8_t Network::InferN() const
{
	uint8_t maxHi = 0;
	for (auto [i, j] : *this)
		maxHi = std::max(maxHi, std::max(i, j));
		
	return maxHi + 1;
}

size_t Network::Depth() const
{
	std::vector<uint64_t> usedChannels;
	for (CE ce : *this)
	{
		// Get a mask of the channels used by this comparator
		uint64_t ceChannels = (1ULL << ce.lo) | (1ULL << ce.hi);

		// Push the comparator backwards until it cannot fit in a layer
		int64_t insertLayer = usedChannels.size() - 1;
		while (insertLayer >= 0 && !(usedChannels[insertLayer] & ceChannels))
			insertLayer--;
		insertLayer++;

		// Resize storage if this comparator is in a new layer
		if (insertLayer >= std::ssize(usedChannels))
			usedChannels.push_back(0);

		// Insert comparator into the layer
		usedChannels[insertLayer] |= ceChannels;
	}

	return usedChannels.size();
}

bool Network::IsGeneralized() const
{
	return std::ranges::any_of(*this, [](CE ce) { return ce.lo > ce.hi; });
}

bool Network::IsSymmetric(uint8_t n) const
{
	IndexedNetwork indexed{ *this, n };
	return indexed.IsSymmetric();
}

bool Network::IsSorting(uint8_t n) const
{
	FactoredOutputSet outputs{ *this, n };
	return outputs.Size() == n + 1;
}

bool Network::Identical(const Network& a, const Network& b)
{
	uint8_t n = a.InferN();
	if (b.InferN() != n) return false;

	IndexedNetwork aIndexed{ a, n };
	IndexedNetwork bIndexed{ b, n };
	return aIndexed == bIndexed;
}

void Network::Permute(const Permutation& perm)
{
	Permutation mapsTo{ perm };
	mapsTo.Invert();

	for (CE& ce : *this)
	{
		uint8_t newLo = mapsTo[ce.lo];
		uint8_t newHi = mapsTo[ce.hi];
		ce = { newLo, newHi };
	}
}

void Network::Untangle()
{
	Permutation mapsTo(InferN());
	std::iota(mapsTo.begin(), mapsTo.end(), 0);
	for (CE& ce : *this)
	{
		uint8_t newLo = mapsTo[ce.lo];
		uint8_t newHi = mapsTo[ce.hi];
		if (newLo > newHi)
		{
			mapsTo[ce.lo] = newHi;
			mapsTo[ce.hi] = newLo;
			std::swap(newLo, newHi);
		}
		ce = { newLo, newHi };
	}
}

Permutation Network::GetOutputPermutation(const Permutation& perm) const
{
	Permutation mapsTo{ perm };
	mapsTo.Invert();

	for (const CE& ce : *this)
		if (mapsTo[ce.lo] > mapsTo[ce.hi])
			std::swap(mapsTo[ce.lo], mapsTo[ce.hi]);

	mapsTo.Invert();
	return mapsTo;
}

std::optional<std::vector<uint8_t>> Network::GetInput(const std::vector<uint8_t>& output) const
{
	uint8_t n = (uint8_t)output.size();

	// Get the binary outputs of the network
	OutputSet binOutputs{ FactoredOutputSet{ *this, n } };

	// Compute threshold masks and ensure each is a valid output
	std::vector<uint64_t> phiT(n + 1);
	for (uint8_t t = 0; t <= n; t++)
	{
		for (size_t i = 0; i < n; i++)
			if (output[i] >= t)
				phiT[t] |= (1ULL << i);

		if (!binOutputs.Contains(phiT[t]))
			return std::nullopt;
	}

	// Search for values s0, s1, ... sn that satisfy conditions (1) and (2) in the theorem
	std::vector<std::unordered_set<uint64_t>> sCandidates(n + 1);
	sCandidates[0].insert((1ULL << n) - 1);
	sCandidates[n].insert(0);
	for (uint8_t t = 1; t < n; t++)
	{
		for (uint64_t stm1 : sCandidates[t - 1])
		{
			for (uint8_t i = 0; i < n; i++)
			{
				if (~stm1 & (1ULL << i)) continue;
				uint64_t st = stm1 ^ (1ULL << i);
				if ((*this)(st) == phiT[t])
					sCandidates[t].insert(st);
			}
		}
		
		if (sCandidates[t].empty()) return std::nullopt;
	}

	// Reconstruct the chain
	std::vector<uint64_t> sChain(n + 1, 0);
	for (int8_t t = n - 1; t >= 0; t--)
	{
		for (uint8_t i = 0; i < n; i++)
		{
			sChain[t] = sChain[t + 1] | (1ULL << i);
			if (sCandidates[t].contains(sChain[t])) break;
		}
	}

	// Extract the input from the chain
	std::vector<uint8_t> input(n);
	for (uint8_t t = 0; t < n; t++)
	{
		uint64_t mask = ~sChain[t + 1] & sChain[t];
		uint64_t changePos = std::countr_zero(mask);
		input[changePos] = t;
	}

	return input;
}