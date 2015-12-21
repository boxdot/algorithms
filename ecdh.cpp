#include "ecdh.h"
#include "tools/catch.hpp"

#include <ostream>
#include <assert.h>
#include <iostream>
#include <random>

// Domain parameters of cryptographic system

template<int p, int a, int b>
struct DomainParams {
    using Fp = PF<p>;
    using E = EllipticCurve<Fp, a, b>;
    using Point = typename E::Point;

    const Point G;     // generator or base point
    const uint64_t n;  // order of G i.e. smallest positive n s.t. nG = O
    const uint64_t h;  // h = 1/n * #E(Fp), positive integer by Lagrange
                       // should be small, i.e. h < 5, preferably, h = 1
};

// Private key

template<typename DomainParams>
struct PrivateKey {
    explicit PrivateKey(const DomainParams& params) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, params.n - 1);
        d = distr(gen);
    }

    int d;  // integer from [1, n - 1], where n is from params
};

// Public key

template<typename DomainParams>
struct PublicKey {
    using Point = typename DomainParams::E::Point;

    PublicKey(const DomainParams& params, const PrivateKey<DomainParams>& key)
        : Q(key.d * params.G) {}

    const Point Q;  // Q = d*G, where G is from params, and d
                    // is the corresponding private key
};


TEST_CASE("ECDH on Curve25519 over F71", "[ECDH]") {
    // I use F71 since I don't have arithmetic on large integers.
    // TODO: Use prime 2^255 − 19
    using Curve25519 = DomainParams<71, 486662, 1>;
    using E = Curve25519::E;

    Curve25519 params = {
        E::Point(7, 16),  // generator
        74,               // of order 74 = #E(F71)
        1,                // since order of E(F71) = 74
    };

    // Alice Key
    PrivateKey<Curve25519> alice_priv(params);
    PublicKey<Curve25519> alice_pub(params, alice_priv);

    // Bob Key
    PrivateKey<Curve25519> bob_priv(params);
    PublicKey<Curve25519> bob_pub(params, bob_priv);

    // Exchange

    // Alice computes
    auto Pa = alice_priv.d * bob_pub.Q;

    // Bob computes
    auto Pb = bob_priv.d * alice_pub.Q;

    // Shared secret is Pa == Pb (first coordinate is enough)
    REQUIRE(Pa == Pb);
}

//
//  Other tests
//

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

TEST_CASE("Compute number of points on y^2 = x^3 + ax + b over F61 and F71",
    "[elliptic curve]")
{
    using F61 = PF<61>;
    using E61 = EllipticCurve<F61, -1, 0>;
    REQUIRE(E61::size() == 72);

    using F71 = PF<71>;
    using E71 = EllipticCurve<F71, -1, 0>;
    REQUIRE(E71::size() == 72);
}

TEST_CASE("Arithmetic on on y^2 = x^3 + ax + b over F7", "[elliptic curve]")
{
    using F = PF<7>;
    using E = EllipticCurve<F, -1, 0>;

    auto O = E::Point();
    for (int x = 0; x < F::size; ++x) {
        for (int y = 0; y < F::size; ++y) {
            if (E::contains(x, y)) {
                auto P = E::Point(x, y);
                REQUIRE(P - P == O);
                REQUIRE(P + O == P);
                REQUIRE(P - O == P);
                REQUIRE(O - P == -P);
            }
        }
    }
}
