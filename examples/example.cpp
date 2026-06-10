#include <print>
#include <chrono>

#include <sortnetutils.h>

int main()
{
	Network network = ParseNetwork(R"(
		[(0,6),(1,10),(2,15),(3,5),(4,9),(7,16),(8,13),(11,17),(12,14)]
		[(0,12),(1,4),(3,11),(5,17),(6,14),(7,8),(9,10),(13,16)]
		[(1,13),(2,7),(4,16),(6,9),(8,11),(10,15)]
		)");

	auto start = std::chrono::steady_clock::now();

	for (size_t i = 0; i < 5000; i++)
	{
#if 1
		NetworkVerifierV2 verifier{ network, 18 };
		bool isValid = verifier.IsValid();
#else
		auto outputs = GetOutputs(network, 18);
#endif
	}

	auto end = std::chrono::steady_clock::now();
	auto duration = end - start;
	uint64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	std::println("Took: {}ms", micros * 1.0e-3);
}