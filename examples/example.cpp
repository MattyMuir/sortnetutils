#include <print>

#include <sortnetutils.h>

void PrintGeneralInfo(const Network& network, uint8_t n)
{
	std::println("{}", network);
	std::println("Symmetric: {}", IsSymmetric(network, n));
	std::println("Valid:     {}", IsValid(network, n));
}

int main()
{
	PrintGeneralInfo(Network{{
			{0,2},{1,3},{4,6},{5,7},
			{0,4},{1,5},{2,6},{3,7},
			{0,1},{2,3},{4,5},{6,7},
			{2,4},{3,5},
			{1,4},{3,6},
			{1,2},{3,4},{5,6}
		}}, 8);

	PrintGeneralInfo(Network{{
			{0,3},{1,7},{2,5},{4,8},
			{0,7},{2,4},{3,8},{5,6},
			{0,2},{1,3},{4,5},{7,8},
			{1,4},{3,6},{5,7},
			{0,1},{2,4},{3,5},{6,8},
			{2,3},{4,5},{6,7},
			{1,2},{3,4},{5,6}
		}}, 9);

	PrintGeneralInfo(Network{{
			{0,3},{1,7},{2,5},{4,8},
			{0,7},{2,4},{3,8},{5,6},
			{0,2},{1,3},{4,5},{7,8},
			{1,4},{3,6},{5,7},
			{0,1},{2,4},{3,5},{6,8},
			{2,3},{4,5},{6,7},
			{1,2},{5,6}
		}}, 9);
}