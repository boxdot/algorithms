#include "ecdh.h"
#include "tools/catch.hpp"

#include <ostream>
#include <assert.h>
#include <iostream>


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

    REQUIRE(F7(1).inverse() == F7(1));
    REQUIRE(F7(2).inverse() == F7(4));
    REQUIRE(F7(3).inverse() == F7(5));
    REQUIRE(F7(4).inverse() == F7(2));
    REQUIRE(F7(5).inverse() == F7(3));
    REQUIRE(F7(6).inverse() == F7(6));

    REQUIRE(F7(7) / F7(1) == F7(0));
    REQUIRE(F7(1) / F7(8) == F7(1));
    REQUIRE(F7(-1) / F7(2) == F7(3));
    REQUIRE(F7(4) / F7(3) == F7(-1));
}


template<typename F>
std::string to_string(const typename EllipticCurve<F>::Element& p)
{
    if (p.is_identity()) {
        return std::string("o");
    }
    std::stringstream ss;
    ss << "(" << p.x() << ", " << p.y() << ")";
    return ss.str();
}


TEST_CASE("Compute number of points on y^2 = x^3 + ax + b over F61 and F71",
    "[elliptic curve]")
{
    using F61 = PF<61>;
    EllipticCurve<F61> E(-1, 0);
    REQUIRE(E.size<F61>() == 72);

    using F71 = PF<71>;
    EllipticCurve<F71> E2(-1, 0);
    REQUIRE(E2.size<F71>() == 72);
}

TEST_CASE("Arithmetic on on y^2 = x^3 + ax + b over F7", "[elliptic curve]")
{
    using F = PF<7>;
    EllipticCurve<F> E(-1, 0);

    auto o = E();
    for (int x = 0; x < F::size; ++x) {
        for (int y = 0; y < F::size; ++y) {
            if (E.contains(x, y)) {
                auto p = E(x, y);
                REQUIRE(p - p == o);
                REQUIRE(p + o == p);
                REQUIRE(p - o == p);
                REQUIRE(o - p == -p);
            }
        }
    }
}
