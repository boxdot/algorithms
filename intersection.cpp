#include <catch.hpp>


struct Vector3D {
    float x, y, z;
};


// scalar multiplication
Vector3D operator*(const float a, const Vector3D& v) {
    return Vector3D{a*v.x, a*v.y, a*v.z};
}

// vector addition
Vector3D operator+(const Vector3D& v, const Vector3D& w) {
    return Vector3D{v.x + w.x, v.y + w.y, v.z + w.z};
}

// vector subtraction
Vector3D operator-(const Vector3D& v, const Vector3D& w) {
    return Vector3D{v.x - w.x, v.y - w.y, v.z - w.z};
}

// cross product
Vector3D operator^(const Vector3D& v, const Vector3D& w) {
    return Vector3D{v.y*w.z - v.z*w.y, v.z*w.x - v.x*w.z, v.x*w.y - v.y*w.x};
}

// scalar product
float operator*(const Vector3D& v, const Vector3D& w) {
    return v.x*w.x + v.y*w.y + v.z*w.z;
}


//
// Return -1 if there is no intersection, otherwise return r s.t. the
// intersection point is orig + r * dir.
//
// If n is normalized, then r is exactly the euklidean distance from the ray to
// the plane.
//
// Args:
//   orig, dir: Points defining a ray from orig in direction dir
//   p, n: Plane through p with normal n
//
// Cf. http://geomalgorithms.com/a06-_intersect-2.html
//
float ray_plane_intersection(
    const Vector3D& orig, const Vector3D& dir,
    const Vector3D& p, const Vector3D& n)
{
    auto denom = n * dir;
    if (denom == 0) {
        return -1;
    }

    auto nom = n * (p - orig);
    auto r = nom / denom;

    return r < 0 ? -1 : r;
}


//
// Return -1 if there is no intersection, otherwise return r s.t. the
// intersection point is P0 + r * (P1 - P0).
//
// If n is normalized, then r is exactly the euklidean distance from the ray to
// the triangle.
//
// Args:
//   orig, dir: Points defining a ray from orig in direction dir
//   p0, p1, p2: Points defining a triangle
//
// Cf. http://geomalgorithms.com/a06-_intersect-2.html
//
float ray_triangle_intersection(
    const Vector3D& orig, const Vector3D& dir,
    const Vector3D& p0, const Vector3D& p1, const Vector3D& p2)
{
    auto u = p1 - p0;
    auto v = p2 - p0;
    auto n = u^v;  // normal vector of the triangle

    auto r = ray_plane_intersection(orig, dir, p0, n);
    if (r == -1) {
        return -1;
    }

    auto p_int = orig + r * dir;
    auto w = p_int - p0;

    // precompute scalar products
    auto uv = u*v;
    auto wv = w*v;
    auto vv = v*v;
    auto wu = w*u;
    auto uu = u*u;

    auto denom = uv * uv - uu * vv;
    auto s = (uv * wv - vv * wu) / denom;

    if (s < 0) {
        return -1;
    }

    auto t = (uv * wu - uu * wv) / denom;

    if (t < 0 || s + t > 1) {
        return -1;
    }

    return r;
}


//
// Intersect a ray with an AABB (axis-aligned bounding box).
//
// The AABB is defined by bounds, where bounds[0] is the minimal corner and
// bounds[1] is the maximal corner. The ray has origin at P0 and direction
// -dir_inv. The inversed direction allows to make the algorithm faster.
//
// Cf. http://people.csail.mit.edu/amy/papers/box-jgt.pdf
//
bool ray_box_intersection(
    const Vector3D& P0, const Vector3D& dir_inv,
    const Vector3D bounds[2])
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    // Normally, this should be also precomputed.
    auto sgn_x = static_cast<int>(dir_inv.x < 0);
    auto sgn_y = static_cast<int>(dir_inv.y < 0);
    auto sgn_z = static_cast<int>(dir_inv.z < 0);

    tmin = (bounds[sgn_x].x - P0.x) * dir_inv.x;
    tmax = (bounds[1 - sgn_x].x - P0.x) * dir_inv.x;
    tymin = (bounds[sgn_y].y - P0.y) * dir_inv.y;
    tymax = (bounds[1 - sgn_y].y - P0.y) * dir_inv.y;

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }
    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    tzmin = (bounds[sgn_z].z - P0.z) * dir_inv.z;
    tzmax = (bounds[1 - sgn_z].z - P0.z) * dir_inv.z;

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }
    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }

    return true;
}

//
// Tests
//

TEST_CASE("Ray plane intersection", "[intersection]") {
    REQUIRE(ray_plane_intersection(
        {0, 0, 0}, {0, 0, 1},
        {0, 0, 0}, {0, 0, 1}) == 0);

    REQUIRE(ray_plane_intersection(
        {0, 0, 0}, {0, 0, 1},
        {0, 0, 1}, {0, 0, 1}) == 1);

    REQUIRE(ray_plane_intersection(
        {0, 0, 0}, {0, 0, 1},
        {1, 1, 1}, {0, 0, 1}) == 1);

    REQUIRE(ray_plane_intersection(
        {1, 1, 1}, {-1, -1, 0},
        {0, 0, 0}, {1, 2, 1}) == 4.f/3.f);

    REQUIRE(ray_plane_intersection(
        {0, 0, 0}, {0, 1, 0},
        {1, 0, 0}, {1, 0, 0}) == -1);

    REQUIRE(ray_plane_intersection(
        {0, 0, 0}, {0, 0, 1},
        {1, 0, 0}, {1, 0, 0}) == -1);
}


TEST_CASE("Ray triangle intersection", "[intersection]") {
    REQUIRE(ray_triangle_intersection(
        {0, 0, 0}, {1, 1, 1},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1}) == 1.f/3.f);

    REQUIRE(ray_triangle_intersection(
        {0, 0, 0}, {1, 0, 0},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1}) == 1);

    REQUIRE(
        std::abs(
            ray_triangle_intersection(
                {0.2, 0.2, 0.2}, {1, 0, 0},
                {1, 0, 0}, {0, 1, 0}, {0, 0, 1})
            - 0.4f)
        < std::numeric_limits<float>::epsilon());

    REQUIRE(ray_triangle_intersection(
        {0, 0, 0}, {0, 1, 0},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1}) == 1);

    REQUIRE(ray_triangle_intersection(
        {0, 0, 0}, {0, 0, 1},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1}) == 1);

    REQUIRE(ray_triangle_intersection(
        {0, 0, 0}, {-1, -1, -1},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1}) == -1);

    REQUIRE(ray_triangle_intersection(
        {0, 0, 0}, {1, 1, 0},
        {1, 0, 0}, {0, 1, 0}, {0, 0, 1}) == 0.5f);
}

TEST_CASE("Ray AABB intersection", "[intersection]") {
    using Box = Vector3D[2];

    REQUIRE(ray_box_intersection(
        {0, 0, 0}, {-1, 1, 1},
        Box{{-1, -1, 1}, {1, 1, 1}}));

    REQUIRE(ray_box_intersection(
        {0, 0, 0}, {-1, 0, 0},
        Box{{-1, -1, 1}, {1, 1, 1}}));

    REQUIRE(!ray_box_intersection(
        {-2, -2, -2}, {1, 0, 0},
        Box{{-1, -1, 1}, {1, 1, 1}}));

    REQUIRE(!ray_box_intersection(
        {-1, 0, 0}, {1, 0, 0},
        Box{{0, 0, 0}, {1, 1, 1}}));
}
