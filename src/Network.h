#pragma once
#include <vector>
#include <format>

#include "CE.h"

using Network = std::vector<CE>;
using LayeredNetwork = std::vector<Network>;

template <typename T>
concept NetworkTy = std::is_same_v<T, Network> || std::is_same_v<T, LayeredNetwork>;

Network Concatenate(const Network& a, const Network& b);
Network Concatenate(const LayeredNetwork& layers);
void Append(Network& a, const Network& b);
size_t GetNetworkSize(const LayeredNetwork& network);
uint8_t InferN(const Network& network);
uint8_t InferN(const LayeredNetwork& layers);
bool IsGeneralized(const Network& network);