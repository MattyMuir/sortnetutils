#include "Network.h"

Network Concatenate(const Network& a, const Network& b)
{
	Network ret{ a };
	ret.insert(ret.end(), b.begin(), b.end());
	return ret;
}

std::format_context::iterator std::formatter<Network>::format(const Network& network, std::format_context& ctx) const
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