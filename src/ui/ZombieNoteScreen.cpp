#include "ui/ZombieNoteScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <string>

namespace object_project {

namespace {

constexpr sf::FloatRect continueBounds{{390.0F, 520.0F}, {180.0F, 46.0F}};

}  // namespace

ZombieNoteScreen::ZombieNoteScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      lawnLoaded_(lawnTexture_.loadFromFile("assets/background/lawn.jpg")),
      buttonLoaded_(buttonTexture_.loadFromFile(
          "assets/pvz/ui/screens/small_button.png"
      )) {
    for (std::size_t index = 0; index < noteTextures_.size(); ++index) {
        noteLoaded_[index] = noteTextures_[index].loadFromFile(
            "assets/pvz/ui/reference/zombie_note_" + std::to_string(index + 1) + ".png"
        );
    }
}

void ZombieNoteScreen::setLevel(const LevelId level) {
    level_ = level;
}

bool ZombieNoteScreen::handleEvent(const sf::Event& event, const sf::RenderWindow& window) const {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        return key->scancode == sf::Keyboard::Scancode::Enter ||
               key->scancode == sf::Keyboard::Scancode::Space;
    }
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        return click->button == sf::Mouse::Button::Left &&
               continueBounds.contains(window.mapPixelToCoords(click->position));
    }
    return false;
}

void ZombieNoteScreen::draw(sf::RenderWindow& window) const {
    if (lawnLoaded_) {
        sf::Sprite lawn(lawnTexture_);
        const sf::Vector2u source = lawnTexture_.getSize();
        lawn.setScale({960.0F / source.x, 600.0F / source.y});
        window.draw(lawn);
    }
    sf::RectangleShape dim({960.0F, 600.0F});
    dim.setFillColor({10, 25, 14, 75});
    window.draw(dim);

    const std::size_t noteIndex = std::min<std::size_t>(
        static_cast<std::size_t>(level_), noteTextures_.size() - 1
    );
    if (noteLoaded_[noteIndex]) {
        sf::Sprite note(noteTextures_[noteIndex]);
        note.setPosition({173.0F, 76.0F});
        window.draw(note);
    }

    if (buttonLoaded_) {
        sf::Sprite button(buttonTexture_);
        const sf::Vector2u source = buttonTexture_.getSize();
        button.setScale({
            continueBounds.size.x / static_cast<float>(source.x),
            continueBounds.size.y / static_cast<float>(source.y),
        });
        button.setPosition(continueBounds.position);
        window.draw(button);
    }
    if (fontLoaded_) {
        sf::Text label(font_, ui::utf8("开始关卡"), 22);
        label.setFillColor({238, 234, 206});
        ui::centerText(label, continueBounds);
        window.draw(label);
    }
}

}  // namespace object_project
