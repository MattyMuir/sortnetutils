#pragma once
#include <vector>

class Permutation : public std::vector<uint8_t>
{
public:
	using std::vector<uint8_t>::vector;

	uint64_t operator()(uint64_t x) const;

	void Invert();
};