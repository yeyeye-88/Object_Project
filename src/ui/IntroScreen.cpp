#include "ui/IntroScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <cmath>

namespace object_project {

namespace {

void fit(sf::Sprite& sprite, const sf::Vector2f size) {
    const sf::Vector2u source = sprite.getTexture().getSize();
    const float scale = std::min(
        size.x / static_cast<float>(source.x),
        size.y / static_cast<float>(source.y)
    );
    sprite.setScale({scale, scale});
}

}  // namespace

IntroScreen::IntroScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile(
          "assets/pvz/ui/reference/main_menu.png"
      )),
      logoLoaded_(logoTexture_.loadFromFile("assets/pvz/ui/pvz_logo_.png")) {}

bool IntroScreen::handleEvent(const sf::Event& event) const {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        return click->button == sf::Mouse::Button::Left;
    }
    return event.is<sf::Event::KeyPressed>();
}

void IntroScreen::update(const float deltaSeconds) {
    time_ += deltaSeconds;
}

void IntroScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape frame({960.0F, 600.0F});
    frame.setFillColor({18, 53, 31});
    window.draw(frame);
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        background.setPosition({30.0F, 0.0F});
        window.draw(background);
    }
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({12, 30, 20, 65});
    window.draw(shade);

    if (logoLoaded_) {
        sf::Sprite logo(logoTexture_);
        fit(logo, {520.0F, 190.0F});
        logo.setPosition({220.0F, 80.0F});
        window.draw(logo);
    }

    sf::FloatRect promptBounds{{365.0F, 470.0F}, {230.0F, 56.0F}};
    sf::RectangleShape prompt(promptBounds.size);
    prompt.setPosition(promptBounds.position);
    prompt.setFillColor({104, 58, 28, 235});
    prompt.setOutlineColor({255, 216, 94});
    prompt.setOutlineThickness(3.0F);
    window.draw(prompt);
    if (fontLoaded_) {
        sf::Text text(font_, ui::utf8("点击进入游戏"), 30);
        text.setFillColor({255, 229, 121, static_cast<std::uint8_t>(210 + 45 * (0.5F + 0.5F * std::sin(time_ * 4.0F)))});
        ui::centerText(text, promptBounds);
        window.draw(text);
    }
}

}  // namespace object_project
