#include <print>
#include <random>
#include <numeric>
#include <sortnetutils.h>

uint64_t RandomSelfMirror(uint8_t n)
{
	static std::mt19937_64 gen{ std::random_device{}() };
	std::uniform_int_distribution<uint64_t> dist{ 0, (1ULL << (n / 2)) - 1 };

	uint64_t x = dist(gen);
	uint64_t mirror = Mirror(n / 2, x);
	return x | (mirror << (n / 2));
}

int main()
{
	// === Parameters ===
	uint8_t n = 8;
	size_t depth = 3;
	// ==================

	static std::mt19937_64 gen{ std::random_device{}() };
	for (;;)
	{
		// Generate random network and permutation
		Network network = RandomNetworkLayered(n, depth, false);
		Permutation perm(n);
		std::ranges::iota(perm, 0);
		std::ranges::shuffle(perm, gen);

		// Get original outputs
		std::vector<uint64_t> outputs = FactoredOutputSet{ network, n }.ToVector();

		// Permute and untangle the network
		Network permuted{ network };
		permuted.Permute(perm);
		permuted.Untangle();

		// Get permuted outputs
		std::vector<uint64_t> permutedOutputs = FactoredOutputSet{ permuted, n }.ToVector();
		std::ranges::sort(permutedOutputs);

		// Compute expected permuted outputs
		Permutation outputPerm = network.GetOutputPermutation(perm);
		std::vector<uint64_t> expectedPermuted{ outputs };
		for (uint64_t& x : expectedPermuted)
			x = outputPerm(x);
		std::ranges::sort(expectedPermuted);

		bool areSame = permutedOutputs == expectedPermuted;
		std::println("{}", areSame);
		if (!areSame)
		{
			std::println("Network:  {}", network);
			std::println("perm:     {}", perm);
			std::println("Permuted: {}", permuted);
			break;
		}
	}
}