#include "ui/MainMenuScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <string>

namespace object_project {

namespace {

bool contains(const sf::FloatRect& bounds, const sf::Vector2f point) {
    return bounds.contains(point);
}

void fitSprite(sf::Sprite& sprite, const sf::Vector2f maximumSize) {
    const sf::Vector2u source = sprite.getTexture().getSize();
    if (source.x == 0 || source.y == 0) {
        return;
    }
    const float scale = std::min(
        maximumSize.x / static_cast<float>(source.x),
        maximumSize.y / static_cast<float>(source.y)
    );
    sprite.setScale({scale, scale});
}

}  // namespace

MainMenuScreen::MainMenuScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile("assets/pvz/ui/reference/main_menu.png")),
      adventureLoaded_(adventureTexture_.loadFromFile(
          "assets/pvz/ui/main_menu/adventure.png"
      )),
      puzzleLoaded_(puzzleTexture_.loadFromFile("assets/pvz/ui/main_menu/puzzle.png")),
      miniGameLoaded_(miniGameTexture_.loadFromFile("assets/pvz/ui/main_menu/minigames.png")),
      almanacLoaded_(almanacTexture_.loadFromFile("assets/pvz/ui/main_menu/almanac.png")),
      welcomeSignLoaded_(welcomeSignTexture_.loadFromFile(
          "assets/pvz/ui/main_menu/welcome_sign.png"
      )),
      userSignLoaded_(userSignTexture_.loadFromFile(
          "assets/pvz/ui/main_menu/user_sign.png"
      )),
      trophyLoaded_(trophyTexture_.loadFromFile("assets/pvz/ui/main_menu/trophy.png")) {}

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
        if (contains(settingsBounds, point)) {
            return MainMenuAction::Settings;
        }
        if (contains(helpBounds, point)) {
            return MainMenuAction::Help;
        }
        if (contains(exitBounds, point)) {
            return MainMenuAction::Exit;
        }
    }
    return MainMenuAction::None;
}

void MainMenuScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape frame({960.0F, 600.0F});
    frame.setFillColor({18, 53, 31});
    window.draw(frame);
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        const sf::Vector2u source = backgroundTexture_.getSize();
        background.setScale({
            960.0F / static_cast<float>(source.x),
            600.0F / static_cast<float>(source.y),
        });
        window.draw(background);
    } else {
        sf::RectangleShape background({960.0F, 600.0F});
        background.setFillColor({35, 82, 45});
        window.draw(background);
    }

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (welcomeSignLoaded_) {
        sf::Sprite welcome(welcomeSignTexture_);
        welcome.setPosition({34.0F, 20.0F});
        window.draw(welcome);
    }
    if (userSignLoaded_) {
        drawMenuAsset(
            window,
            userSignTexture_,
            {{36.0F, 142.0F}, {291.0F, 71.0F}},
            contains({{36.0F, 142.0F}, {291.0F, 71.0F}}, mouse)
        );
    }
    if (trophyLoaded_) {
        sf::Sprite trophy(trophyTexture_);
        fitSprite(trophy, {150.0F, 120.0F});
        trophy.setPosition({58.0F, 410.0F});
        window.draw(trophy);
    }

    if (adventureLoaded_) {
        drawMenuAsset(window, adventureTexture_, playBounds, contains(playBounds, mouse));
    }
    if (puzzleLoaded_) {
        drawMenuAsset(window, puzzleTexture_, puzzleBounds, false);
    }
    if (miniGameLoaded_) {
        drawMenuAsset(window, miniGameTexture_, miniGameBounds, false);
    }
    if (almanacLoaded_) {
        drawMenuAsset(window, almanacTexture_, almanacBounds, contains(almanacBounds, mouse));
    }

    drawText(
        window,
        "选项",
        contains(settingsBounds, mouse) ? 24U : 22U,
        {700.0F, 507.0F},
        contains(settingsBounds, mouse) ? sf::Color(255, 245, 155) : sf::Color(64, 55, 43)
    );
    drawText(
        window,
        "帮助",
        contains(helpBounds, mouse) ? 22U : 20U,
        {784.0F, 520.0F},
        contains(helpBounds, mouse) ? sf::Color(255, 245, 155) : sf::Color(64, 55, 43)
    );
    drawText(
        window,
        "退出",
        contains(exitBounds, mouse) ? 24U : 22U,
        {864.0F, 510.0F},
        contains(exitBounds, mouse) ? sf::Color(255, 245, 155) : sf::Color(64, 55, 43)
    );
}

void MainMenuScreen::drawMenuAsset(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::FloatRect& bounds,
    const bool highlighted
) const {
    const sf::Vector2u source = texture.getSize();
    const int frameHeight = static_cast<int>(source.y / 2U);
    sf::Sprite sprite(texture);
    sprite.setTextureRect(sf::IntRect(
        {0, highlighted ? frameHeight : 0},
        {static_cast<int>(source.x), frameHeight}
    ));
    sprite.setScale({
        bounds.size.x / static_cast<float>(source.x),
        bounds.size.y / static_cast<float>(frameHeight),
    });
    sprite.setPosition(bounds.position);
    window.draw(sprite);
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
    sf::Text text(font_, ui::utf8(value), size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}

}  // namespace object_project
