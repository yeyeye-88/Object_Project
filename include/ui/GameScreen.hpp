#pragma once

#include "game/GameSession.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <cstdint>
#include <optional>

namespace object_project {

class GameScreen {
public:
    explicit GameScreen(GameSession& session);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
    bool takeAlmanacRequest();

private:
    struct CardLayout {
        PlantType type;
        sf::Vector2f position;
    };

    static constexpr sf::Vector2f cardSize{58.0F, 80.0F};
    static constexpr sf::FloatRect shovelBounds{{530.0F, 15.0F}, {62.0F, 62.0F}};
    static constexpr sf::FloatRect almanacBounds{{790.0F, 18.0F}, {70.0F, 42.0F}};
    static constexpr sf::FloatRect pauseBounds{{874.0F, 18.0F}, {54.0F, 42.0F}};
    static constexpr sf::FloatRect resumeBounds{{365.0F, 260.0F}, {100.0F, 60.0F}};
    static constexpr sf::FloatRect restartBounds{{495.0F, 260.0F}, {100.0F, 60.0F}};

    static const std::array<CardLayout, plantTypeCount>& cardLayouts();
    static sf::Color plantColor(PlantType type, std::uint8_t alpha = 255);

    void handleLeftClick(sf::Vector2f position);
    void drawBackground(sf::RenderWindow& window) const;
    void drawCards(sf::RenderWindow& window) const;
    void drawBoard(sf::RenderWindow& window) const;
    void drawCombat(sf::RenderWindow& window) const;
    void drawSuns(sf::RenderWindow& window) const;
    void drawToolbar(sf::RenderWindow& window) const;
    void drawPauseOverlay(sf::RenderWindow& window) const;
    void drawResultOverlay(sf::RenderWindow& window) const;
    void drawPlant(sf::RenderWindow& window, PlantType type, GridPosition position, bool preview) const;
    void drawHealthBar(sf::RenderWindow& window, float ratio, sf::Vector2f position, float width) const;
    void drawNumber(sf::RenderWindow& window, int value, sf::Vector2f position) const;

    GameSession& session_;
    std::optional<PlantType> selectedPlant_;
    bool shovelSelected_{};
    bool almanacRequested_{};
    std::optional<GridPosition> hoveredCell_;

    sf::Texture backgroundTexture_;
    std::array<sf::Texture, plantTypeCount> cardTextures_;
    std::array<sf::Texture, plantTypeCount> plantTextures_;
    sf::Texture sunTexture_;
    sf::Texture shovelTexture_;
    bool backgroundLoaded_{};
    std::array<bool, plantTypeCount> cardLoaded_{};
    std::array<bool, plantTypeCount> plantLoaded_{};
    bool sunLoaded_{};
    bool shovelLoaded_{};
};

}  // namespace object_project
