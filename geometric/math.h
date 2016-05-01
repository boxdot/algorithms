#pragma once

#include <cmath>

namespace math {


class Vector2 {
public:
    Vector2() = default;
    Vector2(float x, float y);
    Vector2(const Vector2&) = default;
    Vector2(float val);

    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-() const;
    Vector2 operator-(const Vector2& other) const;
    float operator*(const Vector2& other) const;

    float square_norm() const;
    float norm() const;
    Vector2 normalize() const;

public:
    union {
        struct {
            float x, y;
        };
        float data[2];
    };
};


class Vector3 {
public:
    Vector3() = default;
    Vector3(float x, float y, float z);
    Vector3(const Vector3&) = default;
    Vector3(float val);

    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-() const;
    Vector3 operator-(const Vector3& other) const;
    float operator*(const Vector3& other) const;  // scalar product
    Vector3 operator^(const Vector3& other) const;  // vector product

    float square_norm() const;
    float norm() const;
    Vector3 normalize() const;

public:
    union {
        struct {
            float x, y, z;
        };
        float data[3];
    };
};


class Matrix3 {
public:
    Matrix3();
    Matrix3(
        float a1, float a2, float a3,
        float b1, float b2, float b3,
        float c1, float c2, float c3);
    Matrix3(const Matrix3&) = default;

public:
    union {
        struct {
            float a1, a2, a3;
            float b1, b2, b3;
            float c1, c2, c3;
        };
        float mdata[3][3];
        float data[9];
    };
};


class Matrix4 {
public:
    Matrix4();
    Matrix4(
        float a1, float a2, float a3, float a4,
        float b1, float b2, float b3, float b4,
        float c1, float c2, float c3, float c4,
        float d1, float d2, float d3, float d4);
    Matrix4(const Matrix4&) = default;

public:
    union {
        struct {
            float a1, a2, a3, a4;
            float b1, b2, b3, b4;
            float c1, c2, c3, c4;
            float d1, d2, d3, d4;
        };
        float mdata[4][4];
        float data[16];
    };
};


// Determinant predicates

// Determinats
float det(const Matrix3& mat);
float det(const Matrix4& mat);

// Orientation of c wrt directed line ab
//
// If return value is negative (positive), then c lies to the left (right) of
// the directed line ab. Equivalently the triangle abc is oriented
// counterclockwise (clockwise). When return value is 0, the three points are
// colinear.
float orient(const Vector2& a, const Vector2& b, const Vector2& c);

// Orientation of d wrt to the plane defined by triangle abc
//
// If return value is negative (positive, then d lies above (below) the plane
// defined by the triangle abc. When return value is 0, the four points are
// coplanar.
float orient(
    const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);

} // namespace math


//
// Inline implementation
//

namespace math {

Vector2::Vector2(float x, float y)
    : x(x), y(y)
{}

Vector2::Vector2(float val)
    : x(val), y(val)
{}

Vector2 Vector2::operator*(float scalar) const {
    return {scalar * x, scalar * y};
}

Vector2 Vector2::operator/(float scalar) const {
    return {x / scalar, y / scalar};
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return {x + other.x, y + other.y};
}

Vector2 Vector2::operator-() const {
    return {-x, -y};
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return {x - other.x, y - other.y};
}

float Vector2::operator*(const Vector2& other) const {
    return x * other.x + y * other.y;
}

float Vector2::square_norm() const {
    return x * x + y * y;
}

float Vector2::norm() const {
    return std::sqrt(square_norm());
}

Vector2 Vector2::normalize() const {
    return (*this) / norm();
}


Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{}

Vector3::Vector3(float val)
    : x(val), y(val), z(val)
{}

Vector3 Vector3::operator*(float scalar) const {
    return {scalar * x, scalar * y, scalar * z};
}

Vector3 Vector3::operator/(float scalar) const {
    return {x / scalar, y / scalar, z / scalar};
}

Vector3 Vector3::operator+(const Vector3& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vector3 Vector3::operator-() const {
    return {-x, -y, -z};
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

float Vector3::operator*(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::operator^(const Vector3& other) const {
    return
        { y * other.z - z * other.z
        , z * other.x - x * other.z
        , x * other.y - y * other.x
        };
}


float Vector3::square_norm() const {
    return x * x + y * y + z * z;
}

float Vector3::norm() const {
    return std::sqrt(square_norm());
}

Vector3 Vector3::normalize() const {
    return (*this) / norm();
}


Matrix3::Matrix3()
    : a1(1.f), a2(), a3()
    , b1(), b2(1.f), b3()
    , c1(), c2(), c3(1.f)
{}

Matrix3::Matrix3(
        float a1, float a2, float a3,
        float b1, float b2, float b3,
        float c1, float c2, float c3)
    : a1(a1), a2(a2), a3(a3)
    , b1(b1), b2(b2), b3(b3)
    , c1(c1), c2(c2), c3(c3)
{}


Matrix4::Matrix4()
    : a1(1.f), a2(), a3(), a4()
    , b1(), b2(1.f), b3(), b4()
    , c1(), c2(), c3(1.f), c4()
    , d1(), d2(), d3(), d4(1.f)
{}

Matrix4::Matrix4(
        float a1, float a2, float a3, float a4,
        float b1, float b2, float b3, float b4,
        float c1, float c2, float c3, float c4,
        float d1, float d2, float d3, float d4)
    : a1(a1), a2(a2), a3(a3), a4(a4)
    , b1(b1), b2(b2), b3(b3), b4(b4)
    , c1(c1), c2(c2), c3(c3), c4(c4)
    , d1(d1), d2(d2), d3(d3), d4(d4)
{}


float det(const Matrix3& mat) {
    return
        mat.a1 * (mat.b2 * mat.c3 - mat.b3 * mat.c2) +
        mat.a2 * (mat.b3 * mat.c1 - mat.b1 * mat.c3) +
        mat.a3 * (mat.b1 * mat.c2 - mat.b2 * mat.c1);
}

float det(const Matrix4& mat) {
    return
          mat.a1*mat.b2*mat.c3*mat.d4 - mat.a1*mat.b2*mat.c4*mat.d3 +
            mat.a1*mat.b3*mat.c4*mat.d2 - mat.a1*mat.b3*mat.c2*mat.d4
        + mat.a1*mat.b4*mat.c2*mat.d3 - mat.a1*mat.b4*mat.c3*mat.d2 -
            mat.a2*mat.b3*mat.c4*mat.d1 + mat.a2*mat.b3*mat.c1*mat.d4
        - mat.a2*mat.b4*mat.c1*mat.d3 + mat.a2*mat.b4*mat.c3*mat.d1 -
            mat.a2*mat.b1*mat.c3*mat.d4 + mat.a2*mat.b1*mat.c4*mat.d3
        + mat.a3*mat.b4*mat.c1*mat.d2 - mat.a3*mat.b4*mat.c2*mat.d1 +
            mat.a3*mat.b1*mat.c2*mat.d4 - mat.a3*mat.b1*mat.c4*mat.d2
        + mat.a3*mat.b2*mat.c4*mat.d1 - mat.a3*mat.b2*mat.c1*mat.d4 -
            mat.a4*mat.b1*mat.c2*mat.d3 + mat.a4*mat.b1*mat.c3*mat.d2
        - mat.a4*mat.b2*mat.c3*mat.d1 + mat.a4*mat.b2*mat.c1*mat.d3 -
            mat.a4*mat.b3*mat.c1*mat.d2 + mat.a4*mat.b3*mat.c2*mat.d1;
}


float orient(const Vector2& a, const Vector2& b, const Vector2& c) {
    return det(Matrix3
        { a.x, a.y, 1.f
        , b.x, b.y, 1.f
        , c.x, c.y, 1.f
        });
}

float orient(
    const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d)
{
    return det(Matrix4
        { a.x, a.y, a.z, 1.f
        , b.x, b.y, b.z, 1.f
        , c.x, c.y, c.z, 1.f
        , d.x, d.y, d.z, 1.f
        });
}

} // namespace math
