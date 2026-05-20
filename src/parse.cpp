#include "parse.h"

#include <ranges>

enum ParseType
{
	Default,
	ExplicitLayers
};

static inline std::vector<uint32_t> ExtractInts(const std::string& str)
{
	std::vector<uint32_t> results;
	const char* ptr = str.data();
	const char* end = ptr + str.size();

	while (ptr < end)
	{
		// Advance to the next digit
		ptr = std::find_if(ptr, end, [](char c) { return std::isdigit(c); });
		if (ptr == end) break;

		// Parse the digit block and advance past it
		uint32_t value{};
		auto [next, ec] = std::from_chars(ptr, end, value);
		if (ec == std::errc{}) results.push_back(value);
		ptr = next;
	}

	return results;
}

static inline Network ParseExplicitLayers(const std::string& str)
{
	std::vector<uint32_t> idxs = ExtractInts(str);

	std::vector<Network> layers;
	for (size_t ceIdx = 0; ceIdx < idxs.size(); ceIdx += 3)
	{
		uint8_t k = idxs[ceIdx + 0];
		uint8_t i = idxs[ceIdx + 1];
		uint8_t j = idxs[ceIdx + 2];

		if (layers.size() <= k) layers.resize(k + 1);
		layers[k].push_back({ i, j });
	}

	Network network;
	for (const Network& layer : layers) Append(network, layer);

	return network;
}

static inline Network ParseDefault(const std::string& str)
{
	std::vector<uint32_t> idxs = ExtractInts(str);

	Network network;
	for (size_t ceIdx = 0; ceIdx < idxs.size(); ceIdx += 2)
	{
		uint8_t i = idxs[ceIdx + 0];
		uint8_t j = idxs[ceIdx + 1];
		network.push_back({ i, j });
	}

	return network;
}

static inline ParseType InferType(const std::string& str)
{
	auto it = std::find_if(str.begin(), str.end(), [](char c) { return c == '(' || c == ')'; });
	bool hasParens = (it != str.end());
	return hasParens ? Default : ExplicitLayers;
}

Network ParseNetwork(const std::string& str)
{
	ParseType type = InferType(str);
	switch (type)
	{
	case Default: return ParseDefault(str);
	case ExplicitLayers: return ParseExplicitLayers(str);
	}
}