#include "tikz.h"

#include <algorithm>

#include "layers.h"

static inline bool Overlaps(CE a, CE b)
{
	bool noOverlap = (a.hi < b.lo) || (b.hi < a.lo);
	return !noOverlap;
}

static inline bool FitsInSublayer(const Network& sublayer, CE ce)
{
	return !std::ranges::any_of(sublayer, [ce](CE existingCE) { return Overlaps(existingCE, ce); });
}

static inline std::vector<Network> SplitIntoSublayers(const Network& layer)
{
	std::vector<Network> sublayers{ {} };
	for (CE ce : layer)
	{
		bool foundLayer = false;
		for (Network& sublayer : sublayers)
		{
			bool fits = FitsInSublayer(sublayer, ce);
			if (!fits) continue;
			sublayer.push_back(ce);
			foundLayer = true;
			break;
		}

		if (!foundLayer) sublayers.push_back({ ce });
	}
	return sublayers;
}

static inline void DrawLine(std::string& latex, float x0, float y0, float x1, float y1, std::string_view color = "black")
{
	latex += std::format("\\draw[{}] ({:.2f},{:.2f}) -- ({:.2f},{:.2f});\n", color, x0, y0, x1, y1);
}

static inline void DrawComparator(std::string& latex, float x0, float y0, float x1, float y1, float radius, std::string_view color = "black")
{
	latex += std::format("\\draw[{}] ({:.2f},{:.2f}) -- ({:.2f},{:.2f});\n", color, x0, y0, x1, y1);
	latex += std::format("\\fill[{}] ({:.2f},{:.2f}) circle ({:.2f});\n", color, x0, y0, radius);
	latex += std::format("\\fill[{}] ({:.2f},{:.2f}) circle ({:.2f});\n", color, x1, y1, radius);
}

std::string GenerateTikz(uint8_t n, const Network& network)
{
	// === Parameters ===
	float sublayerSeparation = 0.4f;
	float layerSeparation = 2.0f;
	float circleRadius = 0.15f;
	// ==================

	// Split into sublayers
	std::vector<Network> layers = GetLayers(network);
	std::vector<std::vector<Network>> allSublayers;

	float maxX = (layers.size() - 1) * layerSeparation;
	for (const auto& layer : layers)
	{
		std::vector<Network> sublayers = SplitIntoSublayers(layer);
		allSublayers.push_back(sublayers);
		maxX += (sublayers.size() - 1) * sublayerSeparation;
	}

	// === Drawing ===
	float scale = 1.0f / (maxX + layerSeparation);
	std::string latex = std::format("\\begin{{tikzpicture}}[x={0}\\linewidth, y=-{0}\\linewidth]\n", scale);

	// Draw channels
	for (uint8_t channel = 0; channel < n; channel++)
		DrawLine(latex, -layerSeparation / 2, channel, maxX + layerSeparation / 2, channel);

	// Draw network
	float xPos = 0;
	for (const auto& layer : allSublayers)
	{
		for (size_t sublayerIdx = 0; sublayerIdx < layer.size(); sublayerIdx++)
		{
			if (sublayerIdx) xPos += sublayerSeparation;
			for (CE ce : layer[sublayerIdx])
				DrawComparator(latex, xPos, ce.lo, xPos, ce.hi, circleRadius);
		}
		xPos += layerSeparation;
	}

	latex += "\\end{tikzpicture}";
	return latex;
}