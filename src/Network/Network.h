#pragma once
#include <vector>
#include <format>

#include "CE.h"

class LayeredNetwork;

class Network : public std::vector<CE>
{
public:
	using std::vector<CE>::vector;
	explicit Network(const LayeredNetwork& network);

	bool operator==(const Network& other) const = delete;
	Network operator+(const Network& other) const;
	Network& operator+=(const Network& other);
	uint64_t operator()(uint64_t x) const;

	uint8_t InferN() const;
	size_t Depth() const;
	bool IsGeneralized() const;
	bool IsSymmetric(uint8_t n) const;
	bool IsSorting(uint8_t n) const;
	static bool Identical(const Network& a, const Network& b);

	/// All permutations use the 'gather' convention:
	/// perm = [3, ...] means that position 0 gets its value from position 3 after the permutation is applied
	/// i.e. src = 3, dst = 0
	/// i.e. perm[dst] = src
	void Permute(const std::vector<uint8_t>& perm);
	void Untangle();
};