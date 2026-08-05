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

const std::vector<uint64_t>& OutputSet::ToVector() const
{
	return outputs;
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

OutputSet OutputSet::Permute(const OutputSet& set, const Permutation& perm)
{
	uint8_t n = perm.size();

	OutputSet permuted{ n };
	permuted.Reserve(set.Size());
	for (uint64_t output : set)
		permuted.Insert(perm(output));

	return permuted;
}

bool OutputSet::StrictSubset(const OutputSet& a, const OutputSet& b)
{
	if (a.Size() >= b.Size()) return false;

	for (uint64_t x : a)
		if (!b.Contains(x))
			return false;
	return true;
}

size_t OutputSetHasher::operator()(const OutputSet& set) const
{
	return HashOutputs(set.outputs);
}