#include "layers.h"

uint8_t ComputeDepth(const Network& network)
{
	std::vector<uint64_t> usedChannels;

	for (CE ce : network)
	{
		// Get a mask of the channels used by this comparator
		uint64_t ceChannels = (1ULL << ce.lo) | (1ULL << ce.hi);

		// Push the comparator backwards until it cannot fit in a layer
		int64_t insertLayer;
		for (insertLayer = usedChannels.size() - 1; insertLayer >= 0; insertLayer--)
			if (usedChannels[insertLayer] & ceChannels)
				break;
		insertLayer++;

		// Resize storage if this comparator is in a new layer
		if (insertLayer >= std::ssize(usedChannels))
			usedChannels.push_back(0);

		// Insert comparator into the layer
		usedChannels[insertLayer] |= ceChannels;
	}

	return usedChannels.size();
}

LayeredNetwork GetLayers(const Network& network)
{
	std::vector<uint64_t> usedChannels;
	LayeredNetwork layers;

	for (CE ce : network)
	{
		// Get a mask of the channels used by this comparator
		uint64_t ceChannels = (1ULL << ce.lo) | (1ULL << ce.hi);

		// Push the comparator backwards until it cannot fit in a layer
		int64_t insertLayer;
		for (insertLayer = layers.size() - 1; insertLayer >= 0; insertLayer--)
			if (usedChannels[insertLayer] & ceChannels)
				break;
		insertLayer++;

		// Resize storage if this comparator is in a new layer
		if (insertLayer >= std::ssize(layers))
		{
			usedChannels.push_back(0);
			layers.push_back({});
		}

		// Insert comparator into the layer
		usedChannels[insertLayer] |= ceChannels;
		layers[insertLayer].push_back(ce);
	}

	return layers;
}