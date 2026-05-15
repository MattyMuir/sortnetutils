#include <iostream>
#include <format>

#include <sortnetutils.h>

int main()
{
	int x = 17;
	std::cout << std::format("{}^2 = {}\n", x, Square(x));
}