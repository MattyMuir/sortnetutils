#include "Permutation.h"

uint64_t Permutation::operator()(uint64_t x) const
{
	size_t n = size();

	uint64_t permuted = 0;
	for (size_t i = 0; i < n; i++)
	{
		size_t src = (*this)[n - i - 1];
		permuted <<= 1;
		permuted |= (x >> src) & 1;
	}

	return permuted;
}

void Permutation::Invert()
{
	Permutation inverted(size());
	for (uint8_t dst = 0; dst < size(); dst++)
		inverted[(*this)[dst]] = dst;
	std::swap(*this, inverted);
}