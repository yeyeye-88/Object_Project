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
          "assets/pvz/ui/screens/title_screen.jpg"
      )),
      logoLoaded_(logoTexture_.loadFromFile("assets/pvz/ui/screens/title_logo.png")),
      startButtonLoaded_(startButtonTexture_.loadFromFile(
          "assets/pvz/ui/screens/start_grass_button.png"
      )) {}

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
    frame.setFillColor({17, 30, 25});
    window.draw(frame);
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        fit(background, {960.0F, 600.0F});
        const auto bounds = background.getGlobalBounds();
        background.setPosition({(960.0F - bounds.size.x) * 0.5F,
                                (600.0F - bounds.size.y) * 0.5F});
        window.draw(background);
    }

    if (logoLoaded_) {
        sf::Sprite logo(logoTexture_);
        fit(logo, {610.0F, 145.0F});
        const auto bounds = logo.getGlobalBounds();
        logo.setPosition({(960.0F - bounds.size.x) * 0.5F, 32.0F});
        window.draw(logo);
    }

    const sf::FloatRect promptBounds{{315.0F, 490.0F}, {330.0F, 90.0F}};
    if (startButtonLoaded_) {
        sf::Sprite prompt(startButtonTexture_);
        fit(prompt, promptBounds.size);
        prompt.setPosition(promptBounds.position);
        const auto pointer = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (promptBounds.contains(pointer)) {
            prompt.setColor({255, 245, 205});
        }
        window.draw(prompt);
    } else if (fontLoaded_) {
        sf::Text text(font_, ui::utf8("点击进入游戏"), 30);
        text.setFillColor({255, 229, 121, static_cast<std::uint8_t>(210 + 45 * (0.5F + 0.5F * std::sin(time_ * 4.0F)))});
        ui::centerText(text, promptBounds);
        window.draw(text);
    }
}

}  // namespace object_project
