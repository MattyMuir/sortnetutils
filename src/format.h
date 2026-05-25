#pragma once
#include "Network.h"

template<>
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

    std::format_context::iterator format(const Network& network, std::format_context& ctx) const;

protected:
	FormatType type = FormatType::Default;

	static std::format_context::iterator FormatDefault(const Network& network, std::format_context& ctx);
	static std::format_context::iterator FormatLayers(const Network& network, std::format_context& ctx);
	static std::format_context::iterator FormatTikz(const Network& network, std::format_context& ctx);
};