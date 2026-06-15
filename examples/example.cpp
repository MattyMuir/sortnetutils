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

void SpeedTest()
{
	uint8_t n = 18;
	Network prefix = ParseNetwork(R"(
		[(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(16,17)]
		[(0,2),(1,3),(4,12),(5,13),(6,8),(9,11),(14,16),(15,17)]
		[(0,14),(1,16),(2,15),(3,17)]
		)");

	auto start = std::chrono::steady_clock::now();

#if 1
	for (size_t i = 0; i < 200'000; i++)
		FactoredOutputSet outputs{ prefix, n };
#else
	for (size_t i = 0; i < 20'000; i++)
		auto outputs = GetOutputs(prefix, n);
#endif

	auto end = std::chrono::steady_clock::now();
	auto duration = end - start;
	std::println("Took: {:.3f}ms", std::chrono::duration_cast<std::chrono::microseconds>(duration).count() * 1.0e-3);
}

int main()
{
	Network network = ParseNetwork("[(4,5),(0,5),(2,7),(2,5),(1,6),(1,5),(1,2),(0,7),(1,7),(1,7),(6,7),(0,5),(0,7),(2,3),(3,4),(6,7)]");

	std::println("{:t}", network);
}