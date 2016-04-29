#pragma once

#include <cmath>

namespace math {


class Vector2 {
public:
    Vector2() = default;
    Vector2(const Vector2&) = default;

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
    Vector3(const Vector3&) = default;

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
    Matrix3() = default;
    explicit Matrix3(const Matrix3&) = default;

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
    Matrix4() = default;
    explicit Matrix4(const Matrix4&) = default;

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

} // end namespace math


//
// Inline implementation
//

namespace math {


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

} // namespace math
