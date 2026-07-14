#pragma once
#include <string>

#include "../Network/LayeredNetwork.h"

std::string GenerateTikz(const LayeredNetwork& network, uint8_t n);