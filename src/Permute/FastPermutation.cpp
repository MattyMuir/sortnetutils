#include "FastPermutation.h"

FastPermutation::FastPermutation(const Permutation& perm)
    : numChunks((perm.size() + 7) / 8), tables(numChunks * 256)
{
    for (uint8_t dst = 0; dst < perm.size(); dst++)
    {
        uint8_t src = perm[dst];
        if (src >= perm.size()) continue;
        size_t chunkIdx = src / 8;
        uint8_t bitOffset = src % 8;
        uint64_t dstMask = 1ULL << dst;

        uint64_t* chunkTable = tables.data() + chunkIdx * 256;
        for (uint32_t v = 0; v < 256; ++v)
            if ((v >> bitOffset) & 1ULL)
                chunkTable[v] |= dstMask;
    }
}

uint64_t FastPermutation::operator()(uint64_t x) const
{
    uint64_t result = 0;
    for (size_t chunkIdx = 0; chunkIdx < numChunks; chunkIdx++)
    {
        const uint64_t* chunkTable = tables.data() + chunkIdx * 256;
        uint8_t byte = x >> (chunkIdx * 8);
        result |= chunkTable[byte];
    }
    return result;
}