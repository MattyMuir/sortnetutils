#pragma once
#include <format>

#include "../Network/Network.h"

template <>
struct std::formatter<Network>
{
protected:
    enum class FormatType
    {
        Default,
        Layers,
        Tikz
    };

public:
    constexpr auto parse(std::format_parse_context& ctx)
    {
        // Scan until closing curly
        auto begin = ctx.begin();
        auto end = begin;
        while (*end != '}') end++;
        size_t size = std::distance(begin, end);

        if (size == 0) return end;
        if (size > 1) throw std::format_error{ "Invalid format specifier" };

        switch (*begin)
        {
        break; case 'd': type = FormatType::Default;
        break; case 'l': type = FormatType::Layers;
        break; case 't': type = FormatType::Tikz;
        break; default: throw std::format_error{ "Invalid format specifier" };
        }

        return end;
    }

    std::format_context::iterator format(const Network& network, std::format_context& ctx) const;

protected:
    FormatType type = FormatType::Default;
};