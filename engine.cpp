#include "engine.h"

void Engine::Add(action::Bomb bomb, Whose whose) {
    const int id = nextId_++;
    pos_.try_emplace(id, pos_.at(bomb.from));
    bombs_.push_back({id, whose, bomb.from, bomb.to, Distance(bomb.from, bomb.to)});
}

void Engine::Add(action::Move move, Whose whose) {
    const int id = nextId_++;
    pos_.try_emplace(id, pos_.at(move.from));
    troops_.push_back({id, whose, move.from, move.to, move.count, Distance(move.from, move.to)});
}

void Engine::AddFactory(Whose whose, int production) {
    int id = nextId_++;
    pos_.try_emplace(id, 100 + factories_.size() * 200, 100 + factories_.size() * 200);
    factories_.push_back({
        .id = id,
        .whose = whose,
        .production = production,
        .cyborgs = 0,
    });
}

int Engine::Distance(int from, int to) const {
    return graph_->Eta(from, to);
}

Engine::State Engine::GetState() const {
    return {bombs_, factories_, troops_, pos_, *graph_};
}

Whose Engine::HasWinner() const {
    /*
     * Your opponent has no cyborgs left, nor any factories capable of producing new cyborgs.
     * You have more cyborgs than your opponent after 200 turns.
     */
    int mineCyborgs = 0;
    int otherCyborgs = 0;
    for (const auto& troop : troops_) {
        if (troop.whose == Whose::Mine) {
            mineCyborgs += troop.count;
        } else {
            otherCyborgs += troop.count;
        }
    }
    bool haveFactories = false;
    bool hasFactories = false;
    for (const auto& factory : factories_) {
        if (factory.whose == Whose::Mine) {
            haveFactories = factory.production > 0;
            mineCyborgs += factory.cyborgs;
        } else if (factory.whose == Whose::Opponent) {
            hasFactories = factory.production > 0;
            otherCyborgs += factory.cyborgs;
        }
    }
    const auto haveMore = turn_ >= 200 && mineCyborgs > otherCyborgs;
    const auto hasMore = turn_ >= 200 && mineCyborgs < otherCyborgs;
    if (mineCyborgs == 0 && !haveFactories || hasMore) {
        return Whose::Opponent;
    } else if (otherCyborgs == 0 && !hasFactories || haveMore) {
        return Whose::Mine;
    }
    return Whose::Neutral;
}

void Engine::Move() {
    for (auto& bomb : bombs_) {
        --bomb.eta;
    }
    for (auto& troop : troops_) {
        --troop.eta;
    }
}

void Engine::Update() {
    /*
     * Each turn, every non-neutral factory produces between 0 and 3 cyborgs.
     */
    for (auto& factory : factories_) {
        if (factory.explosion_ > 0) {
            --factory.explosion_;
        } else if (factory.whose == Whose::Mine || factory.whose == Whose::Opponent) {
            factory.cyborgs += factory.production;
        }
    }

    /*
     * Cyborgs that reach the same destination on the same turn fight between themselves.
     * Remaining cyborgs fight against the ones already present in the factory
     * (beware that the cyborgs currently leaving do not fight).
     */
    std::unordered_map<int, std::pair<Troop, Troop>> troopsByTarget;
    for (std::size_t i = 0; i < troops_.size(); ) {
        auto& troop = troops_[i];
        ASSERT(troop.eta >= 0);
        if (troop.eta == 0) {
            auto& [mine, opponent] = troopsByTarget.try_emplace(
                troop.to,
                Troop{-1, Whose::Mine, -1, troop.to, 0, 0},
                Troop{-1, Whose::Opponent, -1, troop.to, 0, 0}).first->second;
            if (troop.whose == Whose::Mine) {
                mine.count += troop.count;
            } else {
                opponent.count += troop.count;
            }
            std::swap(troop, troops_.back());
            troops_.pop_back();
        } else {
            ++i;
        }
    }
    for (const auto& [id, troops] : troopsByTarget) {
        auto& factory = factories_.at(id);
        const auto left = troops.first.count - troops.second.count;
        if (factory.whose == Whose::Mine) {
            factory.cyborgs += left;
            if (factory.cyborgs < 0) {
                factory.cyborgs = -factory.cyborgs;
                factory.whose = Whose::Opponent;
            }
        } else if (factory.whose == Whose::Opponent) {
            factory.cyborgs -= left;
            if (factory.cyborgs < 0) {
                factory.cyborgs = -factory.cyborgs;
                factory.whose = Whose::Mine;
            }
        } else if (left > 0) {
            factory.cyborgs = left;
            factory.whose = Whose::Mine;
        } else if (left < 0) {
            factory.cyborgs = -left;
            factory.whose = Whose::Opponent;
        }
    }

    for (std::size_t i = 0; i < bombs_.size(); ) {
        auto& bomb = bombs_[i];
        ASSERT(bomb.eta >= 0);
        if (bomb.eta == 0) {
            auto& to = factories_.at(bomb.to);
            to.cyborgs -= std::max(10, static_cast<int>(factories_.size()) / 2);
            to.cyborgs = std::max(to.cyborgs, 0);
            to.explosion_ = 5;
            std::swap(bombs_.back(), bomb);
            bombs_.pop_back();
        } else {
            ++i;
        }
    }

    ++turn_;
}
