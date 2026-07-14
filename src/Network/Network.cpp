#include "Network.h"

#include <algorithm>
#include <numeric>

#include "LayeredNetwork.h"
#include "IndexedNetwork.h"
#include "../Outputs/FactoredOutputSet.h"

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
	for (auto [_, hi] : *this)
		maxHi = std::max(maxHi, hi);
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