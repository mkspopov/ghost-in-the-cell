#include "engine.h"
#include "entities.h"
#include "game.h"
#include "graph.h"
#include "player.h"
#include "utils.h"

int main() {
    Engine engine;
    engine.AddFactory(Whose::Mine, 1, 10);
    engine.AddFactory(Whose::Neutral, 0, 6);
    engine.AddFactory(Whose::Neutral, 1, 10);
    engine.AddFactory(Whose::Neutral, 2, 15);
    engine.AddFactory(Whose::Neutral, 0, 6);
    engine.AddFactory(Whose::Neutral, 1, 20);
    engine.AddFactory(Whose::Opponent, 1, 10);
    engine.SetGraph(GenerateRandomGraph(engine));

    Player mine(Whose::Mine, RandomStrategy{});
    Player opponent(Whose::Opponent, RandomStrategy{});

    for (int i = 0; i < 200; ++i) {
        engine.Move();
        mine.Play(engine);
        opponent.Play(engine);
        engine.Update();

        const auto winner = engine.HasWinner();
        if (winner != Whose::Neutral) {
            if (winner == Whose::Mine) {
                std::cerr << "I win" << std::endl;
            } else {
                std::cerr << "opponent wins" << std::endl;
            }
            break;
        }
    }

    std::cout << "Done." << std::endl;
}
