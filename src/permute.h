#pragma once
#include "Network.h"
#include "OutputSet.h"

// All permutations use the 'gather' convention:
// perm = [3, ...] means that position 0 gets its value from position 3 after the permutation is applied
// i.e. src = 3, dst = 0
// i.e. perm[dst] = src

bool IsValidPerm(const std::vector<uint8_t>& perm);
std::vector<uint8_t> InvertPerm(const std::vector<uint8_t>& perm);
void Permute(Network& network, const std::vector<uint8_t>& perm);
void Untangle(Network& network, uint8_t n);
uint64_t Permute(uint64_t x, const std::vector<uint8_t>& perm);
OutputSet Permute(const OutputSet& outputs, const std::vector<uint8_t>& perm);