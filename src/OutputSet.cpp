#include "OutputSet.h"

OutputSet::OutputSet(uint8_t n)
	: containsOutput(1ULL << n) {}

OutputSet::OutputSet(uint8_t n, const std::vector<uint64_t>& outputs_)
	: outputs(outputs_), containsOutput(1ULL << n)
{
	for (uint64_t output : outputs)
		containsOutput[output] = true;
}

OutputSet::OutputSet(const FactoredOutputSet& outputSet)
	: outputs(outputSet.ToVector()), containsOutput(1ULL << outputSet.wireToCluster.size())
{
	for (uint64_t output : outputs)
		containsOutput[output] = true;
}

OutputSet::OutputSet(FactoredOutputSet&& outputSet)
	: outputs(std::move(outputSet).ToVector()), containsOutput(1ULL << outputSet.wireToCluster.size())
{
	for (uint64_t output : outputs)
		containsOutput[output] = true;
}

bool OutputSet::operator==(const OutputSet& other) const
{
	return containsOutput == other.containsOutput;
}

const uint64_t* OutputSet::begin() const
{
	return outputs.data();
}

const uint64_t* OutputSet::end() const
{
	return outputs.data() + outputs.size();
}

uint64_t OutputSet::operator[](size_t idx) const
{
	return outputs[idx];
}

size_t OutputSet::Size() const
{
	return outputs.size();
}

bool OutputSet::IsEmpty() const
{
	return outputs.empty();
}

bool OutputSet::Contains(uint64_t x) const
{
	return containsOutput[x];
}

void OutputSet::Reserve(uint64_t size)
{
	outputs.reserve(size);
}

void OutputSet::Insert(uint64_t x)
{
	outputs.push_back(x);
	containsOutput[x] = true;
}

// An associative hash combination function
static inline size_t HashCombine(size_t a, size_t b)
{
	b ^= b >> 30;
	b *= 0xbf58476d1ce4e5b9ULL;
	b ^= b >> 27;
	b *= 0x94d049bb133111ebULL;
	b ^= b >> 31;
	return a + b;
}

size_t OutputSetHasher::operator()(const OutputSet& set) const
{
	size_t hash = 0;
	for (uint64_t x : set)
		hash = HashCombine(hash, x);
	return hash;
}