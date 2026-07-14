#pragma once
#include "../Network/Network.h"
#include "OutputSet.h"

bool IsSorted(uint8_t n, uint64_t output);
bool HasSmallerMirror(uint8_t n, uint64_t input);
OutputSet GetOutputs(const Network& network, uint8_t n, bool onlyUnsorted = false, bool symmetric = false);