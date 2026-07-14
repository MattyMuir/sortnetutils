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

	size_t NetworkSize() const;
};