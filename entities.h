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
};

struct Troop {
    int id;
    Whose whose;
    int from;
    int to;
    int count;
    int eta;
};
