#include "Network.h"

Network Concatenate(const Network& a, const Network& b)
{
	Network ret{ a };
	Append(ret, b);
	return ret;
}

Network Concatenate(const std::vector<Network>& layers)
{
	if (layers.empty()) return {};

	Network ret{ layers[0] };
	for (size_t layerIdx = 1; layerIdx < layers.size(); layerIdx++)
		Append(ret, layers[layerIdx]);
	return ret;
}

void Append(Network& a, const Network& b)
{
	a.insert(a.end(), b.begin(), b.end());
}

uint8_t InferN(const Network& network)
{
	uint8_t maxHi = 0;
	for (auto [_, hi] : network)
		maxHi = std::max(maxHi, hi);
	return maxHi + 1;
}

uint8_t InferN(const std::vector<Network>& layers)
{
	uint8_t maxHi = 0;
	for (const Network& layer : layers)
		for (auto [_, hi] : layer)
			maxHi = std::max(maxHi, hi);
	return maxHi + 1;
}