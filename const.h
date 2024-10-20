#pragma once

#include <chrono>
#include <stdexcept>

using namespace std::chrono_literals;

using Pos = std::pair<float, float>;

#define ASSERT(expr) do { \
    if (!(expr)) throw std::runtime_error(#expr); \
} while (false)

constexpr inline float FACTORY_RADIUS = 100;
constexpr inline float BOMB_RADIUS = FACTORY_RADIUS / 3;
constexpr inline float TROOP_RADIUS = FACTORY_RADIUS / 4;
