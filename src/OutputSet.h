#pragma once
#include <vector>

#include "FactoredOutputSet.h"

class OutputSet
{
public:
	OutputSet(const FactoredOutputSet& outputSet);
	OutputSet(FactoredOutputSet&& outputSet);
	OutputSet(uint8_t n, const std::vector<uint64_t>& outputs_);

	bool operator==(const OutputSet& other) const;

	const uint64_t* begin() const;
	const uint64_t* end() const;

	size_t Size() const;
	bool IsEmpty() const;
	bool Contains(uint64_t x) const;

protected:
	std::vector<uint64_t> outputs;
	std::vector<bool> containsOutput;
};