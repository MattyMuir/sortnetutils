#include "random.h"

#include <random>
#include <vector>
#include <algorithm>

CE RandomCE(uint8_t n, bool symmetric)
{
	thread_local std::mt19937_64 gen{ std::random_device{}() };

	if (!symmetric)
	{
		std::uniform_int_distribution<uint32_t> aDist{ 0, n - 1U };
		std::uniform_int_distribution<uint32_t> bDist{ 0, n - 2U };

		// Standard trick to generate a pair of distinct values 
		uint8_t a = (uint8_t)aDist(gen);
		uint8_t b = (uint8_t)bDist(gen);
		b = (b == a) ? n - 1U : b;

		return { std::min(a, b), std::max(a, b) };
	}

	// The space of symmetric comparators (x, y) looks like two triangles stacked on top of each other.
	// By horizontally flipping the top triangle and moving it down, you get a square of size (n/2)^2.
	// To generate a random CE, we can generate a point in the square, and undo the flipping and shifting
	// if the point landed in the lower triangle.
	std::uniform_int_distribution<uint32_t> aDist{ 0, n / 2 - 1U };
	std::uniform_int_distribution<uint32_t> bDist{ 0, n / 2 - 1U };

	uint8_t a = (uint8_t)aDist(gen);
	uint8_t b = (uint8_t)bDist(gen);

	if (b <= a)
	{
		a = n / 2 - 1U - a;
		b += n / 2;
	}

	return { a, b };
}

static inline Network SymmetricExpand(const Network& network, uint8_t n)
{
	// Calculate the new size
	size_t newSize = 0;
	for (auto [i, j] : network)
		newSize += (i == n - 1 - j) ? 1 : 2;

	Network expanded;
	expanded.reserve(newSize);
	for (auto [i, j] : network)
	{
		expanded.emplace_back(i, j);
		if (i != n - 1 - j)
			expanded.emplace_back(n - 1 - j, n - 1 - i);
	}
	return expanded;
}

// Ratios T(m)/T(m-1) of the "telephone numbers", which count the number
// of layers on m channels: T(m) = T(m-1) + (m-1)*T(m-2)
static std::vector<double> LayerRatios(uint8_t n)
{
	std::vector<double> r(n + 1, 0.0);
	if (n >= 1)
		r[1] = 1.0;
	for (uint32_t m = 2; m <= n; m++)
		r[m] = 1.0 + (m - 1) / r[m - 1];
	return r;
}

// A uniformly random layer: a uniformly random partial matching on {0, ..., n-1}.
// Process elements one at a time. If x is the current last remaining element and m
// elements remain, then among all matchings of those m elements, x is unmatched in
// exactly T(m-1) of them, and matched to one specific other element in T(m-2) of
// them. So we leave x free w.p. T(m-1)/T(m), and otherwise pair it with a uniformly
// random partner among the other m-1 elements. By symmetry, whatever's left after
// either choice is itself a uniformly random matching on the residual ground set,
// so this recursion is exact (verified by brute force for small n), not approximate.
static Network RandomLayer(uint8_t n)
{
	thread_local std::mt19937_64 gen{ std::random_device{}() };
	std::uniform_real_distribution<double> u{ 0.0, 1.0 };

	std::vector<double> r = LayerRatios(n);
	std::vector<uint8_t> avail(n);
	for (uint8_t i = 0; i < n; i++)
		avail[i] = i;

	Network layer;
	uint32_t m = n;
	while (m > 0)
	{
		double pFree = 1.0 / r[m];   // T(m-1) / T(m); equals exactly 1.0 when m == 1
		if (u(gen) < pFree)
		{
			m--;   // avail[m] (now dropped) stays unmatched
			continue;
		}

		// only reachable when m >= 2, since pFree == 1.0 exactly when m == 1
		std::uniform_int_distribution<uint32_t> pick{ 0, m - 2 };
		uint32_t idx = pick(gen);
		uint8_t x = avail[m - 1];
		uint8_t y = avail[idx];
		layer.emplace_back(std::min(x, y), std::max(x, y));

		avail[idx] = avail[m - 2];   // swap-remove both x and y
		m -= 2;
	}
	return layer;
}

// Ratios S(h)/S(h-1) for the analogous count of *symmetric* layers on n = 2h elements
// (mirror pairs only, no fixed centre): S(0) = 1, S(1) = 2,
//     S(h) = 2*S(h-1) + 2*(h-1)*S(h-2)
// The factor of 2 on each term comes from each mirror-pair having two ways to be
// "unmatched" (free, or self-paired with its own mirror) and, when linked to another
// pair, two possible orientations (parallel or crossed -- see RandomSymmetricLayer).
static std::vector<double> SymmetricLayerRatios(uint8_t h)
{
	std::vector<double> r(h + 1, 0.0);
	if (h >= 1)
		r[1] = 2.0;
	for (uint32_t k = 2; k <= h; k++)
		r[k] = 2.0 + 2.0 * (k - 1) / r[k - 1];
	return r;
}

// A uniformly random symmetric layer.
//
// Group {0, ..., n-1} into h = n/2 mirror pairs P_k = {k, n-1-k}. (If n is odd, the
// centre index has no mirror partner; for symmetry it can only ever be free, so it's
// simply excluded below -- it never appears in any comparator.)
//
// A symmetric layer is built by deciding, independently with the right weights, what
// happens to each pair P_k:
//   - both elements free
//   - self-paired: comparator (k, n-1-k)                    [a self-symmetric CE]
//   - linked to another pair P_l, via one of 2 orientations:
//       parallel:  (k, l)      and (n-1-k, n-1-l)
//       crossed:   (k, n-1-l)  and (n-1-k, l)
// exactly mirroring the construction in SymmetricExpand. This is the same recursive
// sampling idea as RandomLayer, just run on the h pairs instead of the n elements,
// with an extra fair coin flip whenever a pair is "unmatched" or "linked" to pick
// between its two equally-likely sub-options.
static Network RandomSymmetricLayer(uint8_t n)
{
	thread_local std::mt19937_64 gen{ std::random_device{}() };
	std::uniform_real_distribution<double> u{ 0.0, 1.0 };

	uint8_t h = n / 2;
	std::vector<double> r = SymmetricLayerRatios(h);
	std::vector<uint8_t> avail(h);
	for (uint8_t k = 0; k < h; k++)
		avail[k] = k;

	Network layer;
	uint32_t m = h;
	while (m > 0)
	{
		uint8_t k = avail[m - 1];
		uint8_t mirrorK = n - 1 - k;

		double pUnmatched = 2.0 / r[m];   // 2 * S(m-1) / S(m); equals exactly 1.0 when m == 1
		if (u(gen) < pUnmatched)
		{
			if (u(gen) < 0.5)
				layer.emplace_back(k, mirrorK);   // self-paired
			// else: both free, nothing to add
			m--;
			continue;
		}

		// only reachable when m >= 2, since pUnmatched == 1.0 exactly when m == 1
		std::uniform_int_distribution<uint32_t> pick{ 0, m - 2 };
		uint32_t idx = pick(gen);
		uint8_t l = avail[idx];
		uint8_t mirrorL = n - 1 - l;

		if (u(gen) < 0.5)
		{
			// parallel
			layer.emplace_back(std::min(k, l), std::max(k, l));
			layer.emplace_back(std::min(mirrorK, mirrorL), std::max(mirrorK, mirrorL));
		}
		else
		{
			// crossed
			layer.emplace_back(std::min(k, mirrorL), std::max(k, mirrorL));
			layer.emplace_back(std::min(mirrorK, l), std::max(mirrorK, l));
		}

		avail[idx] = avail[m - 2];   // swap-remove both k and l
		m -= 2;
	}
	return layer;
}

Network RandomNetworkSized(uint8_t n, size_t size, bool symmetric)
{
	// For symmetric networks, the proportion of self-symmetric CEs is 2/n
	// Therefore, the average network grows by a factor of (2 - 2/n) when symmetrically expanded
	if (symmetric)
		size /= 2.0 - 2.0 / n;

	// Construct a random network one comparator at a time
	Network network;
	network.reserve(size);
	for (size_t i = 0; i < size; i++)
		network.emplace_back(RandomCE(n, symmetric));

	// Expand if symmetric
	return symmetric ? SymmetricExpand(network, n) : network;
}

Network RandomNetworkLayered(uint8_t n, size_t depth, bool symmetric)
{
	Network network;
	for (size_t d = 0; d < depth; d++)
	{
		Network layer = symmetric ? RandomSymmetricLayer(n) : RandomLayer(n);
		network.insert(network.end(), layer.begin(), layer.end());
	}
	return network;
}