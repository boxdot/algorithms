#include <catch.hpp>
#include <vector>
#include <random>
#include <assert.h>

// stupid implementation
template<typename Iter>
int sgn(Iter begin, Iter end) {
    int inversion = 0;
    for (auto i = begin; i != end; ++i) {
        for (auto j = i + 1; j != end; ++j) {
            if (*i > *j) {
                inversion += 1;
                inversion %= 2;
            }
        }
    }
    return inversion == 0 ? 1 : -1;
}


// naive version
// assumption: seq == {0, ..., n - 1} as set
bool johnson_trotter(std::vector<int>& seq)
{
    assert(seq.size() <= std::numeric_limits<int>::max());
    const int n = seq.size();
    std::vector<int> invseq(n, 0);

    for (int i = 0; i < n; ++i) {
        invseq[seq[i]] = i;
    }

    // step 1
    std::vector<int> y(n, 0);
    for (int i = 0; i < n; ++i) {
        int sign = sgn(invseq.begin(), invseq.begin() + i);
        y[i] = invseq[i] - sign;
    }

    // step 2
    int i = n - 1;
    while (i >= 0) {
        if ((0 <= y[i] && y[i] < n) && seq[y[i]] < i) {
            break;
        }
        i--;
    }

    // last permutation
    if (i < 0) {
        return false;
    }

    std::swap(seq[invseq[i]], seq[y[i]]);
    return true;
}

// sometimes called plain-changes
bool johnson_trotter_improved(std::vector<int>& seq) {
    const size_t N = seq.size();

    std::vector<int> c(N, 0);
    std::vector<int> o(N, 0);

    // count inversions
    // could be done progressively with state
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            if (seq[j] < seq[i]) {
                c[seq[i]] += 1;
            }
        }
    }

    // find directions
    for (size_t i = 1, k = 0; i < N - 1; ++i) {
        k += c[i];
        o[i + 1] = k % 2;
    }

    // find and move mobile
    for (int j = N - 1, s = 0; j > 0; --j) {
        if (o[j] == 0) {
            if (c[j] < j) {
                std::swap(seq[j - c[j] + s], seq[j - c[j] + s - 1]);
                return true;
            }
            s++;
        } else {
            if (c[j] > 0) {
                std::swap(seq[j - c[j] + s], seq[j - c[j] + s + 1]);
                return true;
            }
        }
    }
    return false;
}

//
// Tests
//

TEST_CASE("Sign of empty permutation is 1", "[sign]") {
    std::vector<int> seq;
    REQUIRE(sgn(seq.begin(), seq.end()) == 1);
}

TEST_CASE("Sign of singleton is 1", "[sign]") {
    std::vector<int> seq{0};
    REQUIRE(sgn(seq.begin(), seq.end()) == 1);
}

TEST_CASE("Sign of 2-permutations is correct", "[sign]") {
    auto seq = std::vector<int>{0, 1};
    REQUIRE(sgn(seq.begin(), seq.end()) == 1);
    seq = std::vector<int>{1, 0};
    REQUIRE(sgn(seq.begin(), seq.end()) == -1);
}

TEST_CASE("Sign of 3-permutations is correct", "[sign]") {
    auto seq = std::vector<int>{0, 1, 2};
    REQUIRE(sgn(seq.begin(), seq.end()) == 1);
    seq = std::vector<int>{0, 2, 1} ;
    REQUIRE(sgn(seq.begin(), seq.end()) == -1);
    seq = std::vector<int>{2, 0, 1};
    REQUIRE(sgn(seq.begin(), seq.end()) == 1);
    seq = std::vector<int>{2, 1, 0};
    REQUIRE(sgn(seq.begin(), seq.end()) == -1);
    seq = std::vector<int>{1, 2, 0};
    REQUIRE(sgn(seq.begin(), seq.end()) == 1);
    seq = std::vector<int>{1, 0, 2};
    REQUIRE(sgn(seq.begin(), seq.end()) == -1);
}


TEST_CASE("Permutate empty vector", "[johnson_trotter]") {
    std::vector<int> seq = {};
    REQUIRE(johnson_trotter(seq) == false);
    REQUIRE(seq == (std::vector<int>{}));
}

TEST_CASE("Permutate a singleton", "[johnson_trotter]") {
    std::vector<int> seq = {0};
    REQUIRE(johnson_trotter(seq) == false);
    REQUIRE(seq == (std::vector<int>{0}));
}

TEST_CASE("Permutate two elements", "[johnson_trotter]") {
    std::vector<int> seq = {0, 1};
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{1, 0}));
    REQUIRE(johnson_trotter(seq) == false);
}

TEST_CASE("Permutate three elements", "[johnson_trotter]") {
    std::vector<int> seq = {0, 1, 2};
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{0, 2, 1}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{2, 0, 1}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{2, 1, 0}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{1, 2, 0}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{1, 0, 2}));
    REQUIRE(johnson_trotter(seq) == false);
    REQUIRE(seq == (std::vector<int>{1, 0, 2}));
}

TEST_CASE("Permutate n (n < 8) elements", "[johnson_trotter]") {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 8);
    int n = dis(gen);

    std::vector<int> initial(n, 0);
    for (int i = 0; i < n; ++i) {
        initial[i] = i;
    }

    auto factorial = [](size_t n) {
        int res = 1;
        while (n > 0) {
            res *= n;
            n--;
        }
        return res;
    };

    int k = 1;
    auto seq = initial;
    while (johnson_trotter(seq)) {
        k += 1;
        REQUIRE(seq != initial);
    }
    REQUIRE(k == factorial(n));
}


TEST_CASE("Permutate empty vector (improved)",
    "[johnson_trotter_improved]")
{
    std::vector<int> seq = {};
    REQUIRE(johnson_trotter(seq) == false);
    REQUIRE(seq == (std::vector<int>{}));
}

TEST_CASE("Permutate a singleton (improved)",
    "[johnson_trotter_improved]")
{
    std::vector<int> seq = {0};
    REQUIRE(johnson_trotter(seq) == false);
    REQUIRE(seq == (std::vector<int>{0}));
}

TEST_CASE("Permutate two elements (improved)",
    "[johnson_trotter_improved]")
{
    std::vector<int> seq = {0, 1};
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{1, 0}));
    REQUIRE(johnson_trotter(seq) == false);
}

TEST_CASE("Permutate three elements (improved)",
    "[johnson_trotter_improved]")
{
    std::vector<int> seq = {0, 1, 2};
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{0, 2, 1}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{2, 0, 1}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{2, 1, 0}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{1, 2, 0}));
    REQUIRE(johnson_trotter(seq));
    REQUIRE(seq == (std::vector<int>{1, 0, 2}));
    REQUIRE(johnson_trotter(seq) == false);
    REQUIRE(seq == (std::vector<int>{1, 0, 2}));
}

TEST_CASE("Permutate n (n < 8) elements (improved)",
    "[johnson_trotter_improved]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 8);
    int n = dis(gen);

    std::vector<int> initial(n, 0);
    for (int i = 0; i < n; ++i) {
        initial[i] = i;
    }

    auto factorial = [](size_t n) {
        int res = 1;
        while (n > 0) {
            res *= n;
            n--;
        }
        return res;
    };

    int k = 1;
    auto seq = initial;
    while (johnson_trotter(seq)) {
        k += 1;
        REQUIRE(seq != initial);
    }
    REQUIRE(k == factorial(n));
}
