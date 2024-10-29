#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

class Graph {
public:
    using Weight = int;
    static constexpr inline Weight NONE = -1;

    Graph(int numVertices) : adjList_(numVertices) {
    }

    void AddEdge(int from, int to, int weight) {
        adjList_[from].try_emplace(to, weight);
    }

    int Distance(int from, int to) const {
        return adjList_[from].contains(to) ? adjList_[from].at(to) : NONE;
    }

    const auto& Edges(int from) const {
        return adjList_[from];
    }

    int Vertices() const {
        return adjList_.size();
    }

private:
    std::vector<std::unordered_map<int, Weight>> adjList_;
};

//struct ShortestPath {
//    std::vector<int> path;
//    Graph::Weight weight;
//};
//
//auto Dijkstra(Graph& graph, int from) {
//    std::vector<int> parents(graph.Vertices(), Graph::NONE);
//    std::vector<int> distances(graph.Vertices(), Graph::NONE);
//    using Elem = std::pair<Graph::Weight, int>;
//    std::priority_queue<Elem, std::vector<Elem>, std::greater<>> heap;
//    heap.emplace(0, from);
//    while (!heap.empty()) {
//        auto [distance, cur] = heap.top();
//        heap.pop();
//        if (distance >= distances[cur]) {
//            continue;
//        }
//        for (auto [to, weight] : graph.Edges(cur)) {
//            if (distances[to] == Graph::NONE || distances[to] > distance + weight) {
//                distances[to] = distance + weight;
//                heap.emplace(distances[to], to);
//                parents[to] = cur;
//            }
//        }
//    }
//}
//
//class ExtendedGraph {
//public:
//    explicit ExtendedGraph(Graph& graph) : graph_(graph) {
//        for (int from = 0; from < graph_.Vertices(); ++from) {
//            auto [parents, distances] = Dijkstra(graph_, from);
//
//        }
//    }
//
//private:
//    Graph& graph_;
//    std::unordered_map<int, ShortestPath> paths_;
//};
