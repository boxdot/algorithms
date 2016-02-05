#include "tools/catch.hpp"
#include <stdint.h>
#include <iostream>
#include <assert.h>
#include <iomanip>


constexpr uint64_t bitmask(int bits) {
    if (bits == 0) {
        return 0;
    }
    return (bitmask(bits - 1) << 1) + 1;
}


template<typename T>
class xorshift64star;


template<>
class xorshift64star<uint64_t> {
public:
    explicit xorshift64star(uint64_t seed) : seed_(seed) { assert(seed != 0); }
    uint64_t operator()() {
        seed_ ^= seed_ >> 12; // a
        seed_ ^= seed_ << 25; // b
        seed_ ^= seed_ >> 27; // c
        return seed_ * 2685821657736338717ULL;
    }

private:
    uint64_t seed_;
};


template<typename T>
class xorshift64star {
public:
    explicit xorshift64star(uint64_t seed) : gen_(seed) { assert(seed != 0); }
    T operator()() {
        return std::ldexp(static_cast<T>(gen_() & MANTISSA_MASK), -DIGITS);
    }

    static constexpr int DIGITS = std::numeric_limits<T>::digits;
    static constexpr uint64_t MANTISSA_MASK = bitmask(DIGITS);
private:
    xorshift64star<uint64_t> gen_;
};


TEST_CASE("xorshift64star", "[xorshift64star]")
{
    std::cout << std::setprecision(100);

    xorshift64star<uint64_t> gen1(1);
    std::cout << gen1() << std::endl;
    std::cout << gen1() << std::endl;

    xorshift64star<double> gen2(1);
    std::cout << gen2() << std::endl;
    std::cout << gen2() << std::endl;

    xorshift64star<float> gen3(1);
    std::cout << gen3() << std::endl;
    std::cout << gen3() << std::endl;

    // uint32_t res = 2.328306436538696289e-10 * 2.328306436538696289e-10;
    // std::cout << res << std::endl;
}
