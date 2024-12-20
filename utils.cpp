#include "utils.h"

#include "engine.h"
#include "graph.h"
#include <random>

std::unique_ptr<Graph> GenerateCompleteGraph(Engine& engine) {
    /*
     * Is the graph always complete?
     */
    const auto factories = engine.GetState().factories;
    auto graph = std::make_unique<Graph>(factories.size());
    for (std::size_t i = 0; i < factories.size(); ++i) {
        for (std::size_t j = i + 1; j < factories.size(); ++j) {
            graph->AddEdge(i, j, j - i);
            graph->AddEdge(j, i, j - i);
        }
    }
    return graph;
}

std::unique_ptr<Graph> GenerateRandomGraph(Engine& engine) {
    /*
     * Is the graph always undirected?
     */
    const auto factories = engine.GetState().factories;
    auto graph = std::make_unique<Graph>(factories.size());
    for (std::size_t i = 0; i < factories.size(); ++i) {
        for (std::size_t j = i + 1; j < factories.size(); ++j) {
            std::uniform_real_distribution willAdd;
            std::uniform_int_distribution weight(1, 10);
            if (willAdd(GetRng()) < 0.9) {
                graph->AddEdge(i, j, weight(GetRng()));
                graph->AddEdge(j, i, weight(GetRng()));
            }
        }
    }
    return graph;
}

std::mt19937& GetRng() {
    thread_local std::mt19937 gen;
    return gen;
}

float Dot(sf::Vector2f lhs, sf::Vector2f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

float Sq(sf::Vector2f v) {
    return Dot(v, v);
}

sf::Vector2f Center(const sf::CircleShape& shape) {
    return shape.getPosition() + sf::Vector2f(shape.getRadius() / 2, shape.getRadius() / 2);
}

bool In(sf::Vector2f point, const sf::CircleShape& shape) {
    return Sq(Center(shape) - point) <= shape.getRadius() * shape.getRadius();
}
