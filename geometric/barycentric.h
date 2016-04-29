#pragma once

#include "math.h"
#include "primitives.h"


// Compute barycentric coordinates (u, v, w) for point p with respect to
// triangle tri
inline void barycentric(
    const Triangle& tri, const math::Vector3& p,
    float& u, float& v, float& w)
{
    auto v0 = tri.b - tri.a, v1 = tri.c - tri.a, v2 = p - tri.a;
    float d00 = v0 * v0;
    float d01 = v0 * v1;
    float d11 = v1 * v1;
    float d20 = v2 * v0;
    float d21 = v2 * v1;
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.f - v - w;
}


// Compute barycentric coordinates (u, v, w) for point p with respect to
// triangle tri (containing precomputed values)
inline void barycentric(
    const CachedTriangle& tri, const math::Vector3& p,
    float& u, float& v, float& w)
{
    auto v2 = p - tri.a;
    float d20 = v2 * tri.v0;
    float d21 = v2 * tri.v1;
    v = (tri.d11 * d20 - tri.d01 * d21) / tri.denom;
    w = (tri.d00 * d21 - tri.d01 * d20) / tri.denom;
    u = 1.f - v - w;
}


namespace detail {

inline auto triangle_area_2d(
    float x1, float y1, float x2, float y2, float x3, float y3)
{
    return (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
}

} // namespace detail


// Compute barycentric coordinates (u, v, w) for point p with respect to
// triangle tri (using area ratio)
inline void areal(
    const Triangle& tri, const math::Vector3& p,
    float& u, float& v, float& w)
{
    // non-normalized normal of triangle
    auto n = (tri.b - tri.a) ^ (tri.c - tri.a);
    float nu, nv, ood;
    float x = std::abs(n.x);
    float y = std::abs(n.y);
    float z = std::abs(n.z);

    if (x >= y && x >= z) {
        // area PBC in yz
        nu = detail::triangle_area_2d(
            p.y, p.z, tri.b.y, tri.b.z, tri.c.z, tri.c.z);
        // area PCA in yz
        nv = detail::triangle_area_2d(
            p.y, p.z, tri.c.y, tri.c.z, tri.a.z, tri.a.z);
        ood = 1.f / n.x;  // 1 / (2 * area of ABC in yz)
    } else if (y >= x && y >= z) {
        // area PBC in xz
        nu = detail::triangle_area_2d(
            p.x, p.z, tri.b.x, tri.b.z, tri.c.x, tri.c.z);
        // area PCA in xz
        nv = detail::triangle_area_2d(
            p.x, p.z, tri.c.x, tri.c.z, tri.a.x, tri.a.z);
        ood = 1.f / n.y;  // 1 / (2 * area of ABC in xz)
    } else {
        // area PBC in xy
        nu = detail::triangle_area_2d(
            p.x, p.y, tri.b.x, tri.b.y, tri.c.x, tri.c.y);
        // area PCA in xy
        nv = detail::triangle_area_2d(
            p.x, p.y, tri.c.x, tri.c.y, tri.a.x, tri.a.y);
        ood = 1.f / n.z;  // 1 / (2 * area of ABC in xy)
    }

    u = nu * ood;
    v = nv * ood;
    w = 1.f - u - v;
}


// Compute barycentric coordinates (u, v, w) for point p with respect to
// triangle tri (using area ratio and precomputed values)
inline void areal(
    const CachedTriangle& tri, const math::Vector3& p,
    float& u, float& v, float& w)
{
    // non-normalized normal of triangle
    auto n = tri.normal;
    float nu, nv, ood;
    float x = std::abs(n.x);
    float y = std::abs(n.y);
    float z = std::abs(n.z);

    if (x >= y && x >= z) {
        // area PBC in yz
        nu = detail::triangle_area_2d(
            p.y, p.z, tri.b.y, tri.b.z, tri.c.z, tri.c.z);
        // area PCA in yz
        nv = detail::triangle_area_2d(
            p.y, p.z, tri.c.y, tri.c.z, tri.a.z, tri.a.z);
        ood = 1.f / n.x;  // 1 / (2 * area of ABC in yz)
    } else if (y >= x && y >= z) {
        // area PBC in xz
        nu = detail::triangle_area_2d(
            p.x, p.z, tri.b.x, tri.b.z, tri.c.x, tri.c.z);
        // area PCA in xz
        nv = detail::triangle_area_2d(
            p.x, p.z, tri.c.x, tri.c.z, tri.a.x, tri.a.z);
        ood = 1.f / n.y;  // 1 / (2 * area of ABC in xz)
    } else {
        // area PBC in xy
        nu = detail::triangle_area_2d(
            p.x, p.y, tri.b.x, tri.b.y, tri.c.x, tri.c.y);
        // area PCA in xy
        nv = detail::triangle_area_2d(
            p.x, p.y, tri.c.x, tri.c.y, tri.a.x, tri.a.y);
        ood = 1.f / n.z;  // 1 / (2 * area of ABC in xy)
    }

    u = nu * ood;
    v = nv * ood;
    w = 1.f - u - v;
}
