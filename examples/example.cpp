#include <sortnetutils.h>

int main()
{
	uint8_t n = 18;
	size_t depth = 3;
	bool symmetric = true;

	for (;;)
	{
		Network network = RandomNetworkLayered(n, depth, symmetric);
		OutputSet outputs = GetOutputs(network, n);

		for (uint64_t x : outputs)
			if (!outputs.Contains(Mirror(n, x)))
				bool jkdfhjk = true;
	}
}