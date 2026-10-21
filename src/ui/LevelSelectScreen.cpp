#include "ui/LevelSelectScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <string>

namespace object_project {

LevelSelectScreen::LevelSelectScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile(
          "assets/pvz/ui/reference/main_menu.png"
      )),
      buttonLoaded_(buttonTexture_.loadFromFile(
          "assets/pvz/ui/reference/wood_button.png"
      )) {}

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
    sf::RectangleShape frame({960.0F, 600.0F});
    frame.setFillColor({18, 53, 31});
    window.draw(frame);
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        background.setPosition({30.0F, 0.0F});
        window.draw(background);
    }
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({12, 26, 18, 145});
    window.draw(shade);
    sf::RectangleShape titlePlate({360.0F, 68.0F});
    titlePlate.setPosition({300.0F, 48.0F});
    titlePlate.setFillColor({104, 55, 26, 245});
    titlePlate.setOutlineColor({230, 173, 73});
    titlePlate.setOutlineThickness(4.0F);
    window.draw(titlePlate);
    drawText(window, "选择关卡", 38, {398.0F, 58.0F}, {255, 224, 111});

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (std::size_t index = 0; index < levelBounds.size(); ++index) {
        const LevelDefinition& level = levelDefinition(static_cast<LevelId>(index));
        const sf::FloatRect& bounds = levelBounds[index];
        sf::RectangleShape panel(bounds.size);
        panel.setPosition(bounds.position);
        const bool hovered = bounds.contains(mouse);
        const std::array<sf::Color, levelCount> colors{{
            {84, 110, 58, 245},
            {74, 98, 52, 245},
            {65, 61, 87, 245},
            {84, 49, 80, 245},
        }};
        sf::Color panelColor = colors[index];
        if (hovered) {
            panelColor.r = static_cast<std::uint8_t>(std::min(255, panelColor.r + 24));
            panelColor.g = static_cast<std::uint8_t>(std::min(255, panelColor.g + 24));
            panelColor.b = static_cast<std::uint8_t>(std::min(255, panelColor.b + 18));
        }
        panel.setFillColor(panelColor);
        panel.setOutlineColor(hovered ? sf::Color(255, 225, 118) : sf::Color(192, 139, 65));
        panel.setOutlineThickness(hovered ? 5.0F : 3.0F);
        window.draw(panel);

        drawText(window, level.name, 24, {bounds.position.x + 20.0F, bounds.position.y + 12.0F}, {255, 229, 132});
        drawText(
            window,
            level.description,
            14,
            {bounds.position.x + 18.0F, bounds.position.y + 48.0F},
            {231, 229, 198}
        );
        drawText(
            window,
            "初始阳光  " + std::to_string(level.initialSun),
            18,
            {bounds.position.x + 18.0F, bounds.position.y + 80.0F},
            {250, 218, 91}
        );
        drawText(
            window,
            level.mode == GameMode::Endless
                ? std::string{"波次  无尽"}
                : "波次  " + std::to_string(level.waveCount),
            18,
            {bounds.position.x + 190.0F, bounds.position.y + 80.0F}
        );
        drawText(window, "点击开始", 16, {bounds.position.x + 151.0F, bounds.position.y + 116.0F});
    }

    if (buttonLoaded_) {
        sf::Sprite back(buttonTexture_);
        const sf::Vector2u source = buttonTexture_.getSize();
        back.setScale({backBounds.size.x / source.x, backBounds.size.y / source.y});
        back.setPosition(backBounds.position);
        window.draw(back);
    }
    drawText(window, "返回", 22, {119.0F, 532.0F}, {255, 220, 96});
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
    sf::Text text(font_, ui::utf8(value), size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}

}  // namespace object_project
