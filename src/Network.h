#pragma once
#include <vector>
#include <format>

#include "CE.h"

using Network = std::vector<CE>;

Network Concatenate(const Network& a, const Network& b);
void Append(Network& a, const Network& b);
uint8_t InferN(const Network& network);
uint8_t InferN(const std::vector<Network>& layers);