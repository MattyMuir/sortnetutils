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

static inline uint64_t Transpose8x8(uint64_t m)
{
	uint64_t t;
	t = (m ^ (m >> 7)) & 0x00AA00AA00AA00AAULL; m ^= t ^ (t << 7);
	t = (m ^ (m >> 14)) & 0x0000CCCC0000CCCCULL; m ^= t ^ (t << 14);
	t = (m ^ (m >> 28)) & 0x00000000F0F0F0F0ULL; m ^= t ^ (t << 28);
	return m;
}

static inline void Transpose(uint64_t* transposed, uint8_t n, const std::vector<uint64_t>& outputs)
{
	memset(transposed, 0, 64 * sizeof(uint64_t));

	for (uint8_t groupBase = 0; groupBase < n; groupBase += 8)
	{
		const uint8_t groupEnd = std::min<uint8_t>(groupBase + 8, n);
		const uint8_t groupIdx = groupBase >> 3; // which byte lane within each transposed uint64_t

		for (uint8_t byteIdx = 0; byteIdx < 8; byteIdx++)
		{
			// Gather byte `byteIdx` from each channel in this group into one uint64_t
			// (one channel per byte of `packed`)
			uint64_t packed = 0;
			for (uint8_t g = groupBase; g < groupEnd; g++)
				packed |= (uint64_t)((outputs[g] >> (byteIdx * 8)) & 0xFF) << ((g - groupBase) * 8);

			// Transpose the 8×8 bit matrix: rows were channels, columns were bit positions
			// After transpose: row p = all 8 channels' contribution to output column byteIdx*8+p
			uint64_t tp = Transpose8x8(packed);

			// Scatter: write byte p of tp into byte `groupIdx` of transposed[byteIdx*8+p].
			// All 8 writes land in the same cache line.
			for (uint8_t p = 0; p < 8; p++)
				reinterpret_cast<uint8_t*>(&transposed[byteIdx * 8 + p])[groupIdx] = (uint8_t)(tp >> (p * 8));
		}
	}
}

std::vector<uint64_t> GetOutputs(const Network& network, uint8_t n, bool onlyUnsorted, bool symmetric)
{
	std::vector<bool> isOutput((1ULL << n), false);
	std::vector<uint64_t> allOutputs;

	std::vector<uint64_t> inputs{
		0b1010101010101010101010101010101010101010101010101010101010101010,
		0b1100110011001100110011001100110011001100110011001100110011001100,
		0b1111000011110000111100001111000011110000111100001111000011110000,
		0b1111111100000000111111110000000011111111000000001111111100000000,
		0b1111111111111111000000000000000011111111111111110000000000000000,
		0b1111111111111111111111111111111100000000000000000000000000000000
	};
	inputs.resize(n);
	std::vector<uint64_t> outputs;

	uint64_t numPacks = ((1ULL << n) + 63) / 64;
	for (uint64_t packIdx = 0; packIdx < numPacks; packIdx++)
	{
		// Update inputs based on the pack index
		for (uint8_t i = 0; (int)i < n - 6; i++)
			inputs[i + 6] = (packIdx & (1ULL << i)) ? (uint64_t)-1 : 0;

		// Run the network
		outputs = inputs;
		RunNetwork(network, outputs);

		// Transpose the outputs
		uint64_t transposed[64];
		Transpose(transposed, n, outputs);

		// Add the distinct and unsorted outputs
		for (uint64_t output : transposed)
		{
			if (onlyUnsorted && IsSorted(n, output)) continue;
			if (symmetric && HasSmallerMirror(n, output)) continue;
			if (!isOutput[output]) allOutputs.push_back(output);
			isOutput[output] = true;
		}
	}

	return allOutputs;
}