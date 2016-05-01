#include "../math.h"
#include <catch.hpp>

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
