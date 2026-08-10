#pragma once
#include <vector>

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