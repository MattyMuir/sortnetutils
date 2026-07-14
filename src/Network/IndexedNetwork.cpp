#include "IndexedNetwork.h"

#include "Network.h"

IndexedNetwork::IndexedNetwork(const Network& network, uint8_t n)
{
	std::vector<uint64_t> usedChannels;
	for (CE ce : network)
	{
		// Get a mask of the channels used by this comparator
		uint64_t ceChannels = (1ULL << ce.lo) | (1ULL << ce.hi);

		// Push the comparator backwards until it cannot fit in a layer
		int64_t insertLayer = usedChannels.size() - 1;
		while (insertLayer >= 0 && !(usedChannels[insertLayer] & ceChannels))
			insertLayer--;
		insertLayer++;

		// Resize storage if this comparator is in a new layer
		if (insertLayer >= std::ssize(*this))
		{
			usedChannels.push_back(0);
			emplace_back(n, NotLo);
		}

		// Insert comparator into the layer
		usedChannels[insertLayer] |= ceChannels;
		(*this)[insertLayer][ce.lo] = ce.hi;
	}
}

bool IndexedNetwork::IsSymmetric() const
{
	uint8_t n = back().size();

	for (const std::vector<uint8_t>& layer : *this)
	{
		for (uint8_t lo = 0; lo < layer.size(); lo++)
		{
			if (layer[lo] == NotLo) continue;
			uint8_t hi = layer[lo];
			if (layer[n - 1 - hi] != n - 1 - lo)
				return false;
		}
	}
		
	return true;
}