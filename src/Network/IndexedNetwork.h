#pragma once
#include <vector>

#include "CE.h"

class Network;
class LayeredNetwork;

class IndexedNetwork : public std::vector<std::vector<uint8_t>>
{
public:
	static constexpr uint8_t NotLo = 255;

public:
	explicit IndexedNetwork(const Network& network, uint8_t n);

	bool IsSymmetric() const;
};