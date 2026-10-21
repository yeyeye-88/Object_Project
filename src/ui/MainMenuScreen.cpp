#include "ui/MainMenuScreen.hpp"

#include "ui/UiText.hpp"

#include <string>

namespace object_project {

namespace {

bool contains(const sf::FloatRect& bounds, const sf::Vector2f point) {
    return bounds.contains(point);
}

}  // namespace

MainMenuScreen::MainMenuScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile("assets/pvz/ui/reference/main_menu.png")),
      adventureLoaded_(adventureTexture_.loadFromFile(
          "assets/pvz/ui/reference/adventure_cn.png"
      )),
      adventureHoverLoaded_(
          adventureHoverTexture_.loadFromFile(
              "assets/pvz/ui/reference/adventure_cn_hover.png"
          )
      ),
      almanacLoaded_(almanacTexture_.loadFromFile("assets/pvz/ui/selectorscreen_almanac.png")),
      almanacHoverLoaded_(almanacHoverTexture_.loadFromFile(
          "assets/pvz/ui/selectorscreen_almanachighlight.png"
      )),
      optionsLoaded_(optionsTexture_.loadFromFile("assets/pvz/ui/selectorscreen_options1.png")),
      optionsHoverLoaded_(optionsHoverTexture_.loadFromFile(
          "assets/pvz/ui/selectorscreen_options2.png"
      )),
      quitLoaded_(quitTexture_.loadFromFile("assets/pvz/ui/selectorscreen_quit1.png")),
      quitHoverLoaded_(quitHoverTexture_.loadFromFile("assets/pvz/ui/selectorscreen_quit2.png")),
      woodButtonLoaded_(woodButtonTexture_.loadFromFile(
          "assets/pvz/ui/reference/wood_button.png"
      )) {}

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
        background.setPosition({30.0F, 0.0F});
        window.draw(background);
    } else {
        sf::RectangleShape background({960.0F, 600.0F});
        background.setFillColor({35, 82, 45});
        window.draw(background);
    }

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const sf::Texture* adventure = contains(playBounds, mouse) && adventureHoverLoaded_
        ? &adventureHoverTexture_
        : adventureLoaded_ ? &adventureTexture_ : nullptr;
    if (adventure) {
        sf::Sprite button(*adventure);
        const sf::Vector2u source = adventure->getSize();
        button.setScale({
            playBounds.size.x / static_cast<float>(source.x),
            playBounds.size.y / static_cast<float>(source.y),
        });
        button.setPosition(playBounds.position);
        window.draw(button);
    } else {
        drawButton(window, playBounds, "选择关卡", {70, 132, 68});
    }
    drawButton(window, almanacBounds, "图鉴", {130, 65, 27});
    drawButton(window, settingsBounds, "设置", {130, 65, 27});
    drawButton(window, exitBounds, "退出", {130, 65, 27});
}

void MainMenuScreen::drawButton(
    sf::RenderWindow& window,
    const sf::FloatRect& bounds,
    const char* label,
    const sf::Color color
) const {
    const bool hovered = bounds.contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    if (woodButtonLoaded_) {
        sf::Sprite button(woodButtonTexture_);
        const sf::Vector2u source = woodButtonTexture_.getSize();
        button.setScale({
            bounds.size.x / static_cast<float>(source.x),
            bounds.size.y / static_cast<float>(source.y),
        });
        button.setPosition(bounds.position);
        if (hovered) {
            button.setColor({255, 225, 175});
        }
        window.draw(button);
    } else {
        sf::RectangleShape button(bounds.size);
        button.setPosition(bounds.position);
        button.setFillColor(color);
        button.setOutlineColor({236, 211, 128});
        button.setOutlineThickness(3.0F);
        window.draw(button);
    }
    if (fontLoaded_) {
        sf::Text text(font_, ui::utf8(label), 25);
        text.setFillColor({255, 218, 82});
        ui::centerText(text, bounds);
        window.draw(text);
    }
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
