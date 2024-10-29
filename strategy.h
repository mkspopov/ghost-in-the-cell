#pragma once

#include "action.h"
#include "engine.h"
#include "graph.h"
#include "player.h"
#include "utils.h"

class RandomStrategy {
public:
    std::vector<action::Action> MakeDecision(
        const Engine::State& engine,
        const Player<RandomStrategy>& player) const
    {
        std::vector<action::Move> moves;
        action::Move move = {-1, -1, 0};
        for (const auto& factory : engine.factories) {
            if (factory.whose != player.Who() || factory.cyborgs == 0) {
                continue;
            }
            for (auto [to, _] : engine.graph.Edges(factory.id)) {
                const auto& otherFactory = engine.factories.at(to);
                if (otherFactory.whose == player.Who()) {
                    continue;
                }
                std::uniform_int_distribution genCyborgs(1, factory.cyborgs);
                moves.push_back({
                    .from = factory.id,
                    .to = otherFactory.id,
                    .cyborgs = genCyborgs(GetRng()),
                });
            }
        }
        if (moves.empty()) {
            return {};
        }
        std::uniform_int_distribution genMove(0, static_cast<int>(moves.size()) - 1);
        return {moves[genMove(GetRng())]};
    }
};
