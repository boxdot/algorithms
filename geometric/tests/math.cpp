#include "../math.h"
#include <catch.hpp>
#include <assert.h>
#include <random>
#include <iostream>

using namespace math;


TEST_CASE("Determinant 3d", "[det]") {
    Matrix3 mat;

    REQUIRE(det(mat) == 1.f);

    mat = Matrix3
        { 1, 0, 0
        , 0, 2, 0
        , 0, 0, 3
        };
    REQUIRE(det(mat) == 6.f);

    mat = Matrix3
        { 1, 2, 3
        , 4, 5, 6
        , 7, 8, 9
        };
    REQUIRE(det(mat) == 0.f);
}

TEST_CASE("Determinant 4d", "[det]") {
    Matrix4 mat;

    REQUIRE(det(mat) == 1.f);

    mat = Matrix4
        { 1, 0, 0, 0
        , 0, 2, 0, 0
        , 0, 0, 3, 0
        , 0, 0, 0, 4
        };
    REQUIRE(det(mat) == 24.f);

    mat = Matrix4
        { 1, 2, 3, 4
        , 5, 6, 6, 7
        , 8, 9, 10, 11
        , 12, 13, 14, 15
        };
    REQUIRE(det(mat) == 0.f);
}

TEST_CASE("Orient 2d", "[orient]") {
    Vector2 a{0, 0}, b{1, 0};
    REQUIRE(orient(a, b, {1, 1}) > 0);
    REQUIRE(orient(a, b, {1, -1}) < 0);
    REQUIRE(orient(a, b, {2, 0}) == 0);
}

TEST_CASE("Orient 3d", "[orient]") {
    Vector3 a{0, 0, 0}, b{2, 0, 0}, c{1, 1, 0};
    REQUIRE(orient(a, b, c, {0, 0, 1}) < 0);
    REQUIRE(orient(a, b, c, {0, 0, -1}) > 0);
    REQUIRE(orient(a, b, c, {0, 0, 0}) == 0);
}

TEST_CASE("In circle", "[is_incircle]") {
    Vector2 a{-1, 0}, b{1, 0}, c{0, 1};
    REQUIRE(is_incircle(a, b, c, {0, 0}) > 0);
    REQUIRE(is_incircle(a, b, c, {0, 10}) < 0);
    REQUIRE(is_incircle(a, b, c, {0, -1}) == 0);
}

TEST_CASE("In sphere", "[is_insphere]") {
    Vector3 a{-1, 0, 0}, b{1, 0, 0}, c{0, 1, 0}, d{0, 0, -1};
    assert(orient(a, b, c, d) > 0);
    REQUIRE(is_insphere(a, b, c, d, {0, 0, 0}) > 0);
    REQUIRE(is_insphere(a, b, c, d, {0, 10, 0}) < 0);
    REQUIRE(is_insphere(a, b, c, d, {0, 0, 1}) == 0);
}

TEST_CASE("Pi approx. through is_incircle", "[is_incircle]") {
    static constexpr size_t N = 1e6;  // number of experiments

    std::default_random_engine gen;
    std::uniform_real_distribution<float> dist(-1, 1);

    Vector2 a{-1, 0}, b{1, 0}, c{0, 1};

    size_t is_incircle_count = 0;
    for (size_t i = 0; i < N; ++i) {
        Vector2 pt{dist(gen), dist(gen)};
        is_incircle_count += is_incircle(a, b, c, pt) > 0;
    }

    double pi_approximation = 4. * is_incircle_count / N;
    REQUIRE(static_cast<int>(pi_approximation * 100) == 314);
}
