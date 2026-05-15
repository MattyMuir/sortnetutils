#include "format.h"

#include "layers.h"
#include "tikz.h"

std::format_context::iterator std::formatter<Network>::format(const Network& network, std::format_context& ctx) const
{
    switch (type)
    {
    case FormatType::Default:   return FormatDefault(network, ctx);
    case FormatType::Layers:    return FormatLayers(network, ctx);
    case FormatType::Tikz:      return FormatTikz(network, ctx);
    }
}

std::format_context::iterator std::formatter<Network>::FormatDefault(const Network& network, std::format_context& ctx) const
{
    auto out = ctx.out();

    *out++ = '[';
    for (size_t i = 0; i < network.size(); i++)
    {
        if (i) *out++ = ',';
        out = std::format_to(out, "({},{})", network[i].lo, network[i].hi);
    }
    *out++ = ']';

    return out;
}

std::format_context::iterator std::formatter<Network>::FormatLayers(const Network& network, std::format_context& ctx) const
{
    auto out = ctx.out();

    std::vector<Network> layers = GetLayers(network);
    for (size_t i = 0; i < layers.size(); i++)
    {
        if (i) *out++ = '\n';
        out = std::format_to(out, "{}", layers[i]);
    }

    return out;
}

std::format_context::iterator std::formatter<Network>::FormatTikz(const Network& network, std::format_context& ctx) const
{
    auto out = ctx.out();

    uint8_t n = InferN(network);
    std::string tikz = GenerateTikz(network, n);
    out = std::format_to(out, "{}", tikz);

    return out;
}