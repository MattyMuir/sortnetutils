#include <print>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <random>

#include <sortnetutils.h>

using Clause = std::vector<int64_t>;

bool NextCombination(std::vector<size_t>& items, size_t n)
{
	size_t r = items.size();

    // Scan for an index which can be incremented
    for (size_t numAfter = 0; numAfter < r; numAfter++)
    {
        size_t itemIdx = r - 1 - numAfter;
        if (items[itemIdx] < n - numAfter - 1)
        {
            // Increment this item
            items[itemIdx]++;

            // Fill remaining slots
            for (size_t i = itemIdx + 1; i < r; i++)
                items[i] = items[i - 1] + 1;

            return true;
        }
    }

    return false;
}

std::vector<Clause> GetAllClauses(size_t n, size_t clauseSize)
{
    std::vector<size_t> vars(clauseSize);
    std::iota(vars.begin(), vars.end(), 0);

    std::vector<Clause> clauses;
    do
    {
        // Copy vars to clause
        Clause varsClause;
        for (size_t i = 0; i < clauseSize; i++) varsClause.push_back((int64_t)vars[i] + 1);

        // Iterate over all ways to negate variables in the clause
        for (uint64_t signMask = 0; signMask < (1ULL << clauseSize); signMask++)
        {
            Clause clause{ varsClause };
            for (size_t i = 0; i < clauseSize; i++)
                if ((signMask >> i) & 1ULL)
                    clause[i] = -clause[i];
            clauses.push_back(clause);
        }
    } while (NextCombination(vars, n));

    return clauses;
}

bool IsClauseValid(uint64_t x, const Clause& clause)
{
    for (int64_t literal : clause)
    {
        int64_t var = std::abs(literal) - 1;
        bool isPos = literal > 0;
        bool val = (x >> var) & 1ULL;
        if (val == isPos) return true;
    }

    return false;
}

bool IsClauseValid(const std::vector<uint64_t>& outputs, const Clause& clause)
{
    for (uint64_t x : outputs)
        if (!IsClauseValid(x, clause))
            return false;
    return true;
}

void PrintClause(const Clause& clause)
{
    std::stringstream ss;
    ss << '(';
    for (size_t i = 0; i < clause.size(); i++)
    {
        if (i) ss << " v ";
        int64_t literal = clause[i];
        int64_t var = std::abs(literal) - 1;
        bool isPos = literal > 0;
        ss << std::format("{}x{}", isPos ? "" : "!", var);
    }
    ss << ')';
    std::println("{}", ss.str());
}

void PrintImplies(const Clause& clause)
{
    std::vector<int64_t> lhs, rhs;
    for (int64_t literal : clause)
        (literal < 0 ? lhs : rhs).push_back(std::abs(literal));

    std::stringstream ssLhs, ssRhs;
    for (size_t i = 0; i < lhs.size(); i++)
    {
        if (i) ssLhs << " ∧ ";
        ssLhs << std::format("x{}", lhs[i] - 1);
    }

    for (size_t i = 0; i < rhs.size(); i++)
    {
        if (i) ssRhs << " ∨ ";
        ssRhs << std::format("x{}", rhs[i] - 1);
    }

    std::println("{:<30} -> {:>30}", ssLhs.str(), ssRhs.str());
}

std::vector<Clause> GetValidClauses(const std::vector<uint64_t>& outputs, size_t n, size_t clauseSize)
{
    auto allClauses = GetAllClauses(n, clauseSize);
    std::erase_if(allClauses, [&](const Clause& clause) { return !IsClauseValid(outputs, clause); });
    return allClauses;
}

bool IsSubset(const Clause& a, const Clause& b)
{
    for (int64_t literal : a)
        if (!std::ranges::contains(b, literal))
            return false;
    return true;
}

bool IsUndermined(const Clause& a, const std::vector<Clause>& subClauses)
{
    for (const Clause& subClause : subClauses)
        if (IsSubset(subClause, a))
            return true;
    return false;
}

std::vector<uint64_t> RandomSet(uint8_t n, size_t size)
{
    static std::mt19937_64 gen{ std::random_device{}() };
    std::uniform_int_distribution<uint64_t> dist{ 0, (1ULL << n) - 1 };
    std::unordered_set<uint64_t> set;
    while (set.size() < size)
        set.insert(dist(gen));
    return { set.begin(), set.end() };
}

int main()
{
    uint8_t n = 10;
    Network prefix = ParseNetwork(R"(
        [(0,8),(1,9),(2,7),(3,5),(4,6)]
        [(0,2),(1,4),(5,8),(7,9)]
        [(0,3),(2,4),(5,7),(6,9)]
        )");

    std::vector<uint64_t> outputs = FactoredOutputSet{ prefix, n }.ToVector();
    std::println("Num outputs: {}", outputs.size());

    outputs = RandomSet(n, 76);

    // Get all valid clauses
    size_t maxClauseSize = 10;
    std::vector<std::vector<Clause>> validClauses;
    for (size_t clauseSize = 1; clauseSize <= maxClauseSize; clauseSize++)
        validClauses.push_back(GetValidClauses(outputs, n, clauseSize));

    // Remove undermined clauses
    for (size_t clauseSize = 2; clauseSize <= maxClauseSize; clauseSize++)
        for (size_t subSize = 1; subSize < clauseSize; subSize++)
            std::erase_if(validClauses[clauseSize - 1], [&](const Clause& clause) { return IsUndermined(clause, validClauses[subSize - 1]); });

    for (const auto& clauses : validClauses)
        for (const Clause& clause : clauses)
            PrintImplies(clause);
}