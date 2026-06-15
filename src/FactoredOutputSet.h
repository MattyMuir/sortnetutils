#pragma once
#include "Network.h"

class FactoredOutputSet
{
protected:
	class BitVec
	{
	public:
		BitVec(size_t size);

		size_t Size() const;
		void Resize(size_t size);
		bool operator[](size_t idx) const;
		void SetBit(size_t idx);
		void ClearBitLazy(size_t idx);

	protected:
		std::vector<uint64_t> packs;
	};

public:
	FactoredOutputSet(const Network& network, uint8_t n);

	bool IsValid();
	size_t Size() const;
	std::vector<uint64_t> ToVector() const;
	bool IsFactored() const;

	void ApplyCE(uint8_t i, uint8_t j);
	void SwapBits(uint8_t i, uint8_t j);

protected:
	std::vector<std::vector<uint64_t>> clusters;
	std::vector<uint8_t> wireToCluster;

	void CombineClusters(uint8_t clusterIdx1, uint8_t clusterIdx2);
	void DoApplyCE(uint8_t i, uint8_t j);
	void ReorderWorklist(std::vector<CE>& worklist);

	friend class OutputSet;
};