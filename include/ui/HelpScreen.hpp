#pragma once

#include <SFML/Graphics.hpp>

namespace object_project {

class HelpScreen {
public:
    HelpScreen();

    bool handleEvent(const sf::Event& event, const sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;

private:
    sf::Font font_;
    sf::Texture backgroundTexture_;
    sf::Texture helpTexture_;
    sf::Texture buttonTexture_;
    bool fontLoaded_{};
    bool backgroundLoaded_{};
    bool helpLoaded_{};
    bool buttonLoaded_{};
};

}  // namespace object_project
