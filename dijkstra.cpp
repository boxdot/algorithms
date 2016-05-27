#include <catch.hpp>

#include <deque>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>


using label_t = std::string;
using weight_t = size_t;
using edge_t = std::tuple<label_t, label_t, weight_t>;
using edges_t = std::vector<edge_t>;
using graph_t = std::map<label_t, std::vector<std::tuple<label_t, weight_t>>>;
using path_t = std::vector<label_t>;


graph_t graph(const edges_t& edges) {
    graph_t g;
    for (const auto& e : edges) {
        label_t from, to;
        weight_t weight;
        std::tie(from, to, weight) = e;
        g[from].push_back(std::make_tuple(to, weight));
    }
    return g;
}


std::tuple<weight_t, path_t> dijkstra(
    const edges_t& edges, const label_t& source, const label_t& target)
{
    auto g = graph(edges);

    using queue_element_t = std::tuple<weight_t, label_t, path_t>;
    std::vector<queue_element_t> queue{std::make_tuple(0, source, path_t{})};
    auto queue_comp = [](const queue_element_t& a, const queue_element_t& b) {
        return std::get<0>(a) >= std::get<0>(b);
    };
    std::set<label_t> seen;

    while (!queue.empty()) {
        weight_t cost; label_t v; path_t path;
        std::tie(cost, v, path) = queue.front();

        std::pop_heap(queue.begin(), queue.end(), queue_comp);
        queue.pop_back();

        if (!seen.count(v)) {
            auto new_path = path;
            new_path.push_back(v);
            if (v == target) {
                return std::make_tuple(cost, new_path);
            }
            seen.insert(v);

            if (g.count(v)) {
                for (const auto& edge : g[v]) {
                    label_t w;
                    weight_t cost_w;
                    std::tie(w, cost_w) = edge;
                    if (!seen.count(w)) {
                        queue.push_back(
                            std::make_tuple(cost + cost_w, w, new_path));
                        std::push_heap(queue.begin(), queue.end(), queue_comp);
                    }
                }
            }
        }
    }
    return std::make_tuple(0, path_t{});
}


TEST_CASE("Test dijsktra", "[dijsktra]") {
    edges_t edges = {
        {"A", "B", 7},
        {"A", "D", 5},
        {"B", "C", 8},
        {"B", "D", 9},
        {"B", "E", 7},
        {"C", "E", 5},
        {"D", "E", 15},
        {"D", "F", 6},
        {"E", "F", 8},
        {"E", "G", 9},
        {"F", "G", 11},
        {"A", "G", 23}
    };

    weight_t w; path_t path;

    std::tie(w, path) = dijkstra(edges, "A", "E");
    REQUIRE(w == 14);
    REQUIRE(path == (path_t{"A", "B", "E"}));

    std::tie(w, path) = dijkstra(edges, "A", "G");
    REQUIRE(w == 22);
    REQUIRE(path == (path_t{"A", "D", "F", "G"}));

    std::tie(w, path) = dijkstra(edges, "B", "A");
    REQUIRE(w == 0);
    REQUIRE(path == (path_t{}));
}
