#include <print>
#include <chrono>
#include <random>
#include <fstream>
#include <format>
#include <numeric>
#include <bit>
#include <algorithm>

#include <sortnetutils.h>

Network RandomNetwork(uint8_t n, size_t size)
{
	std::vector<CE> alphabet;
	for (uint8_t i = 0; i + 1 < n; i++)
		for (uint8_t j = i + 1; j < n; j++)
			alphabet.push_back({ i, j });

	static std::mt19937_64 gen{ std::random_device{}() };
	std::uniform_int_distribution<size_t> dist{ 0, alphabet.size() - 1 };

	Network network;
	for (size_t i = 0; i < size; i++)
		network.push_back(alphabet[dist(gen)]);

	return network;
}

void OutputsToCSV(const std::string& filepath, const OutputSet& outputs, uint8_t n)
{
	std::ofstream file{ filepath, std::ios::binary };

	// Create bit vertices
	for (uint8_t bi = 0; bi < n; bi++)
		file << std::format("{},,bit\n", bi);

	size_t vertexIdx = n;
	for (uint64_t output : outputs)
	{
		// Specify vertex color
		file << std::format("{},,output\n", vertexIdx);

		// Add edges
		for (uint8_t bi = 0; bi < n; bi++)
			if (output & (1ULL << bi))
				file << std::format("{},{}\n", vertexIdx, bi);

		vertexIdx++;
	}
}

enum class BitPermResult
{
    Found,
    NotFound,
    TimedOut
};

class BitPermSubset
{
public:
    BitPermSubset(std::vector<uint64_t> a, std::vector<uint64_t> b, int nbits)
        : A(std::move(a)), B(std::move(b)), n(nbits)
    {
        std::sort(A.begin(), A.end());
        A.erase(std::unique(A.begin(), A.end()), A.end());
        std::sort(B.begin(), B.end());
        B.erase(std::unique(B.begin(), B.end()), B.end());
    }

    // Runs the search, giving up once `timeout` has elapsed.
    BitPermResult solve(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000))
    {
        deadline = std::chrono::steady_clock::now() + timeout;
        timedOut = false;
        checkCounter = 0;
        perm.assign(n, -1);

        if (A.size() > B.size()) return BitPermResult::NotFound;
        if (A.empty())
        {
            std::iota(perm.begin(), perm.end(), 0);
            return BitPermResult::Found;
        }

        std::vector<int> pcA(n, 0), pcB(n, 0);
        for (uint64_t x : A)
            for (int j = 0; j < n; j++)
                pcA[j] += (x >> j) & 1;

        for (uint64_t y : B)
            for (int k = 0; k < n; k++)
                pcB[k] += (y >> k) & 1;

        domains.assign(n, {});
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                if (pcB[k] >= pcA[j]) domains[j].push_back(k);

        used.assign(n, false);

        std::vector<std::vector<int>> initCand(A.size());
        for (size_t i = 0; i < A.size(); i++)
        {
            initCand[i].resize(B.size());
            for (size_t t = 0; t < B.size(); t++) initCand[i][t] = static_cast<int>(t);
        }

        bool found = backtrack(0, initCand);
        if (timedOut) return BitPermResult::TimedOut;
        return found ? BitPermResult::Found : BitPermResult::NotFound;
    }

    // Valid only after solve() has returned BitPermResult::Found.
    const std::vector<int>& getPerm() const
    {
        return perm;
    }

private:
    std::vector<uint64_t> A, B;
    int n;
    std::vector<int> perm;
    std::vector<bool> used;
    std::vector<std::vector<int>> domains;
    std::chrono::steady_clock::time_point deadline;
    bool timedOut = false;
    unsigned long checkCounter = 0;

    int pickNextVar()
    {
        int best = -1, bestSize = std::numeric_limits<int>::max();
        for (int j = 0; j < n; j++)
        {
            if (perm[j] != -1) continue;
            int sz = 0;
            for (int k : domains[j]) if (!used[k]) sz++;
            if (sz < bestSize) { bestSize = sz; best = j; }
        }
        return best;
    }

    bool backtrack(int assignedCount, std::vector<std::vector<int>>& cand)
    {
        // Check the clock only every 256 calls to keep the overhead of
        // std::chrono::steady_clock::now() from dominating the search.
        if (
            std::chrono::steady_clock::now() >= deadline)
        {
            timedOut = true;
        }
        if (timedOut) return false;

        if (assignedCount == n) return true;

        int j = pickNextVar();

        for (int k : domains[j])
        {
            if (used[k]) continue;

            std::vector<std::vector<int>> newCand = cand;
            bool ok = true;
            for (size_t i = 0; i < A.size() && ok; i++)
            {
                int bitJ = (A[i] >> j) & 1;
                std::vector<int> filtered;
                filtered.reserve(newCand[i].size());
                for (int idx : newCand[i])
                    if (((B[idx] >> k) & 1) == bitJ) filtered.push_back(idx);
                if (filtered.empty()) { ok = false; break; }
                newCand[i] = std::move(filtered);
            }
            if (!ok) continue;

            perm[j] = k; used[k] = true;
            if (backtrack(assignedCount + 1, newCand)) return true;
            perm[j] = -1; used[k] = false;

            if (timedOut) return false;
        }
        return false;
    }
};

bool SubsumesV1(const OutputSet& a, const OutputSet& b, uint8_t n)
{
    std::vector<uint8_t> perm(n);
    std::iota(perm.begin(), perm.end(), 0);

    do
    {
        bool isSubset = true;
        for (uint64_t ax : a)
        {
            // Permute the output
            uint64_t permutedOutput = 0;
            for (uint8_t i = 0; i < n; i++)
            {
                permutedOutput <<= 1;
                permutedOutput |= (ax >> perm[n - 1 - i]) & 1;
            }

            // Check if the permuted output is contained in b
            if (!b.Contains(permutedOutput)) { isSubset = false; break; }
        }

        if (isSubset) return true;
    } while (std::next_permutation(perm.begin(), perm.end()));

    return false;
}

BitPermResult SubsumesV2(const OutputSet& a, const OutputSet& b, uint8_t n)
{
    std::vector<uint64_t> aVec{ a.begin(), a.end() };
    std::vector<uint64_t> bVec{ b.begin(), b.end() };

    BitPermSubset solver{ aVec, bVec, n };
    BitPermResult result = solver.solve(std::chrono::milliseconds{ 2000 });
    return result;
}

int main()
{
#if 0
	uint8_t n = 16;

	Network net1 = ParseNetwork("[(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(0,15),(1,14),(3,7),(4,5),(8,12),(10,11)]");
	Network net2 = ParseNetwork("[(0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15),(1,3),(4,11),(5,10),(12,14)]");

	OutputSet outputs1 = GetOutputs(net1, n);
	OutputSet outputs2 = GetOutputs(net2, n);

	OutputsToCSV("C:\\Users\\matty\\Desktop\\graph1.csv", outputs1, n);
	OutputsToCSV("C:\\Users\\matty\\Desktop\\graph2.csv", outputs2, n);
#endif

    uint8_t n = 18;
    size_t size = 18;

    size_t solved = 0;
    size_t total = 0;

    for (;;)
    {
        Network a = RandomNetwork(n, size);
        Network b = RandomNetwork(n, size);

        OutputSet aOutputs = GetOutputs(a, n);
        OutputSet bOutputs = GetOutputs(b, n);
        if (aOutputs.Size() > bOutputs.Size()) continue;

        BitPermResult result = SubsumesV2(aOutputs, bOutputs, n);

        total++;
        if (result != BitPermResult::TimedOut) solved++;
        std::println("Solved {:.3f}%", (double)solved / total * 100.0);
    }
}