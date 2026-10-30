#pragma once

#include "game/LevelDefinition.hpp"

#include <SFML/Graphics.hpp>

#include <array>

namespace object_project {

enum class LevelSelectActionType {
    None,
    StartLevel,
    ResumeEndless,
    Back,
};

struct LevelSelectAction {
    LevelSelectActionType type{LevelSelectActionType::None};
    LevelId level{LevelId::Garden};
};

class LevelSelectScreen {
public:
    LevelSelectScreen();

    LevelSelectAction handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
    void setEndlessProgress(int highestWave, bool hasResume);

private:
    void drawText(
        sf::RenderWindow& window,
        std::string_view value,
        unsigned int size,
        sf::Vector2f position,
        sf::Color color = sf::Color::White
    ) const;

    static constexpr std::array<sf::FloatRect, levelCount> levelBounds{{
        {{70.0F, 150.0F}, {380.0F, 150.0F}},
        {{510.0F, 150.0F}, {380.0F, 150.0F}},
        {{70.0F, 330.0F}, {380.0F, 150.0F}},
        {{510.0F, 330.0F}, {380.0F, 150.0F}},
    }};
    static constexpr sf::FloatRect backBounds{{70.0F, 520.0F}, {150.0F, 55.0F}};
    static constexpr sf::FloatRect resumeEndlessBounds{{315.0F, 274.0F}, {330.0F, 58.0F}};
    static constexpr sf::FloatRect freshEndlessBounds{{315.0F, 350.0F}, {330.0F, 58.0F}};
    static constexpr sf::FloatRect cancelEndlessBounds{{390.0F, 427.0F}, {180.0F, 48.0F}};

    bool endlessDialogVisible_{};
    bool hasEndlessResume_{};
    int highestEndlessWave_{1};

    sf::Font font_;
    bool fontLoaded_{};
    sf::Texture backgroundTexture_;
    sf::Texture levelButtonTexture_;
    sf::Texture backButtonTexture_;
    bool backgroundLoaded_{};
    bool levelButtonLoaded_{};
    bool backButtonLoaded_{};
};

}  // namespace object_project
