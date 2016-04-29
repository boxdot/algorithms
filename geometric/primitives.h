#pragma once

#include "math.h"


class Triangle {
public:
    math::Vector3 a, b, c;
};


class CachedTriangle : public Triangle {
public:
    CachedTriangle(
        const math::Vector3& a,
        const math::Vector3& b,
        const math::Vector3& c);
    CachedTriangle(const Triangle& tri);
    CachedTriangle(const CachedTriangle&) = default;

    friend void barycentric(
        const CachedTriangle& tri, const math::Vector3& p,
        float& u, float& v, float& w);

    friend void areal(
        const CachedTriangle& tri, const math::Vector3& p,
        float& u, float& v, float& w);

private:
    math::Vector3 v0 = b - a, v1 = c - a;
    math::Vector3 normal = v0 ^ v1;  // non-normalized
    float d00 = v0 * v0;
    float d01 = v0 * v1;
    float d11 = v1 * v1;
    float denom = d00 * d11 - d01 * d01;
};


//
// Inline implementation
//

CachedTriangle::CachedTriangle(
    const math::Vector3& a, const math::Vector3& b, const math::Vector3& c)
: Triangle{a, b, c}
{}

CachedTriangle::CachedTriangle(const Triangle& tri)
: CachedTriangle(tri.a, tri.b, tri.c)
{}
