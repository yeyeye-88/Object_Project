#include "ui/MainMenuScreen.hpp"

namespace object_project {

namespace {

bool contains(const sf::FloatRect& bounds, const sf::Vector2f point) {
    return bounds.contains(point);
}

}  // namespace

MainMenuScreen::MainMenuScreen()
    : fontLoaded_(font_.openFromFile("assets/fonts/tuffy.ttf")) {}

MainMenuAction MainMenuScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) const {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button != sf::Mouse::Button::Left) {
            return MainMenuAction::None;
        }
        const sf::Vector2f point = window.mapPixelToCoords(click->position);
        if (contains(playBounds, point)) {
            return MainMenuAction::Play;
        }
        if (contains(almanacBounds, point)) {
            return MainMenuAction::Almanac;
        }
        if (contains(exitBounds, point)) {
            return MainMenuAction::Exit;
        }
    }
    return MainMenuAction::None;
}

void MainMenuScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background({960.0F, 600.0F});
    background.setFillColor({35, 82, 45});
    window.draw(background);

    sf::CircleShape sunGlow(115.0F);
    sunGlow.setPosition({365.0F, -95.0F});
    sunGlow.setFillColor({242, 199, 62, 70});
    window.draw(sunGlow);

    drawText(window, "OBJECT PROJECT", 54, {270.0F, 80.0F}, {250, 225, 115});
    drawText(window, "M4 RICH GAMEPLAY", 25, {352.0F, 150.0F}, {220, 235, 205});
    drawButton(window, playBounds, "SELECT LEVEL", {70, 132, 68});
    drawButton(window, almanacBounds, "ALMANAC", {91, 104, 61});
    drawButton(window, exitBounds, "EXIT", {128, 70, 54});
}

void MainMenuScreen::drawButton(
    sf::RenderWindow& window,
    const sf::FloatRect& bounds,
    const char* label,
    const sf::Color color
) const {
    sf::RectangleShape button(bounds.size);
    button.setPosition(bounds.position);
    button.setFillColor(color);
    button.setOutlineColor({236, 211, 128});
    button.setOutlineThickness(3.0F);
    window.draw(button);
    drawText(window, label, 25, {bounds.position.x + 52.0F, bounds.position.y + 20.0F});
}

void MainMenuScreen::drawText(
    sf::RenderWindow& window,
    const char* value,
    const unsigned int size,
    const sf::Vector2f position,
    const sf::Color color
) const {
    if (!fontLoaded_) {
        return;
    }
    sf::Text text(font_, value, size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}

}  // namespace object_project
