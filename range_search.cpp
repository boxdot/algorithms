#include "tools/catch.hpp"
#include "lib/tree.h"
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


TEST_CASE("Find split node in balanced bin tree", "[range search]") {
    REQUIRE(find_split_node(test_tree(), 18, 77).root() == 49);
}

TEST_CASE("Query for a 1-dim range", "[range search]") {
    REQUIRE(
        oned_range_query(test_tree(), 18, 77) ==
        (std::vector<int>{19, 23, 30, 37, 49, 59, 62, 70}));
}
