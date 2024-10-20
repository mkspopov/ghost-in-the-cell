#pragma once

enum Whose {
    Opponent = -1,
    Neutral,
    Mine,
    MaybeMine,
};

struct Bomb {
    int id;
    Whose whose;
    int from;
    int to;
    int eta;
};

struct Factory {
    int id;
    Whose whose;
    int production;
    int cyborgs;

    int explosion_ = 0;
};

struct Troop {
    int id;
    Whose whose;
    int from;
    int to;
    int count;
    int eta;
};
