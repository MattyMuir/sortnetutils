#include "permute.h"

uint64_t Permute(uint64_t x, const std::vector<uint8_t>& perm)
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
	permuted.Reserve(outputs.Size());
	for (uint64_t output : outputs)
		permuted.Insert(Permute(output, perm));

	return permuted;
}