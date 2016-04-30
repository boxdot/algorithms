#include "tools/sort.hpp"
#include <catch.hpp>
#include <vector>


template<typename T>
void insertion_sort(std::vector<T>& ar) {
    for (size_t i = 1; i < ar.size(); ++i) {
        const T key = ar[i];
        size_t j = i - 1;
        while (j >= 0 && ar[j] > key) {
            ar[j + 1] = ar[j];
            j--;
        }
        ar[j + 1] = key;
    }
}


template <typename T>
void merge(
        std::vector<T>& ar,
        std::size_t p,
        std::size_t q,
        std::size_t r)
{
    std::vector<int> left(ar.begin() + p, ar.begin() + q);
    std::vector<int> right(ar.begin() + q, ar.begin() + r);
    size_t i = 0;
    size_t j = 0;
    size_t k = p;
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

//
// Tests
//

TEST_CASE("Sort empty vector with insertion_sort", "[insertion_sort]") {
    std::vector<int> ar;
    insertion_sort(ar);
    REQUIRE(ar == (std::vector<int>{}));
}


TEST_CASE("Sort a small vector with insertion_sort", "[insertion_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    insertion_sort(ar);
    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST_CASE("Sort several sorted vectors with insertion_sort",
    "[insertion_sort]")
{
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar(n);
        for (int i = 0; i < n; ++i) {
            ar[i] = i;
        }
        auto copy_ar = ar;
        insertion_sort(ar);
        REQUIRE(ar == copy_ar);
    }
}


TEST_CASE("Sort several random vectors with insertion_sort",
    "[insertion_sort]")
{
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        insertion_sort(ar);
        REQUIRE(is_sorted(ar));
    }
}

TEST_CASE("Merge two vectors", "[merge]") {
    std::vector<int> ar{1, 3, 5, 2, 4, 6, 7};
    merge(ar, 0, 3, 7);

    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
}

TEST_CASE("Sort empty vector with merge_sort", "[merge_sort]") {
    std::vector<int> ar;
    merge_sort(ar);
    REQUIRE(ar == (std::vector<int>{}));
}


TEST_CASE("Sort a small vector with merge_sort", "[merge_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    merge_sort(ar);
    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST_CASE("Sort several sorted vectors with merge_sort", "[merge_sort]") {
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


TEST_CASE("Sort several random vectors with merge_sort", "[merge_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        merge_sort(ar);
        REQUIRE(is_sorted(ar));
    }
}


TEST_CASE("Sort empty vector with radix_sort", "[radix_sort]") {
    std::vector<int> ar;
    radix_sort(ar);
    REQUIRE(ar == (std::vector<int>{}));
}


TEST_CASE("Sort a small vector with radix_sort", "[radix_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    radix_sort(ar);
    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST_CASE("Sort several sorted vectors with radix_sort", "[radix_sort]") {
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


TEST_CASE("Sort several random vectors with radix_sort", "[radix_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        radix_sort(ar);
        REQUIRE(is_sorted(ar));
    }
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
