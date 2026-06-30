#include <print>
#include <chrono>
#include <random>
#include <fstream>
#include <format>
#include <numeric>
#include <bit>
#include <algorithm>
#include <set>

#include <sortnetutils.h>

Network RandomNetwork(uint8_t n, size_t size)
{
	std::vector<CE> alphabet;
	for (uint8_t i = 0; i + 1 < n; i++)
		for (uint8_t j = i + 1; j < n; j++)
			alphabet.push_back({ i, j });

	static std::mt19937_64 gen{ std::random_device{}() };
	std::uniform_int_distribution<size_t> dist{ 0, alphabet.size() - 1 };

	Network network;
	for (size_t i = 0; i < size; i++)
		network.push_back(alphabet[dist(gen)]);

	return network;
}

void OutputsToCSV(const std::string& filepath, const OutputSet& outputs, uint8_t n)
{
	std::ofstream file{ filepath, std::ios::binary };

	// Create bit vertices
	for (uint8_t bi = 0; bi < n; bi++)
		file << std::format("{},,bit\n", bi);

	size_t vertexIdx = n;
	for (uint64_t output : outputs)
	{
		// Specify vertex color
		file << std::format("{},,output\n", vertexIdx);

		// Add edges
		for (uint8_t bi = 0; bi < n; bi++)
			if (output & (1ULL << bi))
				file << std::format("{},{}\n", vertexIdx, bi);

		vertexIdx++;
	}
}



int main()
{
	for (uint8_t n = 2; n <= 18; n += 2)
	{
		auto allLayers = GetAllLayers(n, true);

		size_t numFull = 0;
		for (const Network& layer : allLayers)
			if (layer.size() == n / 2)
				numFull++;
		std::println("{}: {}/{} = {:.3f}", n, numFull, allLayers.size(), (double)numFull / allLayers.size());
	}
}