#include "format.h"

#include "tikz.h"
#include "layers.h"

std::format_context::iterator FormatDefault(const Network& network, std::format_context& ctx)
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

std::format_context::iterator FormatLayers(const LayeredNetwork& network, std::format_context& ctx)
{
    auto out = ctx.out();

    for (size_t i = 0; i < network.size(); i++)
    {
        if (i) *out++ = '\n';
        out = std::format_to(out, "{}", network[i]);
    }

    return out;
}

std::format_context::iterator FormatTikz(const LayeredNetwork& network, std::format_context& ctx)
{
    auto out = ctx.out();

    uint8_t n = InferN(network);
    std::string tikz = GenerateTikz(network, n);
    out = std::format_to(out, "{}", tikz);

    return out;
}


std::format_context::iterator FormatDefault(const LayeredNetwork& network, std::format_context& ctx)
{
    return FormatDefault(Concatenate(network), ctx);
}

std::format_context::iterator FormatLayers(const Network& network, std::format_context& ctx)
{
    return FormatLayers(GetLayers(network), ctx);
}

std::format_context::iterator FormatTikz(const Network& network, std::format_context& ctx)
{
    return FormatTikz(GetLayers(network), ctx);
}