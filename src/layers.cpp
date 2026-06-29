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

std::vector<Network> GetAllLayers(uint8_t n, bool symmetric)
{
	// Initialize alphabet
	std::vector<CE> alphabet;
	for (uint8_t i = 0; i + 1 < n; i++)
		for (uint8_t j = i + 1; j < n; j++)
			if (!symmetric || i <= n - 1 - j)
				alphabet.push_back({ i, j });

	// Generate all layers
	std::vector<Network> allLayers{ Network{} };
	for (CE ce : alphabet)
	{
		size_t numPartial = allLayers.size();
		for (size_t li = 0; li < numPartial; li++)
		{
			const Network& partial = allLayers[li];

			// Get a mask of all channels used in this partial layer
			uint64_t usedMask = 0;
			for (auto [i, j] : partial)
				usedMask |= (1ULL << i) | (1ULL << j);

			// Get the mask of the new CE
			uint64_t ceMask = (1ULL << ce.lo) | (1ULL << ce.hi);
			if (symmetric) ceMask |= (1ULL << (n - 1 - ce.hi)) | (1ULL << (n - 1 - ce.lo));

			// Skip this CE if it overlaps an existing one
			if (usedMask & ceMask) continue;

			// Create an extended layer by adding this CE
			Network extended{ partial };
			extended.push_back(ce);
			if (symmetric && ce.lo != n - 1 - ce.hi)
				extended.push_back({ (uint8_t)(n - 1 - ce.hi), (uint8_t)(n - 1 - ce.lo) });

			// Add to allLayers
			allLayers.emplace_back(extended);
		}
	}

	return allLayers;
}