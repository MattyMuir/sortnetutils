#include "symmetry.h"

#include "layers.h"

bool IsSymmetric(const Network& network, uint8_t n)
{
	std::vector<Network> layers = GetLayers(network);

	for (const Network& layer : layers)
	{
		std::vector<uint8_t> hiFromLo(n);
		for (auto [lo, hi] : layer)
			hiFromLo[lo] = hi;

		for (auto [lo, hi] : layer)
			if (hiFromLo[n - 1 - hi] != n - 1 - lo)
				return false;
	}

	return true;
}