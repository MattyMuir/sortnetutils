#include <print>
#include <random>
#include <numeric>
#include <set>
#include <sortnetutils.h>

uint64_t RandomSelfMirror(uint8_t n)
{
	static std::mt19937_64 gen{ std::random_device{}() };
	std::uniform_int_distribution<uint64_t> dist{ 0, (1ULL << (n / 2)) - 1 };

	uint64_t x = dist(gen);
	uint64_t mirror = Mirror(n / 2, x);
	return x | (mirror << (n / 2));
}

void RunNetwork(const Network& network, std::vector<uint8_t>& arr)
{
	for (auto [i, j] : network)
		if (arr[i] > arr[j])
			std::swap(arr[i], arr[j]);
}

int main()
{
	// === Parameters ===
	uint8_t n = 8;
	Network network = ParseNetwork(R"(
		[(0,2),(1,3),(4,6),(5,7)]
		[(0,4),(1,5),(2,6),(3,7)]
		)");
	// ==================

	std::set<Permutation> allOutputPerms;

	Permutation perm(n);
	std::ranges::iota(perm, 0);
	do
	{
		allOutputPerms.insert(network.GetOutputPermutation(perm));
	} while (std::ranges::next_permutation(perm).found);

	std::println("Number of output permutations: {}", allOutputPerms.size());

	std::set<std::vector<uint8_t>> allOutputs;
	std::vector<uint8_t> input(n);
	std::ranges::iota(input, 0);
	do
	{
		auto output{ input };
		RunNetwork(network, output);
		allOutputs.insert(output);
	} while (std::ranges::next_permutation(input).found);

	std::println("Number of outputs: {}", allOutputs.size());
}