#include <catch.hpp>

#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include <iostream>


using label_t = std::string;
using path_t = std::vector<label_t>;

using edge_t = std::pair<label_t, label_t>;
using edges_t = std::vector<edge_t>;
using graph_t = std::map<label_t, std::vector<label_t>>;

using weight_t = size_t;
using weighted_edge_t = std::tuple<label_t, label_t, weight_t>;
using weighted_edges_t = std::vector<weighted_edge_t>;
using weighted_graph_t = std::map<
    label_t, std::vector<std::tuple<label_t, weight_t>>>;


auto graph(const edges_t& edges) {
    graph_t g;
    for (const auto& e : edges) {
        label_t from, to;
        std::tie(from, to) = e;
        g[from].push_back(to);
    }
    return g;
}


auto graph(const weighted_edges_t& edges) {
    weighted_graph_t g;
    for (const auto& e : edges) {
        label_t from, to;
        weight_t weight;
        std::tie(from, to, weight) = e;
        g[from].push_back(std::make_tuple(to, weight));
    }
    return g;
}


template<typename Visitor>
void bfs(const graph_t& g, const label_t& source, Visitor visit) {
    std::queue<label_t> queue{{source}};
    std::set<label_t> seen{{source}};
    while (!queue.empty()) {
        auto v = queue.front();
        queue.pop();
        visit(v);

        auto vit = g.find(v);
        if (vit != g.end()) {
            for (auto w : vit->second) {
                if (!seen.count(w)) {
                    queue.push(w);
                    seen.insert(w);
                }
            }
        }
    }
}


template<typename Visitor>
void dfs(const graph_t& g, const label_t& source, Visitor visit) {
    std::stack<label_t> stack{{source}};
    std::set<label_t> seen{{source}};
    while (!stack.empty()) {
        auto v = stack.top();
        stack.pop();
        visit(v);

        auto vit = g.find(v);
        if (vit != g.end()) {
            for (auto w : vit->second) {
                if (!seen.count(w)) {
                    stack.push(w);
                    seen.insert(w);
                }
            }
        }
    }
}



std::tuple<weight_t, path_t> dijkstra(
    const weighted_graph_t& g, const label_t& source, const label_t& target)
{
    // priority queue = {(0, source, empty path)} and seen set
    using queue_element_t = std::tuple<weight_t, label_t, path_t>;
    struct queue_compare {
        bool operator()(const queue_element_t& a, const queue_element_t& b) {
            return std::get<0>(a) >= std::get<0>(b);
        }
    };
    std::priority_queue<
        queue_element_t, std::vector<queue_element_t>, queue_compare> queue;
    queue.push(std::make_tuple(0, source, path_t{}));
    std::set<label_t> seen;

    // walk
    while (!queue.empty()) {
        weight_t cost; label_t v; path_t path;
        std::tie(cost, v, path) = queue.top();
        queue.pop();

        if (!seen.count(v)) {
            auto new_path = path;
            new_path.push_back(v);
            if (v == target) {
                return std::make_tuple(cost, new_path);
            }
            seen.insert(v);

            if (g.count(v)) {
                for (const auto& edge : g.at(v)) {
                    label_t w;
                    weight_t cost_w;
                    std::tie(w, cost_w) = edge;
                    if (!seen.count(w)) {
                        queue.push(
                            std::make_tuple(cost + cost_w, w, new_path));
                    }
                }
            }
        }
    }
    return std::make_tuple(0, path_t{});
}


TEST_CASE("Test bfs", "[bfs]") {
    auto g = graph(edges_t{{
        {"A", "B"},
        {"A", "D"},
        {"B", "C"},
        {"B", "D"},
        {"B", "E"},
        {"C", "E"},
        {"D", "E"},
        {"D", "F"},
        {"E", "F"},
        {"E", "G"},
        {"F", "G"},
        {"A", "G"},
    }});

    path_t path;
    bfs(g, "A", [&path](const label_t& v) {
        path.push_back(v);
    });
    REQUIRE(path == (path_t{"A", "B", "D", "G", "C", "E", "F"}));
}


TEST_CASE("Test dfs", "[dfs]") {
    auto g = graph(edges_t{{
        {"A", "B"},
        {"A", "D"},
        {"B", "C"},
        {"B", "D"},
        {"B", "E"},
        {"C", "E"},
        {"D", "E"},
        {"D", "F"},
        {"E", "F"},
        {"E", "G"},
        {"F", "G"},
        {"A", "G"},
    }});

    path_t path;
    dfs(g, "A", [&path](const label_t& v) {
        path.push_back(v);
    });
    REQUIRE(path == (path_t{"A", "G", "D", "F", "E", "B", "C"}));
}


TEST_CASE("Test dijsktra", "[dijsktra]") {
    auto g = graph(weighted_edges_t{{
        std::make_tuple("A", "B", 7),
        std::make_tuple("A", "D", 5),
        std::make_tuple("B", "C", 8),
        std::make_tuple("B", "D", 9),
        std::make_tuple("B", "E", 7),
        std::make_tuple("C", "E", 5),
        std::make_tuple("D", "E", 15),
        std::make_tuple("D", "F", 6),
        std::make_tuple("E", "F", 8),
        std::make_tuple("E", "G", 9),
        std::make_tuple("F", "G", 11),
        std::make_tuple("A", "G", 23),
    }});

    weight_t w; path_t path;

    std::tie(w, path) = dijkstra(g, "A", "E");
    REQUIRE(w == 14);
    REQUIRE(path == (path_t{"A", "B", "E"}));

    std::tie(w, path) = dijkstra(g, "A", "G");
    REQUIRE(w == 22);
    REQUIRE(path == (path_t{"A", "D", "F", "G"}));

    std::tie(w, path) = dijkstra(g, "B", "A");
    REQUIRE(w == 0);
    REQUIRE(path == (path_t{}));
}
