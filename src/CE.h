#pragma once
#include <cstdint>
#include <compare>

struct CE
{
	uint8_t lo, hi;

	auto operator<=>(const CE& other) const = default;
};