#include "tools/catch.hpp"
#include "lib/tree.h"
#include "intersection.h"
#include <deque>
#include <vector>

//
// 1-Dimensional Range Searching
//

template<typename T>
Tree<T> find_split_node(Tree<T> t, T x, T y) {
    assert(x <= y);
    while (!t.is_leaf() && (!(y > t.root()) && t.root() < x)) {
        if (!(y > t.root())) {
            t = t.left();
        } else {
            t = t.right();
        }
    }
    return t;
}

template<typename T>
void prepend_subtree(std::deque<T>& res, const Tree<T> t) {
    if (t.is_leaf()) {
        res.push_front(t.root());
        return;
    }
    prepend_subtree(res, t.right());
    prepend_subtree(res, t.left());
}

template<typename T>
void append_subtree(std::deque<T>& res, const Tree<T> t) {
    if (t.is_leaf()) {
        res.push_back(t.root());
        return;
    }
    append_subtree(res, t.left());
    append_subtree(res, t.right());
}

//
// Query points in interval [x, y] in the balanced binary search tree t.
//
// The leaves of the tree t store the points and the internal nodes are
// the splitting values.
//
// Complexity: O(k + log n), where k is the number of found points and n the
// overall number of points.
//
// t - balanced binary search tree
// x, y - search interval [x, y]
//
template<typename T>
std::vector<T> oned_range_query(Tree<T> t, T x, T y) {
    auto t_split = find_split_node(t, x, y);
    if (t_split.is_leaf()) {
        if (x <= t.root() && t.root() <= y) {
            return {t.root()};
        }
        return {};
    }

    std::deque<T> res;

    t = t_split.left();
    while (!t.is_leaf()) {
        if (x <= t.root()) {
            prepend_subtree(res, t.right());
            t = t.left();
        } else {
            t = t.right();
        }
    }
    if (x <= t.root()) {
        res.push_front(t.root());
    }

    t = t_split.right();
    while (!t.is_leaf()) {
        if (t.root() <= y) {
            append_subtree(res, t.left());
            t = t.right();
        } else {
            t = t.left();
        }
    }
    if (t.root() <= y) {
        res.push_back(t.root());
    }

    return std::vector<T>(res.begin(), res.end());
}

//
// kd-Tree (k = 2)
//

struct Point2D {
    float x, y;
    bool operator==(const Point2D& pt) const {
        return x == pt.x && y == pt.y;
    }
};

std::ostream& operator<<(std::ostream& os, const Point2D& pt) {
    return os << "(" << pt.x << ", " << pt.y << ")";
}

//
// Internal nodes are splitting points. If the depth is even, then the y
// coordinate of the splitting point is 0 and the x coordinates defines a right
// half plane s.t. all points in the right subtree lie on this plane. (Note
// that the line defining the half plane also belongs to it.) Similarly, if the
// depth is odd, then the x coordinate of the splitting point is zero, and the
// y coordinate defines a lower half plane s.t. all points in the right subtree
// lie on this plane. The leaves are points.
//
class KDTree : public Tree<Point2D> {
    KDTree() : Tree() {}
    KDTree(Point2D value) : Tree(value) {}
    KDTree(Point2D value, const KDTree& left, const KDTree& right)
        : Tree(value, left, right) {}

public:
    // Construct a kd-tree from a set of points.
    static KDTree build(std::initializer_list<Point2D> points) {
        std::vector<Point2D> points_x(points);
        std::vector<Point2D> points_y(points);
        std::sort(points_x.begin(), points_x.end(),
            [](const auto& p, const auto& q) {
                return p.x < q.x;
            });
        std::sort(points_y.begin(), points_y.end(),
            [](const auto& p, const auto& q) {
                return p.y < q.y;
            });

        return build_(points_x, points_y, 0);
    }

private:
    // The set of points is described by two sets: points_x and points_y. The
    // former consists of all points sorted by x coordinate and the latter by y
    // coordinate. This allows to efficiently compute the median along x resp.
    // y axis.
    static KDTree build_(
        std::vector<Point2D> points_x,
        std::vector<Point2D> points_y,
        size_t depth)
    {
        assert(points_x.size() == points_y.size());

        if (points_x.empty()) {
            return KDTree();
        }

        if (points_x.size() == 1) {
            return KDTree(points_x[0]);
        }

        Point2D split_pt{0, 0};
        std::vector<Point2D> left_x, left_y, right_x, right_y;

        size_t median_idx = points_x.size() / 2;
        if (depth % 2 == 0) {
            split_pt.x = points_x[median_idx].x;
            left_x = std::vector<Point2D>(
                points_x.begin(), points_x.begin() + median_idx);
            right_x = std::vector<Point2D>(
                points_x.begin() + median_idx, points_x.end());
            for (const auto& pt : points_y) {
                if (pt.x < split_pt.x) {
                    left_y.push_back(pt);
                } else {
                    right_y.push_back(pt);
                }
            }
        } else {
            split_pt.y = points_y[median_idx].y;
            left_y = std::vector<Point2D>(
                points_y.begin(), points_y.begin() + median_idx);
            right_y = std::vector<Point2D>(
                points_y.begin() + median_idx, points_y.end());
            for (const auto& pt : points_x) {
                if (pt.y < split_pt.y) {
                    left_x.push_back(pt);
                } else {
                    right_x.push_back(pt);
                }
            }
        }

        KDTree left = build_(left_x, left_y, depth + 1);
        KDTree right = build_(right_x, right_y, depth + 1);
        return KDTree(split_pt, left, right);
    }
};

//
// Kd-Tree (k = 3)
//

struct KDTree3Node {
    Vector3D bbox[2];
    std::vector<size_t> v_indices;
};

class KDTree3 : public Tree<KDTree3Node> {
public:
    // Construct a kd-tree from a set of points.
    static KDTree3 build(std::initializer_list<Vector3D> points) {
        if (points.size() == 0) {
            return KDTree3();
        }

        if (points.size() == 1) {
            return KDTree3();
        }

        return KDTree3();
    }
};


//
// Tests
//

using T = Tree<int>;
T test_tree() {
    return T(49,
        T(23,
            T(10,
                T(3, T(3), T(10)),
                T(19, T(19), T(23))),
            T(37,
                T(30, T(30), T(37)),
                T(49))),
        T(80,
            T(62,
                T(59, T(59), T(62)),
                T(70, T(70), T(80))),
            T(89,
                T(89),
                T(100, T(100), T(105)))));
}

//
// kd-Tree (k = 2)
//



TEST_CASE("Find split node in balanced bin tree", "[range search]") {
    REQUIRE(find_split_node(test_tree(), 18, 77).root() == 49);
}

TEST_CASE("Query for a 1-dim range", "[range search]") {
    REQUIRE(
        oned_range_query(test_tree(), 18, 77) ==
        (std::vector<int>{19, 23, 30, 37, 49, 59, 62, 70}));
}

TEST_CASE("Build a simple KD-Tree", "[kd-tree]") {
    auto t = KDTree::build({
        {0, 0}, {1, 1}, {-1, 2}
    });

    // Result:
    //          0
    // (-1 2)         1
    //          (0, 0) (1, 1)
    REQUIRE(t.root() == (Point2D{0, 0}));
    REQUIRE(t.left().root() == (Point2D{-1, 2}));         // leaf
    REQUIRE(t.right().root() == (Point2D{0, 1}));
    REQUIRE(t.right().left().root() == (Point2D{0, 0}));  // leaf
    REQUIRE(t.right().right().root() == (Point2D{1, 1})); // leaf
}
