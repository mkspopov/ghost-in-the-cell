#include "game.h"

#include "graph.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

const inline auto FONT = []() {
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/tlwg/Purisa.ttf")) {
        throw std::runtime_error("cannot load Purisa font");
    }
    return font;
}();

template <class T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
    return os << v.x << ',' << v.y;
}

void DrawText(sf::RenderWindow& window, sf::Vector2f pos, const std::string& str) {
    static sf::Text text(str, FONT, 20);
    text.setString(str);
    text.setPosition(pos);
    text.setFillColor(sf::Color::Black);
    window.draw(text);
}

sf::Vector2f SfPos(std::pair<float, float> pos) {
    return {pos.first, pos.second};
}

Game::Game(Engine& engine) : engine_(engine) {
}

void Game::Init(sf::RenderWindow& window) {
    window_ = &window;
}

void Game::HandleInput(sf::Event&) {
}

const inline sf::Color MINE_FACTORY = sf::Color(0xde, 0xc0, 0x86);
const inline sf::Color MINE_TROOP = sf::Color(0xe4, 0xc3, 0xae);
const inline sf::Color MINE_BOMB = sf::Color(0xcc, 0xab, 0xa1);
const inline sf::Color OPPONENT_FACTORY = sf::Color(0xbf, 0xc8, 0xc9);
const inline sf::Color OPPONENT_TROOP = sf::Color(0xce, 0xd8, 0xd3);
const inline sf::Color OPPONENT_BOMB = sf::Color(0xa6, 0xbc, 0xb5);
const inline sf::Color NEUTRAL_FACTORY = sf::Color(0x70, 0x79, 0x83);

void Game::Render(float dt) {
    const auto& [bombs, factories, factoryPos, troops, graph] = engine_.GetState();
    for (const auto& factory : factories) {
        static auto shape = sf::CircleShape(FACTORY_RADIUS);
        shape.setPosition(SfPos(factoryPos.at(factory.id)));
        if (factory.whose == Whose::Mine) {
            shape.setFillColor(MINE_FACTORY);
        } else if (factory.whose == Whose::Opponent) {
            shape.setFillColor(OPPONENT_FACTORY);
        } else {
            shape.setFillColor(NEUTRAL_FACTORY);
        }
        window_->draw(shape);
        DrawText(
            *window_,
            SfPos(factoryPos.at(factory.id)) + sf::Vector2f{FACTORY_RADIUS / 3, FACTORY_RADIUS / 3},
            std::format("cyborgs: {}\nprod: {}\nid: {}", factory.cyborgs, factory.production, factory.id));
    }

    const auto setPos = [&](auto& shape, const auto& entity) {
        shape.setPosition(SfPos(factoryPos.at(entity.from))
            + (graph.Distance(entity.from, entity.to) - entity.eta + dt)
                * (SfPos(factoryPos.at(entity.to)) - SfPos(factoryPos.at(entity.from)))
                / static_cast<float>(graph.Distance(entity.from, entity.to)));
    };

    thread_local auto bomb = sf::CircleShape(BOMB_RADIUS);
    for (const auto& entity : bombs) {
        setPos(bomb, entity);
        if (entity.whose == Whose::Mine) {
            bomb.setFillColor(MINE_BOMB);
        } else {
            bomb.setFillColor(OPPONENT_BOMB);
        }
        window_->draw(bomb);
    }
    thread_local auto troop = sf::CircleShape(TROOP_RADIUS);
    for (const auto& entity : troops) {
        setPos(troop, entity);
        if (entity.whose == Whose::Mine) {
            troop.setFillColor(MINE_TROOP);
        } else {
            troop.setFillColor(OPPONENT_TROOP);
        }
        window_->draw(troop);
        DrawText(
            *window_,
            troop.getPosition() + sf::Vector2f{troop.getRadius() / 4, troop.getRadius() / 4},
            std::format("{}", entity.count));
    }
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
