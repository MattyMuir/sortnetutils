#pragma once
#include "Network.h"

CE RandomCE(uint8_t n, bool symmetric);
Network RandomNetworkSized(uint8_t n, size_t size, bool symmetric);
Network RandomNetworkLayered(uint8_t n, size_t depth, bool symmetric);