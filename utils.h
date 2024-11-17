#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <random>

class Engine;
class Graph;

std::unique_ptr<Graph> GenerateCompleteGraph(Engine& engine);

std::unique_ptr<Graph> GenerateRandomGraph(Engine& engine);

std::mt19937& GetRng();

float Dot(sf::Vector2f lhs, sf::Vector2f rhs);

float Sq(sf::Vector2f v);

sf::Vector2f Center(const sf::CircleShape& shape);

bool In(sf::Vector2f point, const sf::CircleShape& shape);
