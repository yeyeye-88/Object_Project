#include "core/AppConfig.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <filesystem>

namespace {

constexpr unsigned int windowWidth = 960;
constexpr unsigned int windowHeight = 600;
constexpr int lawnRows = 5;
constexpr int lawnColumns = 9;
constexpr float cellWidth = 88.0F;
constexpr float cellHeight = 92.0F;
constexpr float lawnLeft = 140.0F;
constexpr float lawnTop = 105.0F;

void drawLawn(sf::RenderWindow& window) {
    const std::array<sf::Color, 2> lawnColors{
        sf::Color(94, 162, 74),
        sf::Color(111, 181, 82),
    };

    for (int row = 0; row < lawnRows; ++row) {
        for (int column = 0; column < lawnColumns; ++column) {
            sf::RectangleShape cell({cellWidth - 2.0F, cellHeight - 2.0F});
            cell.setPosition({
                lawnLeft + static_cast<float>(column) * cellWidth,
                lawnTop + static_cast<float>(row) * cellHeight,
            });
            cell.setFillColor(lawnColors[static_cast<std::size_t>((row + column) % 2)]);
            window.draw(cell);
        }
    }

    sf::CircleShape sun(32.0F);
    sun.setPosition({38.0F, 24.0F});
    sun.setFillColor(sf::Color(255, 210, 55));
    sun.setOutlineColor(sf::Color(255, 242, 150));
    sun.setOutlineThickness(4.0F);
    window.draw(sun);
}

}  // namespace

int main() {
    const std::string title = object_project::loadWindowTitle(
        std::filesystem::path{"assets/config/window_title.txt"},
        "Object Project - M0"
    );

    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), title);
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color(35, 71, 39));
        drawLawn(window);
        window.display();
    }

    return 0;
}

