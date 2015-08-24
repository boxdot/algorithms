#include <vector>

#include "tools/sort.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"


template <typename T>
void merge(
        std::vector<T>& ar,
        std::size_t p,
        std::size_t q,
        std::size_t r)
{
    std::vector<int> left(ar.begin() + p, ar.begin() + q);
    std::vector<int> right(ar.begin() + q, ar.begin() + r);
    int i = 0;
    int j = 0;
    int k = p;
    for (; k < r && i < left.size() && j < right.size(); ++k)
    {
        if (left[i] < right[j]) {
            ar[k] = left[i];
            i++;
        } else {
            ar[k] = right[j];
            j++;
        }
    }
    for (; i < left.size(); ++i) {
        ar[k] = left[i];
        k++;
    }
    for (; j < right.size(); ++j) {
        ar[k] = right[j];
        k++;
    }
}


TEST_CASE("Merge two vectors", "[merge]") {
    std::vector<int> ar{1, 3, 5, 2, 4, 6, 7};
    merge(ar, 0, 3, 7);

    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
}


template <typename T>
void merge_sort(std::vector<T>& ar, std::size_t p, std::size_t r) {
    if (r - p > 1) {
        auto q = (p + r)/2;
        merge_sort(ar, p, q);
        merge_sort(ar, q, r);
        merge(ar, p, q, r);
    }
}


template <typename T>
void merge_sort(std::vector<T>& ar) {
    merge_sort(ar, 0, ar.size());
}


TEST_CASE("Sort empty vector", "[merge_sort]") {
    std::vector<int> ar;
    merge_sort(ar);
    REQUIRE(ar == (std::vector<int>{}));
}


TEST_CASE("Sort a small vector", "[merge_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    merge_sort(ar);
    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST_CASE("Sort several sorted vectors", "[merge_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar(n);
        for (int i = 0; i < n; ++i) {
            ar[i] = i;
        }
        auto copy_ar = ar;
        merge_sort(ar);
        REQUIRE(ar == copy_ar);
    }
}


TEST_CASE("Sort several random vectors", "[merge_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        merge_sort(ar);
        REQUIRE(is_sorted(ar));
    }
}
