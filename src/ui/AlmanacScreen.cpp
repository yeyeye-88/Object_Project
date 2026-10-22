#include "ui/AlmanacScreen.hpp"

#include "game/PlantType.hpp"
#include "ui/UiText.hpp"

#include <algorithm>
#include <array>
#include <string>

namespace object_project {

namespace {

constexpr sf::FloatRect plantTab{{65.0F, 525.0F}, {140.0F, 45.0F}};
constexpr sf::FloatRect zombieTab{{220.0F, 525.0F}, {140.0F, 45.0F}};
constexpr sf::FloatRect previousButton{{130.0F, 22.0F}, {100.0F, 58.0F}};
constexpr sf::FloatRect nextButton{{730.0F, 22.0F}, {100.0F, 58.0F}};
constexpr sf::FloatRect backButton{{755.0F, 525.0F}, {140.0F, 45.0F}};

void drawPanel(sf::RenderWindow& window, const sf::FloatRect& bounds, const sf::Color color) {
    sf::RectangleShape panel(bounds.size);
    panel.setPosition(bounds.position);
    panel.setFillColor(color);
    panel.setOutlineColor({231, 211, 135});
    panel.setOutlineThickness(2.0F);
    window.draw(panel);
}

void drawStoneButton(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::FloatRect bounds,
    const bool selected
) {
    const sf::Vector2u source = texture.getSize();
    const unsigned int frameHeight = source.y / 2U;
    sf::Sprite button(texture);
    button.setTextureRect(sf::IntRect(
        {0, selected ? static_cast<int>(frameHeight) : 0},
        {static_cast<int>(source.x), static_cast<int>(frameHeight)}
    ));
    button.setScale({bounds.size.x / source.x, bounds.size.y / frameHeight});
    button.setPosition(bounds.position);
    window.draw(button);
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
    backgroundLoaded_ = backgroundTexture_.loadFromFile("assets/pvz/ui/screens/almanac_index.jpg");
    buttonLoaded_ = buttonTexture_.loadFromFile("assets/pvz/ui/screens/stone_button.png");

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
        const sf::Vector2u source = backgroundTexture_.getSize();
        background.setScale({960.0F / source.x, 600.0F / source.y});
        window.draw(background);
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
        fitCentered(portrait, {{520.0F, 174.0F}, {360.0F, 245.0F}});
        window.draw(portrait);
    }

    drawText(window, entry.name.data(), 30, {385.0F, 28.0F}, {52, 43, 37});
    drawText(window, "类型", 14, {58.0F, 181.0F}, {244, 216, 157});
    drawText(window, entry.role.data(), 20, {58.0F, 202.0F}, {255, 244, 196});
    drawText(window, "基础数据", 14, {58.0F, 234.0F}, {244, 216, 157});
    drawText(window, entry.stats.data(), 18, {58.0F, 255.0F}, {255, 244, 196});
    drawText(window, "能力", 14, {58.0F, 289.0F}, {244, 216, 157});
    drawText(window, entry.ability.data(), 15, {58.0F, 310.0F}, {255, 244, 196});
    drawText(window, "提示", 14, {58.0F, 348.0F}, {244, 216, 157});
    drawText(window, entry.tip.data(), 14, {58.0F, 370.0F}, {255, 244, 196});

    if (buttonLoaded_) {
        drawStoneButton(window, buttonTexture_, plantTab, category_ == Category::Plants);
        drawStoneButton(window, buttonTexture_, zombieTab, category_ == Category::Zombies);
        drawStoneButton(window, buttonTexture_, backButton, false);
    } else {
        drawPanel(window, plantTab, {77, 48, 27});
        drawPanel(window, zombieTab, {77, 48, 27});
        drawPanel(window, backButton, {91, 51, 29});
    }
    drawText(window, "植物", 21, {111.0F, 533.0F});
    drawText(window, "僵尸", 21, {266.0F, 533.0F});
    drawText(window, "返回", 21, {801.0F, 533.0F});
    drawText(window, "‹", 46, {168.0F, 24.0F}, {246, 231, 190});
    drawText(window, "›", 46, {768.0F, 24.0F}, {246, 231, 190});
    drawText(
        window,
        std::to_string(selectedIndex_ + 1) + " / " + std::to_string(entryCount()),
        16,
        {455.0F, 538.0F},
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
