#include "permute.h"

#include <cassert>
#include <numeric>

bool IsValidPerm(const std::vector<uint8_t>& perm)
{
	std::vector<bool> seen(perm.size(), false);
	for (uint8_t v : perm)
	{
		if (seen[v]) return false;
		seen[v] = true;
	}
	return true;
}

std::vector<uint8_t> InvertPerm(const std::vector<uint8_t>& perm)
{
	assert(IsValidPerm(perm));

	std::vector<uint8_t> inverse(perm.size());
	for (uint8_t dst = 0; dst < (uint8_t)perm.size(); dst++)
	{
		uint8_t src = perm[dst];
		inverse[src] = dst;
	}
	return inverse;
}

void Permute(Network& network, const std::vector<uint8_t>& perm)
{
	assert(IsValidPerm(perm));

	std::vector<uint8_t> mapsTo = InvertPerm(perm);
	for (CE& ce : network)
	{
		uint8_t newLo = mapsTo[ce.lo];
		uint8_t newHi = mapsTo[ce.hi];
		ce = { newLo, newHi };
	}
}

void Untangle(Network& network, uint8_t n)
{
	std::vector<uint8_t> mapsTo(n);
	std::iota(mapsTo.begin(), mapsTo.end(), 0);

	for (CE& ce : network)
	{
		int from = mapsTo[ce.lo];
		int to = mapsTo[ce.hi];
		if (from > to)
		{
			// All further occurcences of "from" will be replaced by "to", and vice versa
			mapsTo[ce.lo] = to;
			mapsTo[ce.hi] = from;
			// Turn direction of comparator
			ce.lo = to;
			ce.hi = from;
		}
		else
		{
			// Correct direction, nothing to do
			ce.lo = from;
			ce.hi = to;
		}
	}
}

static inline uint64_t PermuteOutput(uint64_t x, const std::vector<uint8_t>& perm)
{
	size_t n = perm.size();

	uint64_t permuted = 0;
	for (size_t i = 0; i < perm.size(); i++)
	{
		size_t src = perm[n - i - 1];
		permuted <<= 1;
		permuted |= (x >> src) & 1;
	}

	return permuted;
}

OutputSet Permute(const OutputSet& outputs, const std::vector<uint8_t>& perm)
{
	uint8_t n = perm.size();

	OutputSet permuted{ n };
	for (uint64_t output : outputs)
		permuted.Insert(PermuteOutput(output, perm));

	return permuted;
}