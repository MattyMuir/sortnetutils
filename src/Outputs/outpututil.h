#pragma once
#include "../Network/Network.h"

class OutputSet;

bool IsSorted(uint8_t n, uint64_t output);
uint64_t Mirror(uint8_t n, uint64_t x);
bool HasSmallerMirror(uint8_t n, uint64_t input);
OutputSet GetOutputs(const Network& network, uint8_t n, bool onlyUnsorted = false, bool symmetric = false);
uint64_t HashOutputs(const std::vector<uint64_t>& outputs);