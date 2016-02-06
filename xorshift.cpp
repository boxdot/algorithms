#include <catch.hpp>
#include <stdint.h>
#include <iostream>
#include <type_traits>
#include <assert.h>
#include <iomanip>

extern "C"{

#include <unif01.h>
#include <bbattery.h>

}


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
    explicit xorshift64star(uint64_t seed) : gen_(seed) {
        assert(seed != 0);
    }
    T operator()() {
        return std::ldexp(static_cast<T>(gen_() & MANTISSA_MASK), -DIGITS);
    }

    static constexpr int DIGITS = std::numeric_limits<T>::digits;
    static constexpr uint64_t MANTISSA_MASK = bitmask(DIGITS);

    static_assert(std::is_floating_point<T>::value,
        "T expected to be a floating point type");
    static_assert(DIGITS <= 64, "T is too big");
private:
    xorshift64star<uint64_t> gen_;
};


TEST_CASE("xorshift64star smoke test", "[xorshift64star]")
{
    std::cout << std::setprecision(100);

    xorshift64star<uint64_t> gen1(1);
    REQUIRE(gen1() % 10000U == 5165);
    REQUIRE(gen1() % 10000U == 1517);

    xorshift64star<double> gen2(1);
    REQUIRE(static_cast<int>(gen2() * 10000) == 1501);
    REQUIRE(static_cast<int>(gen2() * 10000) == 4890);

    xorshift64star<float> gen3(1);
    REQUIRE(static_cast<int>(gen3() * 10000) == 4252);
    REQUIRE(static_cast<int>(gen3() * 10000) == 4741);
}


xorshift64star<double> gen(1);
double genfun(void) {
    return gen();
}

// Runtime of this test on my mid 2014 macbook pro is 4h, so I've disabled it.
// Tested with https://www.iro.umontreal.ca/~simardr/testu01/tu01.html.
TEST_CASE("xorshift64star testu01 bigcrush", "[.][bigcrush][xorshift64star]")
{
    auto gen = unif01_CreateExternGen01("xorshift64star", genfun);
    bbattery_BigCrush(gen);
    unif01_DeleteExternGen01(gen);

    REQUIRE(true);
}
