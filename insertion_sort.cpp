#include <vector>

#include <random>
#include <gtest/gtest.h>

#include <iostream>


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


TEST(InsertionSort, Empty) {
    std::vector<int> ar;
    insertion_sort(ar);
    ASSERT_TRUE(ar == (std::vector<int>{}));
}


TEST(InsertionSort, Simple) {
    std::vector<int> ar{5, 2, 4, 6, 1, 3};
    insertion_sort(ar);
    ASSERT_TRUE(ar == (std::vector<int>{1, 2, 3, 4, 5, 6}));
}


TEST(InsertionSort, Trivial) {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar(n);
        for (int i = 0; i < n; ++i) {
            ar[i] = i;
        }
        auto copy_ar = ar;
        insertion_sort(ar);
        ASSERT_TRUE(ar == copy_ar);
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


TEST(InsertionSort, Random) {
    for (int n = 10; n <= 1000; n *= 10) {
        std::vector<int> ar = random_vector(n, 0, n/2);
        insertion_sort(ar);
        ASSERT_TRUE(is_sorted(ar));
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
