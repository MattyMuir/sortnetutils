#include "exec.h"

#include <bit>

#include "FactoredOutputSet.h"

uint64_t RunNetwork(const Network& network, uint64_t input)
{
	for (auto [lo, hi] : network)
	{
		uint64_t loMask = 1ULL << lo;
		uint64_t hiMask = 1ULL << hi;
		if ((input & loMask) && (~input & hiMask))
			input ^= loMask | hiMask;
	}

	return input;
}

void RunNetwork(const Network& network, std::vector<uint64_t>& inputs)
{
	for (auto [lo, hi] : network)
	{
		uint64_t newLo = inputs[lo] & inputs[hi];
		inputs[hi] = inputs[lo] | inputs[hi];
		inputs[lo] = newLo;
	}
}

bool IsValid(const Network& network, uint8_t n)
{
	FactoredOutputSet outputs{ network, n };
	return outputs.Size() == n + 1;
}

bool IsSorted(uint8_t n, uint64_t output)
{
	uint64_t numZeros = n - std::popcount(output);
	uint64_t sorted = ((1ULL << n) - 1) ^ ((1ULL << numZeros) - 1);
	return output == sorted;
}

static inline uint64_t ReverseBits(uint64_t x)
{
	x = __builtin_bswap64(x);
	x = ((x >> 1) & 0x5555555555555555ULL) | ((x & 0x5555555555555555ULL) << 1);
	x = ((x >> 2) & 0x3333333333333333ULL) | ((x & 0x3333333333333333ULL) << 2);
	x = ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((x & 0x0F0F0F0F0F0F0F0FULL) << 4);
	return x;
}

bool HasSmallerMirror(uint8_t n, uint64_t input)
{
	uint64_t flipped = ~input;
	uint64_t reversed = ReverseBits(flipped) >> (64 - n);
	return input > reversed;
}

OutputSet GetOutputs(const Network& network, uint8_t n, bool onlyUnsorted, bool symmetric)
{
	OutputSet outputs{ FactoredOutputSet{ network, n } };
	if (!onlyUnsorted && !symmetric) return outputs;

	std::vector<uint64_t> reducedOutputs{ outputs.begin(), outputs.end() };
	if (onlyUnsorted)	std::erase_if(reducedOutputs, [n](uint64_t x) { return IsSorted(n, x); });
	if (symmetric)		std::erase_if(reducedOutputs, [n](uint64_t x) { return HasSmallerMirror(n, x); });

	return OutputSet{ n, reducedOutputs };
}