#include "../barycentric.h"
#include "../print.h"
#include <catch.hpp>
#include <random>
#include <chrono>
#include <iostream>

using namespace math;


Vector3 random_vector() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0, 1);
    return {dis(gen), dis(gen), dis(gen)};
}


Triangle random_triangle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0, 1);
    return
        { {dis(gen), dis(gen), dis(gen)}
        , {dis(gen), dis(gen), dis(gen)}
        , {dis(gen), dis(gen), dis(gen)}
        };
}


TEST_CASE("barycentric", "[barycentric]") {
    Vector3 a{-1, 0, 0};
    Vector3 b{1, 0, 0};
    Vector3 c{0, 1, 0};
    Triangle tri{a, b, c};
    Vector3 p{0, 0.5f, 0};

    float u, v, w;
    barycentric(tri, p, u, v, w);

    REQUIRE(u == 0.25f);
    REQUIRE(v == 0.25f);
    REQUIRE(w == 0.5f);
}

TEST_CASE("barycentric cached", "[barycentric]") {
    Vector3 a{-1, 0, 0};
    Vector3 b{1, 0, 0};
    Vector3 c{0, 1, 0};
    CachedTriangle tri{a, b, c};
    Vector3 p{0, 0.5f, 0};

    float u, v, w;
    barycentric(tri, p, u, v, w);

    REQUIRE(u == 0.25f);
    REQUIRE(v == 0.25f);
    REQUIRE(w == 0.5f);
}

TEST_CASE("areal non-cached", "[barycentric]") {
    Vector3 a{-1, 0, 0};
    Vector3 b{1, 0, 0};
    Vector3 c{0, 1, 0};
    Triangle tri{a, b, c};
    Vector3 p{0, 0.5f, 0};

    float u, v, w;
    areal(tri, p, u, v, w);

    REQUIRE(u == 0.25f);
    REQUIRE(v == 0.25f);
    REQUIRE(w == 0.5f);
}


// Non-measurable with O2!
TEST_CASE(
    "barycentric/arial performance non-cached vs. cached", "[barycentric]")
{
    constexpr size_t N = 10000;
    constexpr size_t FACTOR = 100;

    std::vector<Triangle> triangles;
    std::vector<CachedTriangle> cached_triangles;
    std::vector<Vector3> points;

    for (size_t i = 0; i != N; ++i) {
        triangles.push_back(random_triangle());
        cached_triangles.push_back(random_triangle());
        points.push_back(random_vector());
    }

    // non-cached barycentric
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i != N * FACTOR; ++i) {
        float u, v, w;
        barycentric(triangles[i % N], points[i % N], u, v, w);
    }
    auto end = std::chrono::steady_clock::now();
    auto dur1 = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();

    // cached barycentric
    start = std::chrono::steady_clock::now();
    for (size_t i = 0; i != N * FACTOR; ++i) {
        float u, v, w;
        barycentric(cached_triangles[i % N], points[i % N], u, v, w);
    }
    end = std::chrono::steady_clock::now();
    auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();

    // non-cached areal
    start = std::chrono::steady_clock::now();
    for (size_t i = 0; i != N * FACTOR; ++i) {
        float u, v, w;
        areal(triangles[i % N], points[i % N], u, v, w);
    }
    end = std::chrono::steady_clock::now();
    auto dur3 = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();

    // cached areal
    start = std::chrono::steady_clock::now();
    for (size_t i = 0; i != N * FACTOR; ++i) {
        float u, v, w;
        areal(cached_triangles[i % N], points[i % N], u, v, w);
    }
    end = std::chrono::steady_clock::now();
    auto dur4 = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();

    std::cerr << "Runtime:" << std::endl;
    std::cerr << "Non-cached (barycentric): " << dur1 << "ms" << std::endl;
    std::cerr << "Cached     (barycentric): " << dur2 << "ms" << std::endl;
    std::cerr << "Non-cached (areal)      : " << dur3 << "ms" << std::endl;
    std::cerr << "Cached     (areal)      : " << dur4 << "ms" << std::endl;
}
