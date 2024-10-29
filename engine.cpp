#include "engine.h"

#include "graph.h"

void Engine::Add(action::Bomb bomb) {
    const auto distance = graph_->Distance(bomb.from, bomb.to);
    if (distance == Graph::NONE) {
        throw std::runtime_error(std::format("no such edge {}->{}", bomb.from, bomb.to));
    }
    const int id = nextId_++;
    auto& factory = factories_.at(bomb.from);
    bombs_.push_back({id, factory.whose, bomb.from, bomb.to, distance});
    if (factory.whose == Whose::Mine) {
        ASSERT(mineBombs_ > 0);
        --mineBombs_;
    } else {
        ASSERT(opponentBombs_ > 0);
        --opponentBombs_;
    }
}

void Engine::Add(action::Move move) {
    const auto distance = graph_->Distance(move.from, move.to);
    if (distance == Graph::NONE) {
        throw std::runtime_error(std::format("no such edge {}->{}", move.from, move.to));
    }
    const int id = nextId_++;
    auto& factory = factories_.at(move.from);
    troops_.push_back({id, factory.whose, move.from, move.to, move.cyborgs, distance});
    ASSERT(factory.cyborgs >= move.cyborgs);
    factory.cyborgs -= move.cyborgs;
}

void Engine::AddFactory(Whose whose, int production, int cyborgs) {
    int id = nextId_++;
    factoryExplosionTurns_.push_back(0);
    factoryPos_.emplace_back(100 + factories_.size() * 200, 100 + factories_.size() * 200);
    factories_.push_back({
        .id = id,
        .whose = whose,
        .production = production,
        .cyborgs = cyborgs,
    });
}

Engine::State Engine::GetState() const {
    return {bombs_, factories_, factoryPos_, troops_, *graph_};
}

Whose Engine::HasWinner() const {
    /*
     * Your opponent has no cyborgs left, nor any factories capable of producing new cyborgs.
     * You have more cyborgs than your opponent after 200 turns.
     */
    int mineCyborgs = 0;
    int opponentCyborgs = 0;
    for (const auto& troop : troops_) {
        if (troop.whose == Whose::Mine) {
            mineCyborgs += troop.count;
        } else {
            opponentCyborgs += troop.count;
        }
    }
    bool haveFactories = false;
    bool hasFactories = false;
    for (const auto& factory : factories_) {
        if (factory.whose == Whose::Mine) {
            haveFactories |= factory.production > 0;
            mineCyborgs += factory.cyborgs;
        } else if (factory.whose == Whose::Opponent) {
            hasFactories |= factory.production > 0;
            opponentCyborgs += factory.cyborgs;
        }
    }
    const auto haveMore = turn_ >= 200 && mineCyborgs > opponentCyborgs;
    const auto hasMore = turn_ >= 200 && mineCyborgs < opponentCyborgs;
    if (mineCyborgs == 0 && !haveFactories || hasMore) {
        return Whose::Opponent;
    }
    if (opponentCyborgs == 0 && !hasFactories || haveMore) {
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

void Engine::SetGraph(std::unique_ptr<Graph> graph) {
    graph_ = std::move(graph);
}

void Engine::Update() {
    /*
     * Each turn, every non-neutral factory produces between 0 and 3 cyborgs.
     */
    for (auto& factory : factories_) {
        if (factoryExplosionTurns_[factory.id] > 0) {
            --factoryExplosionTurns_[factory.id];
        } else if (factory.whose == Whose::Mine || factory.whose == Whose::Opponent) {
            factory.cyborgs += factory.production;
        }
    }

    /*
     * Cyborgs that reach the same destination on the same turn fight between themselves.
     * Remaining cyborgs fight against the ones already present in the factory
     * (beware that the cyborgs currently leaving do not fight).
     */
    thread_local std::vector<int> troopsByTarget(factories_.size());
    std::fill(troopsByTarget.begin(), troopsByTarget.end(), 0);
    for (std::size_t i = 0; i < troops_.size(); ) {
        auto& troop = troops_[i];
        ASSERT(troop.eta >= 0);
        if (troop.eta == 0) {
            if (troop.whose == Whose::Mine) {
                troopsByTarget.at(troop.to) += troop.count;
            } else {
                troopsByTarget.at(troop.to) -= troop.count;
            }
            std::swap(troop, troops_.back());
            troops_.pop_back();
        } else {
            ++i;
        }
    }
    for (std::size_t id = 0; id < factories_.size(); ++id) {
        auto& factory = factories_.at(id);
        const auto left = troopsByTarget.at(id);
        if (left > 0) {
            if (factory.whose != Whose::Mine) {
                factory.cyborgs -= left;
                if (factory.cyborgs < 0) {
                    factory.cyborgs = -factory.cyborgs;
                    factory.whose = Whose::Mine;
                }
            } else {
                factory.cyborgs += left;
            }
        } else if (left < 0) {
            if (factory.whose != Whose::Opponent) {
                factory.cyborgs += left;
                if (factory.cyborgs < 0) {
                    factory.cyborgs = -factory.cyborgs;
                    factory.whose = Whose::Opponent;
                }
            } else {
                factory.cyborgs -= left;
            }
        }
    }

    for (std::size_t i = 0; i < bombs_.size(); ) {
        auto& bomb = bombs_[i];
        ASSERT(bomb.eta >= 0);
        if (bomb.eta == 0) {
            auto& to = factories_.at(bomb.to);
            to.cyborgs -= std::max(10, static_cast<int>(factories_.size()) / 2);
            to.cyborgs = std::max(to.cyborgs, 0);
            factoryExplosionTurns_[to.id] = 5;
            std::swap(bombs_.back(), bomb);
            bombs_.pop_back();
        } else {
            ++i;
        }
    }

    ++turn_;
}
