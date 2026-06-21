#include "Network.h"

#include <algorithm>

Network Concatenate(const Network& a, const Network& b)
{
	Network ret{ a };
	Append(ret, b);
	return ret;
}

Network Concatenate(const LayeredNetwork& layers)
{
	Network ret;
	ret.reserve(GetNetworkSize(layers));

	for (const Network& layer : layers)
		Append(ret, layer);
	return ret;
}

void Append(Network& a, const Network& b)
{
	a.insert(a.end(), b.begin(), b.end());
}

size_t GetNetworkSize(const LayeredNetwork& network)
{
	size_t size = 0;
	for (const Network& layer : network)
		size += layer.size();
	return size;
}

uint8_t InferN(const Network& network)
{
	uint8_t maxHi = 0;
	for (auto [_, hi] : network)
		maxHi = std::max(maxHi, hi);
	return maxHi + 1;
}

uint8_t InferN(const LayeredNetwork& layers)
{
	uint8_t maxHi = 0;
	for (const Network& layer : layers)
		for (auto [_, hi] : layer)
			maxHi = std::max(maxHi, hi);
	return maxHi + 1;
}

bool IsGeneralized(const Network& network)
{
	return std::ranges::any_of(network, [](CE ce) { return ce.lo > ce.hi; });
}