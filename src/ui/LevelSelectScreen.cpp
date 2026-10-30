#include "ui/LevelSelectScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <string>

namespace object_project {

LevelSelectScreen::LevelSelectScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile(
          "assets/pvz/ui/screens/challenge_background.jpg"
      )),
      levelButtonLoaded_(levelButtonTexture_.loadFromFile(
          "assets/pvz/ui/screens/stone_button.png"
      )),
      backButtonLoaded_(backButtonTexture_.loadFromFile(
          "assets/pvz/ui/screens/small_button.png"
      )) {}

LevelSelectAction LevelSelectScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Escape) {
            if (endlessDialogVisible_) {
                endlessDialogVisible_ = false;
                return {};
            }
            return {LevelSelectActionType::Back, LevelId::Garden};
        }
    }
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button != sf::Mouse::Button::Left) {
            return {};
        }
        const sf::Vector2f point = window.mapPixelToCoords(click->position);
        if (endlessDialogVisible_) {
            if (resumeEndlessBounds.contains(point)) {
                endlessDialogVisible_ = false;
                return {LevelSelectActionType::ResumeEndless, LevelId::EndlessDefense};
            }
            if (freshEndlessBounds.contains(point)) {
                endlessDialogVisible_ = false;
                return {LevelSelectActionType::StartLevel, LevelId::EndlessDefense};
            }
            if (cancelEndlessBounds.contains(point)) {
                endlessDialogVisible_ = false;
            }
            return {};
        }
        if (backBounds.contains(point)) {
            return {LevelSelectActionType::Back, LevelId::Garden};
        }
        for (std::size_t index = 0; index < levelBounds.size(); ++index) {
            if (levelBounds[index].contains(point)) {
                const LevelId selected = static_cast<LevelId>(index);
                if (selected == LevelId::EndlessDefense && hasEndlessResume_) {
                    endlessDialogVisible_ = true;
                    return {};
                }
                return {
                    LevelSelectActionType::StartLevel,
                    selected,
                };
            }
        }
    }
    return {};
}

void LevelSelectScreen::setEndlessProgress(const int highestWave, const bool hasResume) {
    highestEndlessWave_ = std::max(1, highestWave);
    hasEndlessResume_ = hasResume;
}

void LevelSelectScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape frame({960.0F, 600.0F});
    frame.setFillColor({18, 53, 31});
    window.draw(frame);
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        const sf::Vector2u source = backgroundTexture_.getSize();
        background.setScale({960.0F / source.x, 600.0F / source.y});
        window.draw(background);
    }
    drawText(window, "选择关卡", 34, {405.0F, 27.0F}, {53, 49, 48});

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (std::size_t index = 0; index < levelBounds.size(); ++index) {
        const LevelDefinition& level = levelDefinition(static_cast<LevelId>(index));
        const sf::FloatRect& bounds = levelBounds[index];
        const bool hovered = bounds.contains(mouse);
        if (levelButtonLoaded_) {
            const sf::Vector2u source = levelButtonTexture_.getSize();
            const unsigned int frameHeight = source.y / 2U;
            sf::Sprite panel(levelButtonTexture_);
            panel.setTextureRect(sf::IntRect(
                {0, hovered ? static_cast<int>(frameHeight) : 0},
                {static_cast<int>(source.x), static_cast<int>(frameHeight)}
            ));
            panel.setScale({bounds.size.x / source.x, bounds.size.y / frameHeight});
            panel.setPosition(bounds.position);
            window.draw(panel);
        } else {
            sf::RectangleShape panel(bounds.size);
            panel.setPosition(bounds.position);
            panel.setFillColor(hovered ? sf::Color(147, 147, 164) : sf::Color(113, 114, 132));
            window.draw(panel);
        }

        const sf::Color primary{45, 42, 48};
        const sf::Color secondary{72, 66, 70};
        drawText(window, level.name, 23, {bounds.position.x + 22.0F, bounds.position.y + 14.0F}, primary);
        drawText(
            window,
            level.description,
            14,
            {bounds.position.x + 22.0F, bounds.position.y + 47.0F},
            secondary
        );
        drawText(
            window,
            "初始阳光  " + std::to_string(level.initialSun),
            18,
            {bounds.position.x + 22.0F, bounds.position.y + 78.0F},
            primary
        );
        drawText(
            window,
            level.mode == GameMode::Endless
                ? std::string{"波次  无尽"}
                : "波次  " + std::to_string(level.waveCount),
            18,
            {bounds.position.x + 205.0F, bounds.position.y + 78.0F},
            primary
        );
        drawText(
            window,
            hovered ? "点击进入" : "开始关卡",
            15,
            {bounds.position.x + 151.0F, bounds.position.y + 116.0F},
            primary
        );
    }

    if (backButtonLoaded_) {
        sf::Sprite back(backButtonTexture_);
        const sf::Vector2u source = backButtonTexture_.getSize();
        back.setScale({backBounds.size.x / source.x, backBounds.size.y / source.y});
        back.setPosition(backBounds.position);
        window.draw(back);
    }
    drawText(window, "返回", 20, {121.0F, 531.0F}, {238, 234, 206});

    if (endlessDialogVisible_) {
        sf::RectangleShape veil({960.0F, 600.0F});
        veil.setFillColor({8, 13, 9, 185});
        window.draw(veil);
        sf::RectangleShape panel({430.0F, 330.0F});
        panel.setPosition({265.0F, 170.0F});
        panel.setFillColor({92, 57, 30, 245});
        panel.setOutlineColor({231, 190, 91});
        panel.setOutlineThickness(4.0F);
        window.draw(panel);
        drawText(window, "无尽模式", 34, {406.0F, 191.0F}, {255, 224, 112});
        drawText(window, "历史最高波次：" + std::to_string(highestEndlessWave_), 20,
                 {389.0F, 237.0F}, sf::Color::White);
        const auto drawChoice = [&](const sf::FloatRect& bounds, const std::string& label) {
            sf::RectangleShape button(bounds.size);
            button.setPosition(bounds.position);
            button.setFillColor({104, 64, 31});
            button.setOutlineColor({245, 201, 92});
            button.setOutlineThickness(2.0F);
            window.draw(button);
            drawText(window, label, 21, {bounds.position.x + 55.0F, bounds.position.y + 13.0F},
                     {255, 241, 194});
        };
        drawChoice(resumeEndlessBounds, "继续上次完整战局");
        drawChoice(freshEndlessBounds, "从第 1 波重新开始");
        drawChoice(cancelEndlessBounds, "取消");
    }
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
