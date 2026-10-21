#pragma once

#include <SFML/Graphics.hpp>

namespace object_project {

class IntroScreen {
public:
    IntroScreen();

    bool handleEvent(const sf::Event& event) const;
    void update(float deltaSeconds);
    void draw(sf::RenderWindow& window) const;

private:
    sf::Font font_;
    sf::Texture backgroundTexture_;
    sf::Texture logoTexture_;
    bool fontLoaded_{};
    bool backgroundLoaded_{};
    bool logoLoaded_{};
    float time_{};
};

}  // namespace object_project
