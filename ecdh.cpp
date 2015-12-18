#include "tools/catch.hpp"

#include <ostream>
#include <iostream>

//
// Prime finite field (of characteristic p)
//

template<int p /* prime */>
class PF {
public:
    static constexpr int characteristic = p;
    static constexpr int size = p;

    PF(int a) : a_(a % p + p * (a % p < 0)) {}

    int operator()() const { return a_; }
    operator int() const { return a_; }

private:
    int a_;
};


template<int p>
PF<p> operator+(const PF<p> x, const PF<p> y) {
    return PF<p>(x() + y());
}

template<int p>
PF<p> operator-(const PF<p> x, const PF<p> y) {
    return PF<p>(x() - y());
}

template<int p>
PF<p> operator*(const PF<p> x, const PF<p> y) {
    return PF<p>(x() * y());
}

template<int p>
PF<p> operator/(const PF<p> x, const PF<p> y) {
    return PF<p>(x() / y());
}

template<int p>
bool operator==(const PF<p> x, const PF<p> y) {
    return x() == y();
}

template<int p>
std::ostream& operator<<(std::ostream& os, const PF<p> x) {
    return os << x();
}


TEST_CASE("Value of negative int is positive", "[finite field]") {
    using F7 = PF<7>;

    REQUIRE(F7(-1)() == 6);
    REQUIRE(F7(-10)() == 4);
}


TEST_CASE("Check arithmetic of F<p> elements", "[finite field]") {
    using F7 = PF<7>;

    REQUIRE(F7(0) + F7(7) == F7(0));
    REQUIRE(F7(1) + F7(7) == F7(1));
    REQUIRE(F7(1) + F7(8) == F7(2));
    REQUIRE(F7(-1) + F7(2) == F7(1));

    REQUIRE(F7(0) - F7(7) == F7(-7));
    REQUIRE(F7(1) - F7(7) == F7(-6));
    REQUIRE(F7(1) - F7(8) == F7(0));
    REQUIRE(F7(-1) - F7(2) == F7(4));

    REQUIRE(F7(0) * F7(7) == F7(0));
    REQUIRE(F7(1) * F7(7) == F7(0));
    REQUIRE(F7(1) * F7(8) == F7(1));
    REQUIRE(F7(-1) * F7(2) == F7(5));

    REQUIRE(F7(7) / F7(1) == F7(0));
    REQUIRE(F7(1) / F7(8) == F7(1));
    REQUIRE(F7(-1) / F7(2) == F7(3));
}


//
// Finite field (or Galois Field)
//

constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1) {
    return exp < 1
        ? result
        : ipow(base*base, exp/2, (exp % 2) ? result*base : result);
}

constexpr int64_t pow(int64_t base, int exp) {
    return ipow(base, exp, 1);
}


template<int p /* prime */, int k /* exponent */>
class GF {
public:
    static constexpr int characteristic = p;
    static constexpr int size = pow(p, k);

    // TODO: Implement using polynomial arithmetic.
};


//
// Elliptic curve
//

template<typename F /* field of char not 2 or 3 */>
class EllipticCurve {
public:
    EllipticCurve(const F a, const F b) : a_(a), b_(b) {}

    // check if the given point lies on the curve
    bool operator()(const F x, const F y) const {
        return y*y*y == x*x + a_*x + b_;
    }

    template<typename L /* finite extension of F */>
    size_t size() {
        size_t count = 1;  // first point is the infinity
        for (int x = 0; x < L::size; ++x) {
            for (int y = 0; y < L::size; ++y) {
                if ((*this)(x, y)) {
                    count +=1;
                }
            }
        }
        return count;
    }

    // TODO: Implement group law

private:
    F a_, b_;
};


TEST_CASE("Compute number of points on y^2 = x^3 + ax + b over F61",
    "[elliptic curve]")
{
    using F = PF<61>;
    EllipticCurve<F> E(-1, 0);
    REQUIRE(E.size<F>() == 63);
}
