#pragma once

#include <memory>
#include <random>

class Engine;
class Graph;

std::unique_ptr<Graph> GenerateCompleteGraph(Engine& engine);

std::unique_ptr<Graph> GenerateRandomGraph(Engine& engine);

std::mt19937& GetRng();
