#pragma once
#include <vector>

#include "FactoredOutputSet.h"
#include "../Permute/Permutation.h"
#include "outpututil.h"

class OutputSet
{
public:
	OutputSet(uint8_t n);
	OutputSet(uint8_t n, const std::vector<uint64_t>& outputs_);
	explicit OutputSet(const FactoredOutputSet& outputSet);
	explicit OutputSet(FactoredOutputSet&& outputSet);

	bool operator==(const OutputSet& other) const;

	const uint64_t* begin() const;
	const uint64_t* end() const;
	uint64_t operator[](size_t idx) const;
	const std::vector<uint64_t>& ToVector() const;

	size_t Size() const;
	bool IsEmpty() const;
	bool Contains(uint64_t x) const;

	void Reserve(uint64_t size);
	void Insert(uint64_t x);

	static OutputSet Permute(const OutputSet& set, const Permutation& perm);
	static bool StrictSubset(const OutputSet& a, const OutputSet& b);

protected:
	std::vector<uint64_t> outputs;
	std::vector<bool> containsOutput;

	friend struct OutputSetHasher;
};

struct OutputSetHasher
{
	size_t operator()(const OutputSet& set) const;
};