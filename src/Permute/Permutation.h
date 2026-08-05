#pragma once
#include <vector>

/// All permutations use the 'gather' convention:
/// perm = [3, ...] means that position 0 gets its value from position 3 after the permutation is applied
/// i.e. src = 3, dst = 0
/// i.e. perm[dst] = src
class Permutation : public std::vector<uint8_t>
{
public:
	using std::vector<uint8_t>::vector;

	uint64_t operator()(uint64_t x) const;

	void Invert();
	bool IsSymmetric() const;
};