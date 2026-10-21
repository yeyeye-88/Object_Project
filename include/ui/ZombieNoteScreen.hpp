#pragma once

#include "game/LevelDefinition.hpp"

#include <SFML/Graphics.hpp>

#include <array>

namespace object_project {

class ZombieNoteScreen {
public:
    ZombieNoteScreen();

    void setLevel(LevelId level);
    bool handleEvent(const sf::Event& event, const sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;

private:
    sf::Font font_;
    sf::Texture lawnTexture_;
    std::array<sf::Texture, 3> noteTextures_;
    sf::Texture buttonTexture_;
    bool fontLoaded_{};
    bool lawnLoaded_{};
    std::array<bool, 3> noteLoaded_{};
    bool buttonLoaded_{};
    LevelId level_{LevelId::Tutorial};
};

}  // namespace object_project
