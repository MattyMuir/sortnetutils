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
	uint8_t n = 18;
	Network prefix = ParseNetwork(R"(
		[(0,6),(1,10),(2,15),(3,5),(4,9),(7,16),(8,13),(11,17),(12,14)]
		[(0,12),(1,4),(3,11),(5,17),(6,14),(7,8),(9,10),(13,16)]
		[(1,13),(2,7),(4,16),(6,9),(8,11),(10,15)]
		)");

	std::println("Num outputs: {}", GetOutputs(prefix, n).size());
	std::println("Num outputs: {}", FactoredOutputSet{ prefix, n }.Size());

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