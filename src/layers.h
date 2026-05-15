#pragma once
#include "Network.h"

uint8_t ComputeDepth(const Network& network);
std::vector<Network> GetLayers(const Network& network);