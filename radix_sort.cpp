#include "tools/sort.hpp"
#include "tools/catch.hpp"


// type contained in Iter is assumed to be an integral number type
template<typename Iter>
void radix_sort(Iter begin, Iter end, size_t bit)
{
    if (begin == end){
        return;
    }

    auto bin0 = begin;
    auto bin1 = end;

    while (bin0 < bin1) {
        auto it = bin0;
        if ((*it >> bit) & 1) {
            std::swap(*it, *(--bin1));
        } else {
            ++bin0;
        }
    }

    if (bit != 0) {
        radix_sort(begin, bin0, bit - 1);
        radix_sort(bin1, end, bit - 1);
    }
}

template<typename T>
void radix_sort(std::vector<T>& v) {
    radix_sort(v.begin(), v.end(), sizeof(T)*8 - 1);
}


TEST_CASE("Sort empty vector", "[radix_sort]") {
    std::vector<int> ar;
    radix_sort(ar);
    REQUIRE(ar == (std::vector<int>{}));
}


TEST_CASE("Sort a small vector", "[radix_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    radix_sort(ar);
    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST_CASE("Sort several sorted vectors", "[radix_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar(n);
        for (int i = 0; i < n; ++i) {
            ar[i] = i;
        }
        auto copy_ar = ar;
        radix_sort(ar);
        REQUIRE(ar == copy_ar);
    }
}


TEST_CASE("Sort several random vectors", "[radix_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        radix_sort(ar);
        REQUIRE(is_sorted(ar));
    }
}


// Works only for vector with positive elements. However, it is possible
// without loss of generality to reduce to this case.
std::pair<int, int> find_with_sum(std::vector<int> v, int sum) {
    radix_sort(v);
    auto left = v.begin();
    auto right = v.end() - 1;
    while (left <= right) {
        auto s = *left + *right;
        if (s == sum) {
            return std::make_pair(*left, *right);
        } else if (s < sum) {
            ++left;
        } else {
            --right;
        }
    }
    return std::make_pair(-1, -1);
}


TEST_CASE("Find two elements with a given sum", "[radix_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    REQUIRE(find_with_sum(ar, 1) == std::make_pair(-1, -1));
    REQUIRE(find_with_sum(ar, 2) == std::make_pair(1, 1));
    REQUIRE(find_with_sum(ar, 3) == std::make_pair(1, 2));
    REQUIRE(find_with_sum(ar, 4) == std::make_pair(1, 3));
    REQUIRE(find_with_sum(ar, 5) == std::make_pair(1, 4));
    REQUIRE(find_with_sum(ar, 7) == std::make_pair(1, 6));
    REQUIRE(find_with_sum(ar, 8) == std::make_pair(2, 6));
    REQUIRE(find_with_sum(ar, 9) == std::make_pair(3, 6));
    REQUIRE(find_with_sum(ar, 10) == std::make_pair(4, 6));
    REQUIRE(find_with_sum(ar, 11) == std::make_pair(5, 6));
    REQUIRE(find_with_sum(ar, 12) == std::make_pair(6, 6));
    REQUIRE(find_with_sum(ar, 13) == std::make_pair(-1, -1));
}
