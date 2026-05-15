#pragma once
#include <vector>
#include <format>

#include "CE.h"

using Network = std::vector<CE>;

Network Concatenate(const Network& a, const Network& b);

template<>
struct std::formatter<Network>
{
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    std::format_context::iterator format(const Network& network, std::format_context& ctx) const;
};