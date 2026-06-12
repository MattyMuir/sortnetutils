#include "OutputSet.h"

OutputSet::OutputSet(const FactoredOutputSet& outputSet)
	: containsOutput(1ULL << outputSet.wireToCluster.size())
{
	size_t numOutputs = outputSet.Size();

	// The first cluster will always be non-empty, copy its elements
	outputs.reserve(numOutputs);
	outputs.insert(outputs.begin(), outputSet.clusters[0].begin(), outputSet.clusters[0].end());

	for (size_t clusterIdx = 1; clusterIdx < outputSet.clusters.size(); clusterIdx++)
	{
		const std::vector<uint64_t>& cluster = outputSet.clusters[clusterIdx];

		// Skip empty clusters
		if (cluster.empty()) continue;

		size_t clusterSize1 = outputs.size();
		for (size_t pi2 = 1; pi2 < cluster.size(); pi2++)
			for (size_t pi1 = 0; pi1 < clusterSize1; pi1++)
				outputs.push_back(outputs[pi1] | cluster[pi2]);
	}

	for (uint64_t output : outputs)
		containsOutput[output] = true;
}

OutputSet::OutputSet(FactoredOutputSet&& outputSet)
	: outputs(std::move(outputSet.clusters[0])), containsOutput(1ULL << outputSet.wireToCluster.size())
{
	// Reserve enough space for all outputs
	size_t numOutputs = outputSet.Size();
	outputs.reserve(numOutputs);

	for (size_t clusterIdx = 1; clusterIdx < outputSet.clusters.size(); clusterIdx++)
	{
		const std::vector<uint64_t>& cluster = outputSet.clusters[clusterIdx];

		// Skip empty clusters
		if (cluster.empty()) continue;

		size_t clusterSize1 = outputs.size();
		for (size_t pi2 = 1; pi2 < cluster.size(); pi2++)
			for (size_t pi1 = 0; pi1 < clusterSize1; pi1++)
				outputs.push_back(outputs[pi1] | cluster[pi2]);
	}

	for (uint64_t output : outputs)
		containsOutput[output] = true;
}

OutputSet::OutputSet(uint8_t n, const std::vector<uint64_t>& outputs_)
	: outputs(outputs_), containsOutput(1ULL << n)
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