#pragma once

#include "action.h"
#include "const.h"
#include "entities.h"

#include <memory>
#include <unordered_map>

class Graph;

class Engine {
public:
    struct State {
        const std::vector<Bomb>& bombs;
        const std::vector<Factory>& factories;
        const std::vector<Pos>& factoryPos;
        const std::vector<Troop>& troops;
        const Graph& graph;
    };

    void Add(action::Bomb bomb, Whose whose);

    void Add(action::Move move, Whose whose);

    void AddFactory(Whose whose, int production);

    int Distance(int from, int to) const;

    State GetState() const;

    Whose HasWinner() const;

    void Move();

    void SetGraph(std::unique_ptr<Graph> graph);

    void Update();

private:
    std::vector<Bomb> bombs_;
    std::vector<Factory> factories_;
    std::vector<int> factoryExplosionTurns_;
    std::vector<Pos> factoryPos_;
    std::vector<Troop> troops_;
    std::unique_ptr<Graph> graph_;
    int mineBombs_ = 2;
    int opponentBombs_ = 2;
    int nextId_ = 0;
    int turn_ = 0;
};
