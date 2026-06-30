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
	Network network = ParseNetwork("(0,1),(0,2),(1,2),(0,1)");
	FactoredOutputSet outputs{ network, 3 };
	std::println("Redundant: {}", outputs.IsRedundant());
}