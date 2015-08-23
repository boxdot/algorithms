#include <vector>
#include <random>

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"


template<typename T>
void insertion_sort(std::vector<T>& ar) {
    for (int i = 1; i < ar.size(); ++i) {
        const T key = ar[i];
        int j = i - 1;
        while (j >= 0 && ar[j] > key) {
            ar[j + 1] = ar[j];
            j--;
        }
        ar[j + 1] = key;
    }
}


template <typename T>
bool is_sorted(std::vector<T>& ar) {
    std::vector<T> ar_copy = ar;
    std::sort(ar_copy.begin(), ar_copy.end());
    return ar_copy == ar;
}


TEST_CASE("Sort empty vector", "[insertion_sort]") {
    std::vector<int> ar;
    insertion_sort(ar);
    REQUIRE(ar == (std::vector<int>{}));
}


TEST_CASE("Sort a small vector", "[insertion_sort]") {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    insertion_sort(ar);
    REQUIRE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST_CASE("Sort several sorted vectors", "[insertion_sort]") {
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


std::vector<int> random_vector(std::size_t size, int from, int to) {
    std::default_random_engine engine;
    std::uniform_int_distribution<int> randint(from, to);

    std::vector<int> vec(size);
    while (size > 0) {
        vec[size - 1] = randint(engine);
        size--;
    }
    return vec;
}


TEST_CASE("Sort several random vectors", "[insertion_sort]") {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        insertion_sort(ar);
        REQUIRE(is_sorted(ar));
    }
}
