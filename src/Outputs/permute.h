#pragma once
#include "OutputSet.h"

uint64_t Permute(uint64_t x, const std::vector<uint8_t>& perm);
OutputSet Permute(const OutputSet& outputs, const std::vector<uint8_t>& perm);