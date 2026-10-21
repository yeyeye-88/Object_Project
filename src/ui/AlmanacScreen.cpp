#include "ui/AlmanacScreen.hpp"

#include "game/PlantType.hpp"
#include "ui/UiText.hpp"

#include <algorithm>
#include <array>
#include <string>

namespace object_project {

namespace {

constexpr sf::FloatRect plantTab{{90.0F, 515.0F}, {145.0F, 48.0F}};
constexpr sf::FloatRect zombieTab{{250.0F, 515.0F}, {145.0F, 48.0F}};
constexpr sf::FloatRect previousButton{{155.0F, 35.0F}, {115.0F, 62.0F}};
constexpr sf::FloatRect nextButton{{690.0F, 35.0F}, {115.0F, 62.0F}};
constexpr sf::FloatRect backButton{{725.0F, 515.0F}, {145.0F, 48.0F}};

void drawPanel(sf::RenderWindow& window, const sf::FloatRect& bounds, const sf::Color color) {
    sf::RectangleShape panel(bounds.size);
    panel.setPosition(bounds.position);
    panel.setFillColor(color);
    panel.setOutlineColor({231, 211, 135});
    panel.setOutlineThickness(2.0F);
    window.draw(panel);
}

void fitCentered(sf::Sprite& sprite, const sf::FloatRect bounds) {
    const sf::Vector2u source = sprite.getTexture().getSize();
    const float scale = std::min(
        bounds.size.x / static_cast<float>(source.x),
        bounds.size.y / static_cast<float>(source.y)
    );
    sprite.setScale({scale, scale});
    const sf::FloatRect local = sprite.getLocalBounds();
    sprite.setPosition({
        bounds.position.x + (bounds.size.x - local.size.x * scale) * 0.5F,
        bounds.position.y + (bounds.size.y - local.size.y * scale) * 0.5F,
    });
}

}  // namespace

AlmanacScreen::AlmanacScreen() {
    fontLoaded_ = ui::loadChineseFont(font_);
    backgroundLoaded_ = backgroundTexture_.loadFromFile("assets/background/lawn.jpg");
    panelLoaded_ = panelTexture_.loadFromFile("assets/pvz/ui/reference/almanac_panel.png");
    buttonLoaded_ = buttonTexture_.loadFromFile("assets/pvz/ui/reference/wood_button.png");

    const std::array<std::string, plantTypeCount> plantPaths{
        "assets/pvz/plants/sunflower.png",
        "assets/pvz/plants/peashooter.png",
        "assets/pvz/plants/wallnut.png",
        "assets/pvz/plants/repeater.png",
        "assets/pvz/plants/threepeater_head.png",
        "assets/pvz/plants/snowpea.png",
        "assets/pvz/plants/tallnut.png",
        "assets/pvz/plants/potato_mine.png",
        "assets/pvz/plants/cherrybomb.png",
        "assets/pvz/plants/torchwood_body.png",
    };
    const std::array<std::string, zombieTypeCount> zombiePaths{
        "assets/pvz/zombies/normal.png",
        "assets/pvz/zombies/conehead.png",
        "assets/pvz/zombies/buckethead.png",
        "assets/pvz/zombies/flag.png",
        "assets/pvz/zombies/pole_vaulting.gif",
        "assets/pvz/zombies/newspaper.gif",
    };
    for (std::size_t index = 0; index < plantTextures_.size(); ++index) {
        plantLoaded_[index] = plantTextures_[index].loadFromFile(plantPaths[index]);
    }
    for (std::size_t index = 0; index < zombieTextures_.size(); ++index) {
        zombieLoaded_[index] = zombieTextures_[index].loadFromFile(zombiePaths[index]);
    }
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
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        window.draw(background);
    }
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({19, 35, 20, 115});
    window.draw(shade);
    if (panelLoaded_) {
        sf::Sprite panel(panelTexture_);
        panel.setScale({1.75F, 1.75F});
        panel.setPosition({60.0F, 20.0F});
        window.draw(panel);
    }

    const AlmanacEntry& entry = selectedEntry();
    const sf::Texture* portraitTexture = nullptr;
    if (category_ == Category::Plants && plantLoaded_[selectedIndex_]) {
        portraitTexture = &plantTextures_[selectedIndex_];
    } else if (category_ == Category::Zombies && zombieLoaded_[selectedIndex_]) {
        portraitTexture = &zombieTextures_[selectedIndex_];
    }
    if (portraitTexture) {
        sf::Sprite portrait(*portraitTexture);
        fitCentered(portrait, {{330.0F, 112.0F}, {300.0F, 178.0F}});
        window.draw(portrait);
    }

    drawText(window, entry.name.data(), 28, {333.0F, 338.0F}, {74, 38, 17});
    drawText(window, entry.role.data(), 18, {335.0F, 377.0F}, {115, 59, 24});
    drawText(window, entry.stats.data(), 17, {335.0F, 407.0F}, {58, 42, 25});
    drawText(window, entry.ability.data(), 15, {335.0F, 437.0F}, {58, 42, 25});
    drawText(window, entry.tip.data(), 14, {335.0F, 469.0F}, {95, 61, 30});

    drawPanel(window, plantTab, category_ == Category::Plants ? sf::Color(120, 73, 35) : sf::Color(77, 48, 27));
    drawPanel(window, zombieTab, category_ == Category::Zombies ? sf::Color(120, 73, 35) : sf::Color(77, 48, 27));
    drawPanel(window, backButton, {91, 51, 29});
    drawText(window, "植物", 22, {138.0F, 524.0F});
    drawText(window, "僵尸", 22, {298.0F, 524.0F});
    drawText(window, "返回", 22, {773.0F, 524.0F});
    drawText(window, "‹", 46, {197.0F, 38.0F}, {246, 231, 190});
    drawText(window, "›", 46, {733.0F, 38.0F}, {246, 231, 190});
    drawText(
        window,
        std::to_string(selectedIndex_ + 1) + " / " + std::to_string(entryCount()),
        16,
        {455.0F, 527.0F},
        {92, 55, 27}
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
    sf::Text text(font_, ui::utf8(value), size);
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
