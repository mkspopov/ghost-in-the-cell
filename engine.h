#pragma once

#include "action.h"
#include "const.h"
#include "entities.h"
#include "graph.h"

#include <memory>
#include <unordered_map>

class Engine {
public:
    struct State {
        const std::vector<Bomb>& bombs;
        const std::vector<Factory>& factories;
        const std::vector<Troop>& troops;
        const std::unordered_map<int, Pos>& pos;
        const Graph& graph;
    };

    void Add(action::Bomb bomb, Whose whose);

    void Add(action::Move move, Whose whose);

    void AddFactory(Whose whose, int production);

    int Distance(int from, int to) const;

    State GetState() const;

    Whose HasWinner() const;

    void Move();

    void Update();

    std::vector<Bomb> bombs_;
    std::vector<Factory> factories_;
    std::vector<Troop> troops_;
    std::unordered_map<int, Pos> pos_;
    std::unique_ptr<Graph> graph_;
    int nextId_ = 0;
    int turn_ = 0;
};