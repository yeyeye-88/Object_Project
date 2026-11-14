#pragma once

#include <SFML/Graphics.hpp>

namespace object_project {

enum class ModeSelectCategory { Puzzle, MiniGames };
enum class ModeSelectAction { None, Back, FirstMode };

class ModeSelectScreen {
public:
    ModeSelectScreen();

    void setCategory(ModeSelectCategory category) { category_ = category; }
    ModeSelectCategory category() const { return category_; }
    ModeSelectAction handleEvent(const sf::Event& event, const sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;

private:
    void drawCard(
        sf::RenderWindow& window,
        const sf::FloatRect& bounds,
        const char* title,
        const char* description,
        bool available,
        bool hovered
    ) const;
    void drawText(
        sf::RenderWindow& window,
        const char* value,
        unsigned int size,
        sf::Vector2f position,
        sf::Color color
    ) const;

    static constexpr sf::FloatRect firstBounds{{245.0F, 180.0F}, {470.0F, 250.0F}};
    static constexpr sf::FloatRect backBounds{{55.0F, 520.0F}, {155.0F, 55.0F}};

    ModeSelectCategory category_{ModeSelectCategory::Puzzle};
    sf::Font font_;
    sf::Texture background_;
    bool fontLoaded_{};
    bool backgroundLoaded_{};
};

}  // namespace object_project
