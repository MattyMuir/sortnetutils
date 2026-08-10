#pragma once
#include "../Network/Network.h"

class FactoredOutputSet
{
public:
	FactoredOutputSet(const Network& network, uint8_t n);
	FactoredOutputSet(const std::vector<uint64_t>& outputs, uint8_t n);

	bool IsValid();
	size_t Size() const;
	std::vector<uint64_t> ToVector() const&;
	std::vector<uint64_t> ToVector() &&;
	bool IsFactored() const;
	bool IsRedundant() const;

	void ApplyCE(uint8_t i, uint8_t j);
	void ApplyCEs(const Network& ces);
	void SwapBits(uint8_t i, uint8_t j);

	std::vector<std::vector<uint64_t>> clusters;
	std::vector<uint8_t> wireToCluster;

protected:
	bool isRedundant = false;

	void CombineClusters(uint8_t clusterIdx1, uint8_t clusterIdx2);
	void DoApplyCE(uint8_t i, uint8_t j);
	void ReorderWorklist(std::vector<CE>& worklist);

	friend class OutputSet;
};