#include <print>
#include <random>
#include <numeric>
#include <set>
#include <unordered_set>
#include <chrono>
#include <numeric>
#include <algorithm>

#include <sortnetutils.h>

std::vector<uint8_t> RandomVec(uint8_t n)
{
	static std::mt19937_64 gen{ std::random_device{}() };

	std::vector<uint8_t> perm(n);
	std::iota(perm.begin(), perm.end(), 0);
	std::shuffle(perm.begin(), perm.end(), gen);
	return perm;
}

void RunNetwork(const Network& network, std::vector<uint8_t>& arr)
{
	for (auto [i, j] : network)
		if (arr[i] > arr[j])
			std::swap(arr[i], arr[j]);
}

std::set<std::vector<uint8_t>> GetManyOutputs(const Network& prefix, uint8_t n, size_t num)
{
	std::set<std::vector<uint8_t>> outputs;
	while (outputs.size() < num)
	{
		std::vector<uint8_t> vec = RandomVec(n);
		RunNetwork(prefix, vec);
		outputs.insert(vec);
	}
	return outputs;
}

int main()
{
	// === Parameters ===
	uint8_t n = 18;
	Network prefix = ParseNetwork("[(0,13),(4,17),(1,2),(15,16),(3,14),(5,6),(11,12),(7,10),(8,9),(0,11),(6,17),(1,16),(2,9),(8,15),(3,10),(7,14),(4,5),(12,13),(0,4),(13,17),(1,8),(9,16),(2,15),(3,7),(10,14),(5,12),(6,11)]");
	// ==================

	auto outputs = GetManyOutputs(prefix, n, 1000);

	auto start = std::chrono::steady_clock::now();
	for (const auto& vec : outputs)
	{
		bool isOutput = (bool)prefix.GetInput(vec);
		if (!isOutput) std::println("No!");
	}
	auto end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::println("Took: {:.6f}s", duration.count() * 1e-6);
}