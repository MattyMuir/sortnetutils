#include "Network.h"

Network Concatenate(const Network& a, const Network& b)
{
	Network ret{ a };
	ret.insert(ret.end(), b.begin(), b.end());
	return ret;
}

uint8_t InferN(const Network& network)
{
	uint8_t maxHi = 0;
	for (auto [_, hi] : network)
		maxHi = std::max(maxHi, hi);
	return maxHi + 1;
}