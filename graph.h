#pragma once

#include <unordered_map>
#include <vector>

class Graph {
public:
    using Weight = int;

    explicit Graph(int numVertices) : adjList_(numVertices) {}

    void AddEdge(int from, int to, int weight) {
        adjList_[from].try_emplace(to, weight);
    }

    int Eta(int from, int to) const {
        return adjList_.at(from).at(to);
    }

    std::vector<std::unordered_map<int, Weight>> adjList_;
};

//class Graph {
//public:
//    using Weight = int;
//
//    struct Edge {
//        int id;
//        int from;
//        int to;
//    };
//
//    Graph(int numVertices) : adjList_(numVertices) {}
//
//    void AddEdge(int from, int to, int weight) {
//        int id = edgeProperties_.size();
//        edgeProperties_.push_back(weight);
//        edges_.push_back({id, from, to});
//        adjList_[from].push_back(id);
//    }
//
//    int Eta(int from, int to) const {
//        for (const auto neighbor : adjList_.at(from)) {
//            if (to == edges_.at(neighbor).to) {
//                return edgeProperties_.at(neighbor);
//            }
//        }
//        throw std::runtime_error("no such edge");
//    }
//
//    auto GetOutgoingEdges(int from) const {
//        return adjList_[from];
//    }
//
//    int GetTarget(int edgeId) const {
//        return edges_[edgeId].to;
//    }
//
//    int GetSource(int edgeId) const {
//        return edges_[edgeId].from;
//    }
//
//    int GetWeight(int edgeId) const {
//        return edgeProperties_[edgeId];
//    }
//
//    std::vector<std::vector<int>> adjList_;
//    std::vector<Weight> edgeProperties_;
//    std::vector<Edge> edges_;
//};
