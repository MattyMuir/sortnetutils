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

void AddCE(Network& network, uint8_t i, uint8_t j)
{
	network.push_back({ (uint8_t)(i - 1), (uint8_t)(j - 1) });
}

Network CanonicalFL(uint8_t n, uint8_t k)
{
	Network L;
	for (uint8_t i = 1; i <= n / 2 - k * 2; i++)
		AddCE(L, i, n + 1 - i);

	for (uint8_t i = 1; i <= k * 2; i++)
		AddCE(L, n / 2 - k * 2 + i, n / 2 + i);

	return L;
}

int main()
{
	Network a = ParseNetwork("(0,1),(1,2),(3,4)");
	Network b = ParseNetwork("(3,4),(1,2),(0,1)");

	bool areIdentical = Network::Identical(a, b);
	std::println("Identical: {}", areIdentical);

	std::println("{}\n", a);
	std::println("{:l}\n", a);
	std::println("{:t}\n", a);
}