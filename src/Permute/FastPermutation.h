#pragma once
#include "Permutation.h"

class FastPermutation
{
public:
    FastPermutation(uint8_t n);

    void Assign(const Permutation& perm);

    uint64_t operator()(uint64_t x) const;

protected:
    size_t numChunks;
    std::vector<uint64_t> tables;
};