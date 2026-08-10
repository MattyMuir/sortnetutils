#include "BitVec.h"

BitVec::BitVec(size_t size)
	: packs((size + 63) / 64, 0)
{}

size_t BitVec::Size() const
{
	return packs.size() * 64;
}

void BitVec::Resize(size_t size)
{
	packs.resize((size + 63) / 64, 0);
}

bool BitVec::operator[](size_t idx) const
{
	size_t packIdx = idx / 64;
	size_t packOff = idx % 64;
	return packs[packIdx] & (1ULL << packOff);
}

void BitVec::SetBit(size_t idx)
{
	size_t packIdx = idx / 64;
	size_t packOff = idx % 64;
	packs[packIdx] |= (1ULL << packOff);
}

void BitVec::ClearBitLazy(size_t idx)
{
	packs[idx / 64] = 0;
}