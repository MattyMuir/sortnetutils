#include <print>
#include <random>
#include <numeric>
#include <set>
#include <unordered_set>
#include <chrono>

#include <sortnetutils.h>

void RunNetwork(const Network& network, std::vector<uint8_t>& arr)
{
	for (auto [i, j] : network)
		if (arr[i] > arr[j])
			std::swap(arr[i], arr[j]);
}

int main()
{
	uint8_t n = 16;
	Network prefix = ParseNetwork(R"(
		[(0,5),(1,4),(2,12),(3,13),(6,7),(8,9),(10,15),(11,14)]
		[(0,2),(1,10),(3,6),(4,7),(5,14),(8,11),(9,12),(13,15)]
		[(0,8),(1,3),(2,11),(4,13),(5,9),(6,10),(7,15),(12,14)]
		)");

	std::vector<uint8_t> input(n);
	std::ranges::iota(input, 0);
	static std::mt19937_64 gen{ std::random_device{}() };
	std::ranges::shuffle(input, gen);

	std::vector<uint8_t> output{ input };
	RunNetwork(prefix, output);

	auto inputOpt = prefix.GetInput(output);
	std::vector<uint8_t> outputRT{ *inputOpt };
	RunNetwork(prefix, outputRT);

	std::println("Output:    {}", output);
	std::println("Output RT: {}", outputRT);
}