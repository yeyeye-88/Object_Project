#include "ui/AlmanacScreen.hpp"

#include "game/PlantType.hpp"

#include <algorithm>
#include <string>

namespace object_project {

namespace {

constexpr sf::FloatRect plantTab{{80.0F, 85.0F}, {180.0F, 50.0F}};
constexpr sf::FloatRect zombieTab{{280.0F, 85.0F}, {180.0F, 50.0F}};
constexpr sf::FloatRect previousButton{{80.0F, 500.0F}, {100.0F, 50.0F}};
constexpr sf::FloatRect nextButton{{200.0F, 500.0F}, {100.0F, 50.0F}};
constexpr sf::FloatRect backButton{{780.0F, 500.0F}, {100.0F, 50.0F}};

void drawPanel(sf::RenderWindow& window, const sf::FloatRect& bounds, const sf::Color color) {
    sf::RectangleShape panel(bounds.size);
    panel.setPosition(bounds.position);
    panel.setFillColor(color);
    panel.setOutlineColor({231, 211, 135});
    panel.setOutlineThickness(2.0F);
    window.draw(panel);
}

}  // namespace

AlmanacScreen::AlmanacScreen() {
    fontLoaded_ = font_.openFromFile("assets/fonts/tuffy.ttf");
}

bool AlmanacScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Escape) {
            return true;
        }
        if (key->scancode == sf::Keyboard::Scancode::Left && selectedIndex_ > 0) {
            --selectedIndex_;
        } else if (key->scancode == sf::Keyboard::Scancode::Right &&
                   selectedIndex_ + 1 < entryCount()) {
            ++selectedIndex_;
        }
    }

    const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
    if (click == nullptr || click->button != sf::Mouse::Button::Left) {
        return false;
    }
    const sf::Vector2f position = window.mapPixelToCoords(click->position);
    if (backButton.contains(position)) {
        return true;
    }
    if (plantTab.contains(position)) {
        category_ = Category::Plants;
        selectedIndex_ = 0;
    } else if (zombieTab.contains(position)) {
        category_ = Category::Zombies;
        selectedIndex_ = 0;
    } else if (previousButton.contains(position) && selectedIndex_ > 0) {
        --selectedIndex_;
    } else if (nextButton.contains(position) && selectedIndex_ + 1 < entryCount()) {
        ++selectedIndex_;
    }
    return false;
}

void AlmanacScreen::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background({960.0F, 600.0F});
    background.setFillColor({31, 56, 35});
    window.draw(background);

    drawText(window, "ALMANAC", 42, {70.0F, 22.0F}, {255, 232, 145});
    drawPanel(window, plantTab, category_ == Category::Plants ? sf::Color(88, 139, 67) : sf::Color(61, 83, 58));
    drawPanel(window, zombieTab, category_ == Category::Zombies ? sf::Color(125, 103, 67) : sf::Color(61, 83, 58));
    drawText(window, "PLANTS", 24, {119.0F, 96.0F});
    drawText(window, "ZOMBIES", 24, {311.0F, 96.0F});

    drawPanel(window, {{80.0F, 160.0F}, {800.0F, 310.0F}}, {47, 72, 48});
    const AlmanacEntry& entry = selectedEntry();
    drawText(window, entry.name.data(), 36, {115.0F, 188.0F}, {255, 224, 115});
    drawText(window, entry.role.data(), 22, {116.0F, 244.0F}, {174, 220, 153});
    drawText(window, entry.stats.data(), 22, {116.0F, 292.0F});
    drawText(window, entry.ability.data(), 20, {116.0F, 346.0F});
    drawText(window, entry.tip.data(), 18, {116.0F, 405.0F}, {225, 215, 173});

    drawPanel(window, previousButton, {68, 91, 61});
    drawPanel(window, nextButton, {68, 91, 61});
    drawPanel(window, backButton, {115, 73, 48});
    drawText(window, "PREV", 20, {103.0F, 512.0F});
    drawText(window, "NEXT", 20, {221.0F, 512.0F});
    drawText(window, "BACK", 20, {802.0F, 512.0F});
    drawText(
        window,
        std::to_string(selectedIndex_ + 1) + " / " + std::to_string(entryCount()),
        18,
        {430.0F, 515.0F},
        {230, 220, 170}
    );
}

void AlmanacScreen::drawText(
    sf::RenderWindow& window,
    const std::string& value,
    const unsigned int size,
    const sf::Vector2f position,
    const sf::Color color
) const {
    if (!fontLoaded_) {
        return;
    }
    sf::Text text(font_, value, size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}

const AlmanacEntry& AlmanacScreen::selectedEntry() const {
    if (category_ == Category::Plants) {
        return plantAlmanacEntry(static_cast<PlantType>(selectedIndex_));
    }
    return zombieAlmanacEntry(static_cast<ZombieType>(selectedIndex_));
}

std::size_t AlmanacScreen::entryCount() const {
    return category_ == Category::Plants ? plantTypeCount : zombieTypeCount;
}

}  // namespace object_project
