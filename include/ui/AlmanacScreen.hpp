#pragma once

#include "game/AlmanacData.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>

namespace object_project {

class AlmanacScreen {
public:
    AlmanacScreen();

    bool handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

private:
    enum class Category {
        Plants,
        Zombies,
    };

    void drawText(
        sf::RenderWindow& window,
        const std::string& value,
        unsigned int size,
        sf::Vector2f position,
        sf::Color color = sf::Color::White
    ) const;
    const AlmanacEntry& selectedEntry() const;
    std::size_t entryCount() const;

    Category category_{Category::Plants};
    std::size_t selectedIndex_{};
    sf::Font font_;
    bool fontLoaded_{};
};

}  // namespace object_project
