#pragma once

#include "audio/AudioSystem.hpp"
#include "game/GameSession.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace object_project {

class GameScreen {
public:
    explicit GameScreen(GameSession& session);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float deltaSeconds);
    void draw(sf::RenderWindow& window) const;
    std::vector<SoundCue> takeSoundCues();
    bool takeAlmanacRequest();
    bool takeMenuRequest();
    void resetInteraction();

private:
    struct CardLayout {
        PlantType type;
        sf::Vector2f position;
    };

    struct PulseEffect {
        sf::Vector2f center;
        sf::Color color;
        float age{};
        float duration{0.5F};
    };

    static constexpr sf::Vector2f cardSize{46.0F, 64.0F};
    static constexpr sf::FloatRect shovelBounds{{600.0F, 15.0F}, {62.0F, 62.0F}};
    static constexpr sf::FloatRect almanacBounds{{790.0F, 18.0F}, {70.0F, 42.0F}};
    static constexpr sf::FloatRect pauseBounds{{874.0F, 18.0F}, {54.0F, 42.0F}};
    static constexpr sf::FloatRect resumeBounds{{340.0F, 275.0F}, {130.0F, 48.0F}};
    static constexpr sf::FloatRect restartBounds{{490.0F, 275.0F}, {130.0F, 48.0F}};
    static constexpr sf::FloatRect menuBounds{{390.0F, 350.0F}, {180.0F, 50.0F}};
    static constexpr sf::FloatRect resultRestartBounds{{335.0F, 490.0F}, {130.0F, 48.0F}};
    static constexpr sf::FloatRect resultMenuBounds{{495.0F, 490.0F}, {130.0F, 48.0F}};

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
    void drawFeedback(sf::RenderWindow& window) const;
    void drawPlant(sf::RenderWindow& window, PlantType type, GridPosition position, bool preview) const;
    void drawHealthBar(sf::RenderWindow& window, float ratio, sf::Vector2f position, float width) const;
    void drawNumber(sf::RenderWindow& window, int value, sf::Vector2f position) const;

    GameSession& session_;
    std::optional<PlantType> selectedPlant_;
    bool shovelSelected_{};
    bool almanacRequested_{};
    bool menuRequested_{};
    std::optional<GridPosition> hoveredCell_;
    std::vector<SoundCue> soundCues_;
    std::vector<PulseEffect> pulseEffects_;
    float animationTime_{};
    float waveBannerSeconds_{};
    int observedWave_{};
    int observedTotalHealth_{};
    std::size_t observedProjectileCount_{};

    sf::Texture backgroundTexture_;
    std::array<sf::Texture, plantTypeCount> cardTextures_;
    std::array<sf::Texture, plantTypeCount> plantTextures_;
    std::array<sf::Texture, zombieTypeCount> zombieTextures_;
    std::array<std::vector<sf::Texture>, plantTypeCount> plantAnimationFrames_;
    std::array<std::vector<sf::Texture>, zombieTypeCount> zombieAnimationFrames_;
    std::vector<sf::Texture> poleWalkingAnimationFrames_;
    std::vector<sf::Texture> newspaperEnragedAnimationFrames_;
    std::array<sf::Texture, 3> threepeaterStemTextures_;
    sf::Texture threepeaterHeadTexture_;
    sf::Texture potatoMineTexture_;
    sf::Texture torchwoodBodyTexture_;
    sf::Texture torchwoodFireTexture_;
    sf::Texture mowerTexture_;
    sf::Texture peaTexture_;
    sf::Texture frozenPeaTexture_;
    sf::Texture sunTexture_;
    sf::Texture shovelTexture_;
    sf::Texture chooserTexture_;
    sf::Texture shovelSlotTexture_;
    sf::Texture overlayPanelTexture_;
    sf::Texture overlayButtonTexture_;
    sf::Texture resultPanelTexture_;
    sf::Texture zombiesWonTexture_;
    bool backgroundLoaded_{};
    std::array<bool, plantTypeCount> cardLoaded_{};
    std::array<bool, plantTypeCount> plantLoaded_{};
    std::array<bool, zombieTypeCount> zombieLoaded_{};
    std::array<bool, 3> threepeaterStemLoaded_{};
    bool threepeaterHeadLoaded_{};
    bool potatoMineLoaded_{};
    bool torchwoodBodyLoaded_{};
    bool torchwoodFireLoaded_{};
    bool mowerLoaded_{};
    bool peaLoaded_{};
    bool frozenPeaLoaded_{};
    bool sunLoaded_{};
    bool shovelLoaded_{};
    bool chooserLoaded_{};
    bool shovelSlotLoaded_{};
    bool overlayPanelLoaded_{};
    bool overlayButtonLoaded_{};
    bool resultPanelLoaded_{};
    bool zombiesWonLoaded_{};
    sf::Font feedbackFont_;
    bool feedbackFontLoaded_{};
};

}  // namespace object_project
