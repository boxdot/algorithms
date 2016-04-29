#pragma once

#include "math.h"
#include <ostream>


inline std::ostream& operator<<(std::ostream& out, const math::Vector2& v) {
    return out << "(" << v.x << ", " << v.y << ")";
}


inline std::ostream& operator<<(std::ostream& out, const math::Vector3& v) {
    return out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}


inline std::ostream& operator<<(std::ostream& out, const math::Matrix4& mat) {
    return out << "("
        << mat.a1 << ", " << mat.a2 << ", " << mat.a3 << ", " << mat.a4
        << ",\n"
        << mat.b1 << ", " << mat.b2 << ", " << mat.b3 << ", " << mat.b4
        << ",\n"
        << mat.c1 << ", " << mat.c2 << ", " << mat.c3 << ", " << mat.c4
        << ",\n"
        << mat.d1 << ", " << mat.d2 << ", " << mat.d3 << ", " << mat.d4 << ")";
}
