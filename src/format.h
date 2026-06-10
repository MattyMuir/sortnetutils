#pragma once
#include <format>

#include "Network.h"

std::format_context::iterator FormatDefault(const Network& network, std::format_context& ctx);
std::format_context::iterator FormatLayers(const LayeredNetwork& network, std::format_context& ctx);
std::format_context::iterator FormatTikz(const LayeredNetwork& network, std::format_context& ctx);

std::format_context::iterator FormatDefault(const LayeredNetwork& network, std::format_context& ctx);
std::format_context::iterator FormatLayers(const Network& network, std::format_context& ctx);
std::format_context::iterator FormatTikz(const Network& network, std::format_context& ctx);

template <NetworkTy T>
struct std::formatter<T>
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
        auto it = ctx.begin();
        auto end = ctx.end();

        // Determine format string size
        size_t size = 0;
        for (auto it2 = it; it2 != end && *it2 != '}'; it2++)
            size++;

        if (size == 0) return it;
        if (size > 1)  throw std::format_error{ "Invalid format specifier" };

        switch (*it)
        {
        break; case 'd': type = FormatType::Default;
        break; case 'l': type = FormatType::Layers;
        break; case 't': type = FormatType::Tikz;
        break; default:  throw std::format_error{ "Invalid format specifier" };
        }

        return it + 1;
    }

    std::format_context::iterator format(const T& network, std::format_context& ctx) const
    {
        switch (type)
        {
        case FormatType::Default:   return FormatDefault(network, ctx);
        case FormatType::Layers:    return FormatLayers(network, ctx);
        case FormatType::Tikz:      return FormatTikz(network, ctx);
        }
    }

protected:
    FormatType type = FormatType::Default;
};