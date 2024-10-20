#include "game.h"

#include <SFML/Graphics/CircleShape.hpp>

sf::Vector2f SfPos(std::pair<float, float> pos) {
    return {pos.first, pos.second};
}

void Game::Init(sf::RenderWindow& window) {
    window_ = &window;
}

void Game::HandleInput(sf::Event&) {
}

void Game::Render(double) {
    const auto& pos = engine_.GetState().pos;
    const auto& factories = engine_.GetState().factories;
    for (const auto& factory : factories) {
        static auto shape = sf::CircleShape(FACTORY_RADIUS);
        shape.setPosition(SfPos(pos.at(factory.id)));
        shape.setFillColor(PEACH);
        window_->draw(shape);
    }
    const auto drawMoving = [this, &pos](const auto& entities, sf::Shape& shape) {
        const auto& graph = engine_.GetState().graph;
        for (const auto& entity : entities) {
            shape.setPosition(SfPos(pos.at(entity.id))
                              + static_cast<float>(entity.eta) / graph.Eta(entity.from, entity.to)
                                * (SfPos(pos.at(entity.to)) - SfPos(pos.at(entity.from))));
            shape.setFillColor(PEACH);
            window_->draw(shape);
        }
    };
    static auto troop = sf::CircleShape(TROOP_RADIUS);
    drawMoving(engine_.GetState().troops, troop);
    static auto bomb = sf::CircleShape(BOMB_RADIUS);
    drawMoving(engine_.GetState().bombs, bomb);
}

void Game::Update(Player& player) {
    const auto actions = player.Play(engine_.GetState(), RandomStrategy{});
    for (const auto& action : actions) {
        if (auto ptr = std::get_if<action::Bomb>(&action)) {
            engine_.Add(*ptr, player.who_);
        } else if (auto ptr = std::get_if<action::Inc>(&action)) {
            /*
             * At any moment, you can decide to sacrifice 10 cyborgs in a factory to indefinitely increase its production by one cyborg per turn.
             * A factory will not be able to produce more than 3 cyborgs per turn.
             */
            throw std::runtime_error("not implemented");
        } else if (auto ptr = std::get_if<action::Move>(&action)) {
            engine_.Add(*ptr, player.who_);
        } else if (auto ptr = std::get_if<action::Msg>(&action)) {
            std::cerr << ptr->msg << std::endl;
        } else if (auto ptr = std::get_if<action::Wait>(&action)) {
        } else {
            throw std::runtime_error("unknown action");
        }
    }
}

double Game::Update() {
    /*
     * Move existing troops and bombs
     * Execute user orders
     * Produce new cyborgs in all factories
     * Solve battles
     * Make the bombs explode
     * Check end conditions
     */
    engine_.Move();
    Update(mine_);
    Update(opponent_);
    engine_.Update();

    const auto winner = engine_.HasWinner();
    if (winner != Whose::Neutral) {
        if (winner == Whose::Mine) {
            std::cerr << "I win" << std::endl;
        } else {
            std::cerr << "opponent wins" << std::endl;
        }
        std::cerr << "Done." << std::endl;
        std::this_thread::sleep_for(10s);
        window_->close();
    }
    return 1;
}
