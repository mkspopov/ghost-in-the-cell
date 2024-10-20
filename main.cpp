#include "game.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

const inline auto FONT = []() {
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/tlwg/Purisa.ttf")) {
        throw std::runtime_error("cannot load Purisa font");
    }
    return font;
}();

void DrawText(sf::RenderWindow& window, sf::Vector2f pos, const std::string& str) {
    static sf::Text text(str, FONT, 20);
    text.setString(str);
    text.setPosition(pos);
    window.draw(text);
}

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

        const auto dt = game.Update();
        game.Render(0);

        metrics.Update("update time: {:.2f}us", clock.restart().asMicroseconds());
        metrics.Update("updates per frame: {:.3f}", 1 / dt);
        metrics.Print(window, totalClock.getElapsedTime().asMilliseconds());

        window.display();
        std::this_thread::sleep_for(500ms);
    }
}

std::unique_ptr<Graph> GenerateCompleteGraph(Engine& engine) {
    /*
     * Is the graph always complete?
     */
    auto graph = std::make_unique<Graph>(engine.factories_.size());
    for (std::size_t i = 0; i < engine.factories_.size(); ++i) {
        for (std::size_t j = i + 1; j < engine.factories_.size(); ++j) {
            graph->AddEdge(i, j, j - i);
            graph->AddEdge(j, i, j - i);
        }
    }
    return graph;
}

int main() {
    Game game;
    game.engine_.AddFactory(Whose::Mine, 1);
    game.engine_.AddFactory(Whose::Neutral, 1);
    game.engine_.AddFactory(Whose::Neutral, 2);
    game.engine_.AddFactory(Whose::Opponent, 1);
    game.engine_.graph_ = GenerateCompleteGraph(game.engine_);
    RunSimulation(game, "Ghost in the Cell");
    return 0;
}
