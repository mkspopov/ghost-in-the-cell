#pragma once

#include "engine.h"
#include "player.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>
#include <thread>

const inline auto GREY = sf::Color(0x333333ff);
const inline auto PEACH = sf::Color(0xffd7beff);

void DrawText(sf::RenderWindow& window, sf::Vector2f pos, const std::string& str);

class Game {
public:
    void Init(sf::RenderWindow& window);

    void HandleInput(sf::Event&);

    void Render(float dt);

    void Update(Player& player);

    double Update();

    sf::RenderWindow* window_;
    Engine engine_;
    Player mine_{Whose::Mine};
    Player opponent_{Whose::Opponent};
};
