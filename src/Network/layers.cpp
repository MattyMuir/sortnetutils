#include "layers.h"

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