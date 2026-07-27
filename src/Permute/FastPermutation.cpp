#include "FastPermutation.h"

#include <bit>

FastPermutation::FastPermutation(uint8_t n)
    : numChunks((n + 3) / 4), tables(numChunks * 16) {}

void FastPermutation::Assign(const Permutation& perm)
{
    uint8_t n = perm.size();

    uint64_t* contribution = (uint64_t*)__builtin_alloca(numChunks * 4 * sizeof(uint64_t));
    memset(contribution, 0, numChunks * 4 * sizeof(uint64_t));
    for (uint8_t dst = 0; dst < n; dst++)
        if (perm[dst] < n)
            contribution[perm[dst]] = 1ULL << dst;

    for (size_t chunkIdx = 0; chunkIdx < numChunks; chunkIdx++)
    {
        uint64_t* chunkTable = tables.data() + chunkIdx * 16;
        uint64_t* contrib = contribution + chunkIdx * 4;
        for (uint32_t v = 1; v < 16; v++)
        {
            uint32_t low = v & (-v);
            uint32_t lowIdx = std::countr_zero(low);
            chunkTable[v] = chunkTable[v & (v - 1)] | contrib[lowIdx];
        }
    }
}

uint64_t FastPermutation::operator()(uint64_t x) const
{
    uint64_t result = 0;
    for (size_t chunkIdx = 0; chunkIdx < numChunks; chunkIdx++)
    {
        const uint64_t* chunkTable = tables.data() + chunkIdx * 16;
        uint8_t nibble = (x >> (chunkIdx * 4)) & 0xF;
        result |= chunkTable[nibble];
    }
    return result;
}