#include <catch.hpp>
#include <algorithm>
#include <tuple>
#include <assert.h>


template<typename T>
using triple = std::tuple<T, T, T>;

// 3sum - find 3 (different) elements in v s.t. their sum is the given sum
// This solution is O(n^2) in time and O(1) in space.
template<typename T>
triple<T> sum3(std::vector<T> v, T sum) {
    assert(v.size() >= 3);

    std::sort(v.begin(), v.end());

    for (auto it = v.begin(); it != v.end() - 2; ++it) {
        auto left = it + 1;
        auto right = v.end() - 1;

        while (left < right) {
            auto s = *it + *left + *right;
            if (s == sum) {
                return triple<T>(*it, *left, *right);
            } else if (s < sum) {
                ++left;
            } else {
                --right;
            }
        }
    }

    return triple<T>(0, 0, 0);
}


TEST_CASE("Find three unsigned elements with a given sum", "[3sum]") {
    std::vector<size_t> v{5, 2, 4, 6, 1, 3};

    triple<size_t> invalid(0, 0, 0);
    REQUIRE(sum3<size_t>(v, 1) == invalid);
    REQUIRE(sum3<size_t>(v, 2) == invalid);
    REQUIRE(sum3<size_t>(v, 3) == invalid);
    REQUIRE(sum3<size_t>(v, 4) == invalid);
    REQUIRE(sum3<size_t>(v, 5) == invalid);
    REQUIRE(sum3<size_t>(v, 6) == triple<size_t>(1, 2, 3));
    REQUIRE(sum3<size_t>(v, 7) == triple<size_t>(1, 2, 4));
    REQUIRE(sum3<size_t>(v, 8) == triple<size_t>(1, 2, 5));
    REQUIRE(sum3<size_t>(v, 9) == triple<size_t>(1, 2, 6));
    REQUIRE(sum3<size_t>(v, 10) == triple<size_t>(1, 3, 6));
    REQUIRE(sum3<size_t>(v, 15) == triple<size_t>(4, 5, 6));
    REQUIRE(sum3<size_t>(v, 16) == invalid);
}

TEST_CASE("Find three signed elements with a given sum", "[3sum]") {
    std::vector<int> v{5, 2, 4, -6, 1, 3};

    triple<int> invalid(0, 0, 0);
    REQUIRE(sum3<int>(v, 1) == triple<int>(-6, 2, 5));
    REQUIRE(sum3<int>(v, 16) == invalid);
}
