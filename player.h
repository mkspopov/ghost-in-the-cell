#pragma once

#include "engine.h"
#include <utility>

template <class TStrategy>
class Player {
public:
    Player(Whose who, TStrategy strategy) : strategy_(strategy), who_(who) {
    }

    void Play(Engine& engine) {
        /*
         * Be careful, your radar is able to detect the launch of a bomb but you don't know where its target is!
         *
         * It is impossible to send a bomb and a troop at the same time from the same factory and to the same destination.
         * If you try to do so, only the bomb will be sent.
         */
        const auto actions = strategy_.MakeDecision(engine.GetState(), *this);
        for (const auto& action : actions) {
            if (auto ptr = std::get_if<action::Bomb>(&action)) {
                engine.Add(*ptr);
            } else if (auto ptr = std::get_if<action::Inc>(&action)) {
                /*
                * At any moment, you can decide to sacrifice 10 cyborgs in a factory to indefinitely increase its production by one cyborg per turn.
                * A factory will not be able to produce more than 3 cyborgs per turn.
                */
                throw std::runtime_error("not implemented");
            } else if (auto ptr = std::get_if<action::Move>(&action)) {
                engine.Add(*ptr);
            } else if (auto ptr = std::get_if<action::Msg>(&action)) {
                // std::cerr << ptr->msg << std::endl;
            } else if (auto ptr = std::get_if<action::Wait>(&action)) {
            } else {
                throw std::runtime_error("unknown action");
            }
        }
    }

    Whose Who() const {
        return who_;
    }

private:
    TStrategy strategy_;
    int bombs_ = 2;
    const Whose who_;
};
