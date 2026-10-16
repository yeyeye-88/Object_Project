#include "core/AppConfig.hpp"
#include "game/GameSession.hpp"
#include "ui/GameScreen.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <filesystem>

int main() {
    const std::string title = object_project::loadWindowTitle(
        std::filesystem::path{"assets/config/window_title.txt"},
        "Object Project - M2 Basic Combat"
    );

    sf::RenderWindow window(sf::VideoMode({960, 600}), title);
    window.setFramerateLimit(60);

    object_project::GameSession session;
    object_project::GameScreen screen(session);
    sf::Clock frameClock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else {
                screen.handleEvent(*event, window);
            }
        }

        const float deltaSeconds = std::min(frameClock.restart().asSeconds(), 0.1F);
        session.update(deltaSeconds);

        window.clear(sf::Color(35, 71, 39));
        screen.draw(window);
        window.display();
    }

    return 0;
}
