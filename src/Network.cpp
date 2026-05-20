#include "Network.h"

Network Concatenate(const Network& a, const Network& b)
{
	Network ret{ a };
	Append(ret, b);
	return ret;
}

void Append(Network& a, const Network& b)
{
	a.insert(a.begin(), b.begin(), b.end());
}

uint8_t InferN(const Network& network)
{
	uint8_t maxHi = 0;
	for (auto [_, hi] : network)
		maxHi = std::max(maxHi, hi);
	return maxHi + 1;
}