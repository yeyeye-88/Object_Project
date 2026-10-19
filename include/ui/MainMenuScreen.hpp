#pragma once

#include <SFML/Graphics.hpp>

namespace object_project {

enum class MainMenuAction {
    None,
    Play,
    Almanac,
    Exit,
};

class MainMenuScreen {
public:
    MainMenuScreen();

    MainMenuAction handleEvent(const sf::Event& event, const sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;

private:
    void drawButton(
        sf::RenderWindow& window,
        const sf::FloatRect& bounds,
        const char* label,
        sf::Color color
    ) const;
    void drawText(
        sf::RenderWindow& window,
        const char* value,
        unsigned int size,
        sf::Vector2f position,
        sf::Color color = sf::Color::White
    ) const;

    static constexpr sf::FloatRect playBounds{{330.0F, 230.0F}, {300.0F, 70.0F}};
    static constexpr sf::FloatRect almanacBounds{{330.0F, 320.0F}, {300.0F, 70.0F}};
    static constexpr sf::FloatRect exitBounds{{330.0F, 410.0F}, {300.0F, 70.0F}};

    sf::Font font_;
    bool fontLoaded_{};
};

}  // namespace object_project
