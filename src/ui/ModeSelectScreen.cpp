#include "ui/ModeSelectScreen.hpp"

#include "ui/UiText.hpp"

namespace object_project {

ModeSelectScreen::ModeSelectScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(background_.loadFromFile(
          "assets/pvz/minigames/challenge_background.jpg"
      )) {}

ModeSelectAction ModeSelectScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) const {
    const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
    if (!click || click->button != sf::Mouse::Button::Left) {
        return ModeSelectAction::None;
    }
    const sf::Vector2f point = window.mapPixelToCoords(click->position);
    if (backBounds.contains(point)) {
        return ModeSelectAction::Back;
    }
    if (firstBounds.contains(point)) {
        return ModeSelectAction::FirstMode;
    }
    if (secondBounds.contains(point)) {
        return ModeSelectAction::SecondMode;
    }
    return ModeSelectAction::None;
}

void ModeSelectScreen::draw(sf::RenderWindow& window) const {
    if (backgroundLoaded_) {
        sf::Sprite background(background_);
        background.setScale({1.2F, 1.0F});
        window.draw(background);
    } else {
        sf::RectangleShape background({960.0F, 600.0F});
        background.setFillColor({91, 55, 28});
        window.draw(background);
    }
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({24, 14, 7, 105});
    window.draw(shade);

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    drawText(
        window,
        category_ == ModeSelectCategory::Puzzle ? "益智模式" : "迷你游戏",
        44U,
        {380.0F, 70.0F},
        {255, 230, 126}
    );
    if (category_ == ModeSelectCategory::Puzzle) {
        drawCard(window, firstBounds, "砸罐子", "敲开罐子，利用其中的植物消灭僵尸。", true,
                 firstBounds.contains(mouse));
        drawCard(window, secondBounds, "我是僵尸", "控制僵尸吃掉五行脑子。", false,
                 secondBounds.contains(mouse));
    } else {
        drawCard(window, firstBounds, "坚果保龄球", "滚动坚果，反弹撞倒来袭僵尸。", true,
                 firstBounds.contains(mouse));
        drawCard(window, secondBounds, "宝石迷阵", "交换植物完成三消，同时抵挡僵尸。", false,
                 secondBounds.contains(mouse));
    }

    sf::RectangleShape back(backBounds.size);
    back.setPosition(backBounds.position);
    back.setFillColor(backBounds.contains(mouse) ? sf::Color(121, 68, 29) : sf::Color(82, 45, 23));
    back.setOutlineColor({240, 198, 96});
    back.setOutlineThickness(3.0F);
    window.draw(back);
    drawText(window, "返回", 26U, {100.0F, 529.0F}, sf::Color::White);
}

void ModeSelectScreen::drawCard(
    sf::RenderWindow& window,
    const sf::FloatRect& bounds,
    const char* title,
    const char* description,
    const bool available,
    const bool hovered
) const {
    sf::RectangleShape shadow(bounds.size + sf::Vector2f{10.0F, 10.0F});
    shadow.setPosition(bounds.position + sf::Vector2f{6.0F, 8.0F});
    shadow.setFillColor({27, 16, 9, 155});
    window.draw(shadow);

    sf::RectangleShape card(bounds.size);
    card.setPosition(bounds.position);
    card.setFillColor(available
        ? (hovered ? sf::Color(107, 91, 47) : sf::Color(77, 67, 38))
        : sf::Color(63, 58, 53));
    card.setOutlineColor(available ? sf::Color(229, 181, 77) : sf::Color(145, 132, 108));
    card.setOutlineThickness(4.0F);
    window.draw(card);
    drawText(window, title, 34U, bounds.position + sf::Vector2f{28.0F, 28.0F}, {255, 226, 120});
    drawText(window, description, 18U, bounds.position + sf::Vector2f{28.0F, 98.0F}, {245, 236, 209});
    drawText(
        window,
        available ? "点击开始" : "后续阶段开放",
        22U,
        bounds.position + sf::Vector2f{95.0F, 178.0F},
        available ? sf::Color::White : sf::Color(190, 181, 163)
    );
}

void ModeSelectScreen::drawText(
    sf::RenderWindow& window,
    const char* value,
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
