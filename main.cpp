#include "game.h"

#include "graph.h"
#include "utils.h"

struct Metrics {
    struct Metric {
        double sum = 0;
        int count = 0;
    };

    void Update(const std::string& metricName, double value) {
        metrics[metricName].sum += value;
        ++metrics[metricName].count;
    }

    void UpdateCounter(const std::string& metricName) {
        ++counters[metricName];
    }

    void Print(sf::RenderWindow& window, int64_t curMs) {
        if (lastPrintMs + 1'000 <= curMs) {
            lastPrintMs = curMs;
            for (auto& [name, metric] : metrics) {
                if (metric.count != 0) {
                    toPrint[name] = metric.sum / metric.count;
                    metric = {};
                }
            }
            for (auto& [name, count] : counters) {
                toPrint[name] = count;
                count = 0;
            }
        }
        float dy = 20;
        for (auto& [name, value] : toPrint) {
            DrawText(
                window,
                sf::Vector2f(window.getSize().x - 300, dy),
                name + ": " + std::to_string(value));
            dy += 30;
        }
    }

    int64_t lastPrintMs = 0;
    std::unordered_map<std::string, Metric> metrics;
    std::unordered_map<std::string, int> counters;
    std::unordered_map<std::string, double> toPrint;
};

template <typename TSim>
void RunSimulation(TSim& game, std::string_view name, int steps = 0) {
    sf::RenderWindow window(sf::VideoMode(2000, 1000), name.data());
    game.Init(window);
    sf::Clock clock;
    sf::Clock totalClock;
    Metrics metrics;
    const sf::Int64 usPerUpdate = 1'000'000;
    sf::Int64 waitUpdate = 0;

    int step = 0;
    while (window.isOpen() && ++step != steps) {
        window.clear(GREY);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            } else {
                game.HandleInput(event);
            }
        }
        if (!window.isOpen()) {
            break;
        }

        waitUpdate += totalClock.restart().asMicroseconds();
        while (waitUpdate >= usPerUpdate) {
            waitUpdate -= usPerUpdate;
            clock.restart();
            game.Update();
            metrics.Update("update time: {:.2f}us", clock.restart().asMicroseconds());
        }
        game.Render(static_cast<float>(waitUpdate) / usPerUpdate);

        metrics.Print(window, totalClock.getElapsedTime().asMilliseconds());

        window.display();
        std::this_thread::sleep_for(5ms);
    }
}

int main() {
    Engine engine;
    Game game(engine);
    engine.AddFactory(Whose::Mine, 1);
    engine.AddFactory(Whose::Neutral, 1);
    engine.AddFactory(Whose::Neutral, 2);
    engine.AddFactory(Whose::Opponent, 1);
    engine.SetGraph(GenerateCompleteGraph(engine));
    RunSimulation(game, "Ghost in the Cell");
    return 0;
}
