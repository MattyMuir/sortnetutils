#include <print>
#include <chrono>
#include <random>

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

int main()
{
	uint8_t n = 10;
	size_t size = 20;

	for (;;)
	{
		Network network = RandomNetwork(n, size);
		std::println("{}", network);

		auto outputs1 = GetOutputs(network, n);
		FactoredOutputSet outputs2{ network, n };

		if (outputs1.size() != outputs2.Size()) break;
	}
}