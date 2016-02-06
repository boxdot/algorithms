#include <catch.hpp>
#include <vector>
#include <random>


// https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// Works also for multisets
template<typename T>
bool next_permutation(std::vector<T>& seq) {
    // trivial cases
    if (seq.size() < 2) {
        return false;
    }

    // find pivot
    int i = seq.size() - 1;
    while (i > 0 && seq[i - 1] >= seq[i]) {
        i--;
    }

    // last permutation
    if (i == 0) {
        return false;
    }

    // find right most element greater than pivot
    int j = seq.size() - 1;
    while (seq[j] <= seq[i - 1]) {
        j--;
    }
    std::swap(seq[i - 1], seq[j]);

    // sort suffix <=> reverse suffix
    j = seq.size() - 1;
    while (i < j) {
        std::swap(seq[i], seq[j]);
        i++;
        j--;
    }

    return true;
}


TEST_CASE("Permutate empty vector", "[next_permutation]") {
    std::vector<int> seq = {};
    REQUIRE(next_permutation(seq) == false);
    REQUIRE(seq == (std::vector<int>{}));
}

TEST_CASE("Permutate a singelton", "[next_permutation]") {
    std::vector<int> seq = {0};
    REQUIRE(next_permutation(seq) == false);
    REQUIRE(seq == (std::vector<int>{0}));
}

TEST_CASE("Permutate two elements", "[next_permutation]") {
    std::vector<int> seq = {0, 1};
    REQUIRE(next_permutation(seq) == true);
    REQUIRE(seq == (std::vector<int>{1, 0}));
    REQUIRE(next_permutation(seq) == false);
}

TEST_CASE("Permutate three elements", "[next_permutation]") {
    std::vector<int> seq = {0, 1, 2};
    REQUIRE(next_permutation(seq));
    REQUIRE(seq == (std::vector<int>{0, 2, 1}));
    REQUIRE(next_permutation(seq));
    REQUIRE(seq == (std::vector<int>{1, 0, 2}));
    REQUIRE(next_permutation(seq));
    REQUIRE(seq == (std::vector<int>{1, 2, 0}));
    REQUIRE(next_permutation(seq));
    REQUIRE(seq == (std::vector<int>{2, 0, 1}));
    REQUIRE(next_permutation(seq));
    REQUIRE(seq == (std::vector<int>{2, 1, 0}));
    REQUIRE(!next_permutation(seq));
    REQUIRE(seq == (std::vector<int>{2, 1, 0}));
}

TEST_CASE("Permutate 10 different elements", "[next_permutation]") {
    std::vector<int> initial = {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<int> seq = initial;

    int n = 1;
    while (next_permutation(seq)) {
        n += 1;
        REQUIRE(seq != initial);
    }
    REQUIRE(n == 40320);
}

TEST_CASE("Permutate 10 elements where two are equal", "[next_permutation]") {
    std::vector<int> initial = {1, 1, 2, 4, 5, 6, 7};
    std::vector<int> seq = initial;

    int n = 1;
    while (next_permutation(seq)) {
        n += 1;
        REQUIRE(seq != initial);
    }
    REQUIRE(n == 2520);
}

TEST_CASE("Permutate n (n < 7) elements where m are equal",
    "[next_permutation]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 7);
    int n = dis(gen);
    int m = dis(gen);
    if (n < m) {
        std::swap(n, m);
    }

    std::vector<int> initial(n, 0);
    for (int i = m; i < n; ++i) {
        initial[i] = i;
    }

    auto mfactorial = [](size_t m, size_t n) {
        int res = 1;
        while (n > m) {
            res *= n;
            n--;
        }
        return res;
    };

    int k = 1;
    auto seq = initial;
    while (next_permutation(seq)) {
        k += 1;
        REQUIRE(seq != initial);
    }
    REQUIRE(k == mfactorial(m, n));
}
