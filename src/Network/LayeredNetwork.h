#pragma once
#include <vector>

#include "CE.h"

class Network;
class IndexedNetwork;

class LayeredNetwork : public std::vector<Network>
{
public:
	LayeredNetwork() = default;
	explicit LayeredNetwork(const Network& network);

	LayeredNetwork operator+(const Network& layer) const;
	LayeredNetwork& operator+=(const Network& layer);

	size_t NetworkSize() const;
};