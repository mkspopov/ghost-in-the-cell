#pragma once

#include "engine.h"

class Player {
public:
    explicit Player(Whose who) : who_(who) {
    }

    template <class TStrategy>
    std::vector<action::Action> Play(const Engine::State& engine, TStrategy&& strategy) {
        /*
         * Be careful, your radar is able to detect the launch of a bomb but you don't know where its target is!
         *
         * It is impossible to send a bomb and a troop at the same time from the same factory and to the same destination.
         * If you try to do so, only the bomb will be sent.
         */
        return strategy(engine, *this);
    }

    int bombs_ = 2;
    const Whose who_;
};

struct RandomStrategy {
    std::vector<action::Action> operator()(const Engine::State& engine, Player& player) const {
        action::Move move = {-1, -1, 0};
        for (const auto& factory : engine.factories) {
            if (factory.whose == player.who_) {
                if (move.count < factory.cyborgs) {
                    move.from = factory.id;
                    move.count = factory.cyborgs / 2;
                }
            } else {
                move.to = factory.id;
            }
        }
        if (move.from != -1 && move.to != -1 && move.count > 0) {
            return {move};
        }
        return {};
    }
};
