#pragma once

#include <SFML/Graphics.hpp>

namespace object_project {

enum class MainMenuAction {
    None,
    Play,
    Almanac,
    Settings,
    Exit,
};

class MainMenuScreen {
public:
    MainMenuScreen();

    MainMenuAction handleEvent(const sf::Event& event, const sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;

private:
    void drawMenuAsset(
        sf::RenderWindow& window,
        const sf::Texture& texture,
        const sf::FloatRect& bounds,
        bool highlighted
    ) const;
    void drawText(
        sf::RenderWindow& window,
        const char* value,
        unsigned int size,
        sf::Vector2f position,
        sf::Color color = sf::Color::White
    ) const;

    static constexpr sf::FloatRect playBounds{{510.0F, 78.0F}, {340.0F, 150.0F}};
    static constexpr sf::FloatRect puzzleBounds{{525.0F, 218.0F}, {300.0F, 128.0F}};
    static constexpr sf::FloatRect miniGameBounds{{530.0F, 333.0F}, {300.0F, 127.0F}};
    static constexpr sf::FloatRect almanacBounds{{385.0F, 432.0F}, {96.0F, 96.0F}};
    static constexpr sf::FloatRect settingsBounds{{688.0F, 492.0F}, {82.0F, 80.0F}};
    static constexpr sf::FloatRect helpBounds{{772.0F, 506.0F}, {78.0F, 66.0F}};
    static constexpr sf::FloatRect exitBounds{{852.0F, 492.0F}, {82.0F, 80.0F}};

    sf::Font font_;
    bool fontLoaded_{};
    sf::Texture backgroundTexture_;
    sf::Texture adventureTexture_;
    sf::Texture puzzleTexture_;
    sf::Texture miniGameTexture_;
    sf::Texture almanacTexture_;
    sf::Texture welcomeSignTexture_;
    sf::Texture userSignTexture_;
    sf::Texture trophyTexture_;
    bool backgroundLoaded_{};
    bool adventureLoaded_{};
    bool puzzleLoaded_{};
    bool miniGameLoaded_{};
    bool almanacLoaded_{};
    bool welcomeSignLoaded_{};
    bool userSignLoaded_{};
    bool trophyLoaded_{};
};

}  // namespace object_project
