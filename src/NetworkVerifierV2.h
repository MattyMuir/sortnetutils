#pragma once
#include "Network.h"

class NetworkVerifierV2
{
public:
	NetworkVerifierV2(const Network& network, uint8_t n_);

	bool IsValid();

protected:
	std::vector<CE> worklist;
	uint8_t n;

	std::vector<std::vector<uint64_t>> clusters;
	std::vector<uint8_t> wireToCluster;
	std::vector<bool> hasPattern;

	void CombineClusters(uint8_t clusterIdx1, uint8_t clusterIdx2);
	void ApplyCE(uint8_t i, uint8_t j);
	size_t NumOutputs() const;
	void ReorderWorklist();
};