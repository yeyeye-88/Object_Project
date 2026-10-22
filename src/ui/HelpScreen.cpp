#include "ui/HelpScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>

namespace object_project {

namespace {

constexpr sf::FloatRect backBounds{{390.0F, 515.0F}, {180.0F, 54.0F}};

void drawFitted(sf::RenderWindow& window, const sf::Texture& texture, const sf::FloatRect bounds) {
    sf::Sprite sprite(texture);
    const sf::Vector2u source = texture.getSize();
    const float scale = std::min(
        bounds.size.x / static_cast<float>(source.x),
        bounds.size.y / static_cast<float>(source.y)
    );
    sprite.setScale({scale, scale});
    sprite.setPosition({
        bounds.position.x + (bounds.size.x - static_cast<float>(source.x) * scale) * 0.5F,
        bounds.position.y + (bounds.size.y - static_cast<float>(source.y) * scale) * 0.5F,
    });
    window.draw(sprite);
}

}  // namespace

HelpScreen::HelpScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile("assets/pvz/ui/reference/main_menu.png")),
      helpLoaded_(helpTexture_.loadFromFile("assets/pvz/ui/screens/help.png")),
      buttonLoaded_(buttonTexture_.loadFromFile("assets/pvz/ui/screens/stone_button.png")) {}

bool HelpScreen::handleEvent(const sf::Event& event, const sf::RenderWindow& window) const {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        return key->scancode == sf::Keyboard::Scancode::Escape ||
               key->scancode == sf::Keyboard::Scancode::Enter;
    }
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        return click->button == sf::Mouse::Button::Left &&
               backBounds.contains(window.mapPixelToCoords(click->position));
    }
    return false;
}

void HelpScreen::draw(sf::RenderWindow& window) const {
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        const sf::Vector2u size = backgroundTexture_.getSize();
        background.setScale({960.0F / size.x, 600.0F / size.y});
        window.draw(background);
    }
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({8, 22, 12, 145});
    window.draw(shade);

    if (helpLoaded_) {
        drawFitted(window, helpTexture_, {{150.0F, 55.0F}, {660.0F, 430.0F}});
    }
    if (buttonLoaded_) {
        sf::Sprite button(buttonTexture_);
        button.setTextureRect(sf::IntRect(
            {0, 0},
            {
                static_cast<int>(buttonTexture_.getSize().x),
                static_cast<int>(buttonTexture_.getSize().y / 2U),
            }
        ));
        button.setScale({
            backBounds.size.x / static_cast<float>(buttonTexture_.getSize().x),
            backBounds.size.y / static_cast<float>(buttonTexture_.getSize().y / 2U),
        });
        button.setPosition(backBounds.position);
        window.draw(button);
    }
    if (fontLoaded_) {
        sf::Text label(font_, ui::utf8("返回主菜单"), 24);
        label.setFillColor({238, 234, 206});
        ui::centerText(label, backBounds);
        window.draw(label);
    }
}

}  // namespace object_project
