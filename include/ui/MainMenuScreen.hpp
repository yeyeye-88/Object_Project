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

    static constexpr sf::FloatRect playBounds{{540.0F, 105.0F}, {300.0F, 126.0F}};
    static constexpr sf::FloatRect almanacBounds{{535.0F, 275.0F}, {145.0F, 54.0F}};
    static constexpr sf::FloatRect settingsBounds{{700.0F, 275.0F}, {145.0F, 54.0F}};
    static constexpr sf::FloatRect exitBounds{{618.0F, 355.0F}, {145.0F, 50.0F}};

    sf::Font font_;
    bool fontLoaded_{};
    sf::Texture backgroundTexture_;
    sf::Texture adventureTexture_;
    sf::Texture adventureHoverTexture_;
    sf::Texture almanacTexture_;
    sf::Texture almanacHoverTexture_;
    sf::Texture optionsTexture_;
    sf::Texture optionsHoverTexture_;
    sf::Texture quitTexture_;
    sf::Texture quitHoverTexture_;
    sf::Texture woodButtonTexture_;
    bool backgroundLoaded_{};
    bool adventureLoaded_{};
    bool adventureHoverLoaded_{};
    bool almanacLoaded_{};
    bool almanacHoverLoaded_{};
    bool optionsLoaded_{};
    bool optionsHoverLoaded_{};
    bool quitLoaded_{};
    bool quitHoverLoaded_{};
    bool woodButtonLoaded_{};
};

}  // namespace object_project
