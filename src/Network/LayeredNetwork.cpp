#include "LayeredNetwork.h"

#include "Network.h"

LayeredNetwork::LayeredNetwork(const Network& network)
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
			push_back({});
		}

		// Insert comparator into the layer
		usedChannels[insertLayer] |= ceChannels;
		(*this)[insertLayer].push_back(ce);
	}
}

LayeredNetwork LayeredNetwork::operator+(const Network& layer) const
{
	LayeredNetwork ret{ *this };
	ret.emplace_back(layer);
	return ret;
}

LayeredNetwork& LayeredNetwork::operator+=(const Network& layer)
{
	emplace_back(layer);
	return *this;
}

size_t LayeredNetwork::NetworkSize() const
{
	size_t size = 0;
	for (const Network& layer : *this)
		size += layer.size();
	return size;
}