#pragma once
#include "Network.h"

uint8_t ComputeDepth(const Network& network);
LayeredNetwork GetLayers(const Network& network);
std::vector<Network> GetAllLayers(uint8_t n, bool symmetric);