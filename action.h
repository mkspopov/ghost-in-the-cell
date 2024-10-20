#pragma once

#include <string>
#include <variant>

namespace action {

//#define ACTION_MACRO(DO) \
//    DO(BOMB) \
//    DO(INC) \
//    DO(MOVE) \
//    DO(MSG) \
//    DO(WAIT)
//
//#define MAKE_ENUM(VAR) VAR,
//enum Type {
//    ACTION_MACRO(MAKE_ENUM)
//};
//
//#define MAKE_STRINGS(VAR) #VAR,
//constexpr inline std::array<std::string_view, 5> NAMES = {{
//    ACTION_MACRO(MAKE_STRINGS)
//}};

struct Bomb {
    int from;
    int to;
};

struct Inc {
    int factory;
};

struct Move {
    int from;
    int to;
    int cyborgs;
};

struct Msg {
    std::string msg;
};

struct Wait {
};

using Action = std::variant<Bomb, Inc, Move, Msg, Wait>;

}  // namespace action
