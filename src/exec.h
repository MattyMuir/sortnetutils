#pragma once
#include "Network.h"

uint64_t RunNetwork(const Network& network, uint64_t input);
void RunNetwork(const Network& network, std::vector<uint64_t>& inputs);
bool IsValid(const Network& network, uint8_t n);
bool IsSorted(uint8_t n, uint64_t output);
bool HasSmallerMirror(uint8_t n, uint64_t input);
std::vector<uint64_t> GetOutputs(const Network& network, uint8_t n, bool onlyUnsorted = false, bool symmetric = false);