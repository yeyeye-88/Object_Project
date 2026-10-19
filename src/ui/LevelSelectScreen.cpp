#include "ui/LevelSelectScreen.hpp"

#include <string>

namespace object_project {

LevelSelectScreen::LevelSelectScreen()
    : fontLoaded_(font_.openFromFile("assets/fonts/tuffy.ttf")) {}

LevelSelectAction LevelSelectScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) const {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Escape) {
            return {LevelSelectActionType::Back, LevelId::Garden};
        }
    }
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button != sf::Mouse::Button::Left) {
            return {};
        }
        const sf::Vector2f point = window.mapPixelToCoords(click->position);
        if (backBounds.contains(point)) {
            return {LevelSelectActionType::Back, LevelId::Garden};
        }
        for (std::size_t index = 0; index < levelBounds.size(); ++index) {
            if (levelBounds[index].contains(point)) {
                return {
                    LevelSelectActionType::StartLevel,
                    static_cast<LevelId>(index),
                };
            }
        }
    }
    return {};
}

void LevelSelectScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background({960.0F, 600.0F});
    background.setFillColor({31, 63, 43});
    window.draw(background);
    drawText(window, "SELECT A LEVEL", 42, {330.0F, 65.0F}, {248, 222, 124});

    for (std::size_t index = 0; index < levelBounds.size(); ++index) {
        const LevelDefinition& level = levelDefinition(static_cast<LevelId>(index));
        const sf::FloatRect& bounds = levelBounds[index];
        sf::RectangleShape panel(bounds.size);
        panel.setPosition(bounds.position);
        panel.setFillColor(index == 0 ? sf::Color(73, 118, 65)
                                     : index == 1 ? sf::Color(72, 99, 58)
                                                  : sf::Color(62, 64, 86));
        panel.setOutlineColor({229, 199, 108});
        panel.setOutlineThickness(3.0F);
        window.draw(panel);

        drawText(window, level.name, 22, {bounds.position.x + 18.0F, bounds.position.y + 22.0F});
        drawText(
            window,
            level.description,
            14,
            {bounds.position.x + 18.0F, bounds.position.y + 80.0F},
            {215, 225, 202}
        );
        drawText(
            window,
            "SUN  " + std::to_string(level.initialSun),
            18,
            {bounds.position.x + 18.0F, bounds.position.y + 160.0F},
            {250, 218, 91}
        );
        drawText(
            window,
            "WAVES  " + std::to_string(level.waveCount),
            18,
            {bounds.position.x + 18.0F, bounds.position.y + 198.0F}
        );
        drawText(window, "CLICK TO START", 16, {bounds.position.x + 50.0F, bounds.position.y + 238.0F});
    }

    sf::RectangleShape back(backBounds.size);
    back.setPosition(backBounds.position);
    back.setFillColor({112, 70, 52});
    back.setOutlineColor({229, 199, 108});
    back.setOutlineThickness(2.0F);
    window.draw(back);
    drawText(window, "BACK", 22, {112.0F, 514.0F});
}

void LevelSelectScreen::drawText(
    sf::RenderWindow& window,
    const std::string_view value,
    const unsigned int size,
    const sf::Vector2f position,
    const sf::Color color
) const {
    if (!fontLoaded_) {
        return;
    }
    sf::Text text(font_, std::string(value), size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}

}  // namespace object_project
