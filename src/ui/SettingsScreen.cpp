#include "ui/SettingsScreen.hpp"

#include "ui/UiText.hpp"

#include <string>

namespace object_project {

namespace {

constexpr sf::FloatRect musicMinus{{540.0F, 185.0F}, {50.0F, 46.0F}};
constexpr sf::FloatRect musicPlus{{760.0F, 185.0F}, {50.0F, 46.0F}};
constexpr sf::FloatRect effectsMinus{{540.0F, 255.0F}, {50.0F, 46.0F}};
constexpr sf::FloatRect effectsPlus{{760.0F, 255.0F}, {50.0F, 46.0F}};
constexpr sf::FloatRect resolutionMinus{{540.0F, 325.0F}, {50.0F, 46.0F}};
constexpr sf::FloatRect resolutionPlus{{760.0F, 325.0F}, {50.0F, 46.0F}};
constexpr sf::FloatRect fullscreenBounds{{540.0F, 395.0F}, {270.0F, 46.0F}};
constexpr sf::FloatRect backBounds{{365.0F, 500.0F}, {230.0F, 58.0F}};

bool contains(const sf::FloatRect bounds, const sf::Vector2f point) {
    return bounds.contains(point);
}

}  // namespace

SettingsScreen::SettingsScreen(AppSettings& settings)
    : settings_(settings),
      fontLoaded_(ui::loadChineseFont(font_)),
      backgroundLoaded_(backgroundTexture_.loadFromFile(
          "assets/pvz/ui/reference/main_menu.png"
      )),
      buttonLoaded_(buttonTexture_.loadFromFile(
          "assets/pvz/ui/reference/wood_button.png"
      )) {}

SettingsAction SettingsScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Escape) {
            return SettingsAction::Back;
        }
    }
    const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
    if (!click || click->button != sf::Mouse::Button::Left) {
        return SettingsAction::None;
    }
    const sf::Vector2f point = window.mapPixelToCoords(click->position);
    if (contains(musicMinus, point) || contains(musicPlus, point)) {
        settings_.adjustMusicVolume(contains(musicMinus, point) ? -10 : 10);
        return SettingsAction::Changed;
    }
    if (contains(effectsMinus, point) || contains(effectsPlus, point)) {
        settings_.adjustEffectsVolume(contains(effectsMinus, point) ? -10 : 10);
        return SettingsAction::Changed;
    }
    if (contains(resolutionMinus, point) || contains(resolutionPlus, point)) {
        settings_.cycleResolution(contains(resolutionMinus, point) ? -1 : 1);
        return SettingsAction::WindowChanged;
    }
    if (contains(fullscreenBounds, point)) {
        settings_.toggleFullscreen();
        return SettingsAction::WindowChanged;
    }
    return contains(backBounds, point) ? SettingsAction::Back : SettingsAction::None;
}

void SettingsScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape frame({960.0F, 600.0F});
    frame.setFillColor({18, 53, 31});
    window.draw(frame);
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        background.setPosition({30.0F, 0.0F});
        window.draw(background);
    }
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({13, 28, 18, 150});
    window.draw(shade);
    sf::RectangleShape panel({650.0F, 400.0F});
    panel.setPosition({155.0F, 125.0F});
    panel.setFillColor({91, 51, 29, 245});
    panel.setOutlineColor({211, 150, 65});
    panel.setOutlineThickness(4.0F);
    window.draw(panel);

    drawText(window, "设置", 46, {430.0F, 55.0F}, {250, 222, 116});
    drawRow(window, 185.0F, "音乐音量", std::to_string(settings_.musicVolume()) + "%");
    drawRow(window, 255.0F, "音效音量", std::to_string(settings_.effectsVolume()) + "%");
    const WindowResolution resolution = settings_.resolution();
    drawRow(
        window,
        325.0F,
        "分辨率",
        std::to_string(resolution.width) + " x " + std::to_string(resolution.height)
    );
    drawText(window, "显示模式", 24, {220.0F, 404.0F});
    drawButton(window, fullscreenBounds, settings_.fullscreen() ? "全屏" : "窗口");
    drawButton(window, backBounds, "返回");
}

void SettingsScreen::drawRow(
    sf::RenderWindow& window,
    const float y,
    const std::string_view label,
    const std::string& value
) const {
    drawText(window, label, 24, {220.0F, y + 8.0F});
    drawButton(window, {{540.0F, y}, {50.0F, 46.0F}}, "-");
    drawText(window, value, 21, {610.0F, y + 10.0F}, {244, 229, 166});
    drawButton(window, {{760.0F, y}, {50.0F, 46.0F}}, "+");
}

void SettingsScreen::drawButton(
    sf::RenderWindow& window,
    const sf::FloatRect bounds,
    const std::string_view label
) const {
    sf::RectangleShape button(bounds.size);
    if (buttonLoaded_) {
        sf::Sprite themedButton(buttonTexture_);
        const sf::Vector2u source = buttonTexture_.getSize();
        themedButton.setScale({bounds.size.x / source.x, bounds.size.y / source.y});
        themedButton.setPosition(bounds.position);
        window.draw(themedButton);
    } else {
        button.setPosition(bounds.position);
        button.setFillColor({112, 66, 33});
        button.setOutlineColor({226, 198, 111});
        button.setOutlineThickness(2.0F);
        window.draw(button);
    }
    if (fontLoaded_) {
        sf::Text text(font_, ui::utf8(label), 21);
        ui::centerText(text, bounds);
        window.draw(text);
    }
}

void SettingsScreen::drawText(
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
