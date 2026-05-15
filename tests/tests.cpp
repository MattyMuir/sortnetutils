#include <iostream>
#include <charconv>
#include <format>

#include <sortnetutils.h>

#define ERROR(msg) { std::cerr << msg << '\n'; return 1; }

int RunTest(int x, int expected)
{
	if (Square(x) != expected)
		ERROR(std::format("Error x: {}", x));

	return 0;
}

int main(int argc, char** argv)
{
	// Check number of arguments is correct
	if (argc != 2)
		ERROR("Test must have exactly one extra argument");

	// Get second argument
	std::string arg{ argv[1] };

	// Convert to integer
	size_t testIdx;
	auto convRes = std::from_chars(arg.data(), arg.data() + arg.size(), testIdx);
	if (convRes.ec != std::errc())
		ERROR("Test index could not be parsed");

	switch (testIdx)
	{
	case 0: return RunTest(7, 49);
	case 1: return RunTest(12, 144);
	default: ERROR("Invalid test index");
	}
}