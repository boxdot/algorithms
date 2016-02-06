#include <random>
#include <algorithm>


template <typename T>
bool is_sorted(std::vector<T>& ar) {
    std::vector<T> ar_copy = ar;
    std::sort(ar_copy.begin(), ar_copy.end());
    return ar_copy == ar;
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