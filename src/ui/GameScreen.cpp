#include "ui/GameScreen.hpp"

#include "game/BoardGeometry.hpp"

#include <algorithm>
#include <array>
#include <string>

namespace object_project {

namespace {

bool contains(const sf::FloatRect& rectangle, const sf::Vector2f position) {
    return rectangle.contains(position);
}

void drawButton(
    sf::RenderWindow& window,
    const sf::FloatRect& bounds,
    const sf::Color fill
) {
    sf::RectangleShape button(bounds.size);
    button.setPosition(bounds.position);
    button.setFillColor(fill);
    button.setOutlineColor(sf::Color(244, 232, 177));
    button.setOutlineThickness(2.0F);
    window.draw(button);
}

void fitSprite(sf::Sprite& sprite, const sf::Vector2f targetSize) {
    const sf::Vector2u sourceSize = sprite.getTexture().getSize();
    if (sourceSize.x == 0 || sourceSize.y == 0) {
        return;
    }
    const float scale = std::min(
        targetSize.x / static_cast<float>(sourceSize.x),
        targetSize.y / static_cast<float>(sourceSize.y)
    );
    sprite.setScale({scale, scale});
}

}  // namespace

GameScreen::GameScreen(GameSession& session) : session_(session) {
    backgroundLoaded_ = backgroundTexture_.loadFromFile("assets/background/lawn.jpg");

    const std::array<std::string, plantTypeCount> cardPaths{
        "assets/cards/sunflower.png",
        "assets/cards/peashooter.png",
        "assets/cards/wallnut.png",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
    };
    const std::array<std::string, plantTypeCount> plantPaths{
        "assets/plants/sunflower/idle.png",
        "assets/plants/peashooter/idle.png",
        "assets/plants/wallnut/idle.png",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
    };

    for (std::size_t index = 0; index < plantTypeCount; ++index) {
        cardLoaded_[index] = !cardPaths[index].empty() &&
                             cardTextures_[index].loadFromFile(cardPaths[index]);
        plantLoaded_[index] = !plantPaths[index].empty() &&
                              plantTextures_[index].loadFromFile(plantPaths[index]);
    }

    sunLoaded_ = sunTexture_.loadFromFile("assets/sun/sun.png");
    shovelLoaded_ = shovelTexture_.loadFromFile("assets/ui/shovel.png");
}

const std::array<GameScreen::CardLayout, plantTypeCount>& GameScreen::cardLayouts() {
    static const std::array<CardLayout, plantTypeCount> layouts{{
        {PlantType::Sunflower, {78.0F, 8.0F}},
        {PlantType::Peashooter, {128.0F, 8.0F}},
        {PlantType::WallNut, {178.0F, 8.0F}},
        {PlantType::Repeater, {228.0F, 8.0F}},
        {PlantType::Threepeater, {278.0F, 8.0F}},
        {PlantType::SnowPea, {328.0F, 8.0F}},
        {PlantType::TallNut, {378.0F, 8.0F}},
        {PlantType::PotatoMine, {428.0F, 8.0F}},
        {PlantType::CherryBomb, {478.0F, 8.0F}},
        {PlantType::Torchwood, {528.0F, 8.0F}},
    }};
    return layouts;
}

sf::Color GameScreen::plantColor(const PlantType type, const std::uint8_t alpha) {
    switch (type) {
        case PlantType::Sunflower:
            return {249, 205, 55, alpha};
        case PlantType::Peashooter:
            return {74, 178, 75, alpha};
        case PlantType::WallNut:
            return {155, 95, 48, alpha};
        case PlantType::Repeater:
            return {40, 140, 70, alpha};
        case PlantType::Threepeater:
            return {80, 190, 120, alpha};
        case PlantType::SnowPea:
            return {92, 190, 225, alpha};
        case PlantType::TallNut:
            return {116, 72, 43, alpha};
        case PlantType::PotatoMine:
            return {170, 123, 62, alpha};
        case PlantType::CherryBomb:
            return {210, 48, 55, alpha};
        case PlantType::Torchwood:
            return {218, 112, 35, alpha};
        case PlantType::Count:
            break;
    }
    return {220, 220, 220, alpha};
}

bool GameScreen::takeAlmanacRequest() {
    const bool requested = almanacRequested_;
    almanacRequested_ = false;
    return requested;
}

void GameScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Escape) {
            session_.togglePause();
        } else if (key->scancode == sf::Keyboard::Scancode::R) {
            session_.restart();
            selectedPlant_.reset();
            shovelSelected_ = false;
        }
        return;
    }

    if (const auto* move = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f worldPosition = window.mapPixelToCoords(move->position);
        hoveredCell_ = BoardGeometry::gridAt(
            worldPosition.x,
            worldPosition.y
        );
        return;
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Right) {
            selectedPlant_.reset();
            shovelSelected_ = false;
        } else if (click->button == sf::Mouse::Button::Left) {
            handleLeftClick(window.mapPixelToCoords(click->position));
        }
    }
}

void GameScreen::handleLeftClick(const sf::Vector2f position) {
    if (contains(almanacBounds, position)) {
        almanacRequested_ = true;
        selectedPlant_.reset();
        shovelSelected_ = false;
        return;
    }

    if (contains(pauseBounds, position)) {
        session_.togglePause();
        return;
    }

    if (session_.isPaused()) {
        if (contains(resumeBounds, position)) {
            session_.togglePause();
        } else if (contains(restartBounds, position)) {
            session_.restart();
            selectedPlant_.reset();
            shovelSelected_ = false;
        }
        return;
    }

    if (session_.state() == GameState::Won || session_.state() == GameState::Lost) {
        if (contains(restartBounds, position)) {
            session_.restart();
            selectedPlant_.reset();
            shovelSelected_ = false;
        }
        return;
    }

    if (session_.collectSunAt(static_cast<float>(position.x), static_cast<float>(position.y))) {
        return;
    }

    for (const CardLayout& layout : cardLayouts()) {
        if (contains({layout.position, cardSize}, position)) {
            selectedPlant_ = layout.type;
            shovelSelected_ = false;
            return;
        }
    }

    if (contains(shovelBounds, position)) {
        shovelSelected_ = !shovelSelected_;
        if (shovelSelected_) {
            selectedPlant_.reset();
        }
        return;
    }

    const auto cell = BoardGeometry::gridAt(
        position.x,
        position.y
    );
    if (!cell) {
        return;
    }

    if (shovelSelected_) {
        session_.removePlant(*cell);
        shovelSelected_ = false;
        return;
    }

    if (selectedPlant_ &&
        session_.tryPlace(*selectedPlant_, *cell) == PlacementResult::Allowed) {
        selectedPlant_.reset();
    }
}

void GameScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window);
    drawBoard(window);
    drawCombat(window);
    drawSuns(window);
    drawCards(window);
    drawToolbar(window);
    if (session_.isPaused()) {
        drawPauseOverlay(window);
    } else if (session_.state() == GameState::Won || session_.state() == GameState::Lost) {
        drawResultOverlay(window);
    }
}

void GameScreen::drawBackground(sf::RenderWindow& window) const {
    if (backgroundLoaded_) {
        sf::Sprite background(backgroundTexture_);
        window.draw(background);
        return;
    }

    sf::RectangleShape fallback({960.0F, 600.0F});
    fallback.setFillColor({42, 103, 44});
    window.draw(fallback);
}

void GameScreen::drawBoard(sf::RenderWindow& window) const {
    for (int row = 0; row < BoardGeometry::rows; ++row) {
        for (int column = 0; column < BoardGeometry::columns; ++column) {
            const GridPosition position{row, column};
            if (const Plant* plant = session_.board().plantAt(position)) {
                drawPlant(window, plant->type(), position, false);
                drawHealthBar(
                    window,
                    static_cast<float>(plant->health()) /
                        static_cast<float>(plant->maxHealth()),
                    {BoardGeometry::cellLeft(column) + 10.0F,
                     BoardGeometry::cellTop(row) + BoardGeometry::cellHeight - 10.0F},
                    BoardGeometry::cellWidth - 20.0F
                );
            }
        }
    }

    if (!hoveredCell_ || session_.isPaused()) {
        return;
    }

    sf::RectangleShape highlight({BoardGeometry::cellWidth - 4.0F, BoardGeometry::cellHeight - 4.0F});
    highlight.setPosition({
        BoardGeometry::cellLeft(hoveredCell_->column) + 2.0F,
        BoardGeometry::cellTop(hoveredCell_->row) + 2.0F,
    });

    bool allowed = true;
    if (selectedPlant_) {
        allowed = session_.placementStatus(*selectedPlant_, *hoveredCell_) == PlacementResult::Allowed;
        drawPlant(window, *selectedPlant_, *hoveredCell_, true);
    } else if (shovelSelected_) {
        allowed = session_.board().isOccupied(*hoveredCell_);
    }
    highlight.setFillColor(allowed ? sf::Color(100, 255, 110, 55) : sf::Color(255, 70, 70, 75));
    highlight.setOutlineColor(allowed ? sf::Color(190, 255, 190) : sf::Color(255, 150, 150));
    highlight.setOutlineThickness(2.0F);
    window.draw(highlight);
}

void GameScreen::drawCombat(sf::RenderWindow& window) const {
    for (const LawnMower& mower : session_.combat().mowers()) {
        if (mower.state == LawnMowerState::Used) {
            const float laneTop = BoardGeometry::cellTop(mower.row);

            sf::RectangleShape emptyBay({48.0F, 34.0F});
            emptyBay.setPosition({CombatSystem::mowerStartX, laneTop + 54.0F});
            emptyBay.setFillColor({42, 42, 38, 125});
            emptyBay.setOutlineColor({155, 58, 45, 220});
            emptyBay.setOutlineThickness(3.0F);
            window.draw(emptyBay);

            for (const float yOffset : {66.0F, 82.0F}) {
                for (int column = 0; column < BoardGeometry::columns; ++column) {
                    sf::RectangleShape track({BoardGeometry::cellWidth * 0.62F, 4.0F});
                    track.setPosition({
                        BoardGeometry::cellLeft(column) + BoardGeometry::cellWidth * 0.18F,
                        laneTop + yOffset,
                    });
                    track.setFillColor({25, 30, 22, 72});
                    window.draw(track);
                }
            }

            sf::ConvexShape warning(3);
            warning.setPoint(0, {CombatSystem::mowerStartX + 24.0F, laneTop + 8.0F});
            warning.setPoint(1, {CombatSystem::mowerStartX + 7.0F, laneTop + 42.0F});
            warning.setPoint(2, {CombatSystem::mowerStartX + 41.0F, laneTop + 42.0F});
            warning.setFillColor({205, 55, 42, 235});
            warning.setOutlineColor({255, 214, 118});
            warning.setOutlineThickness(2.0F);
            window.draw(warning);

            sf::RectangleShape warningStem({4.0F, 15.0F});
            warningStem.setPosition({CombatSystem::mowerStartX + 22.0F, laneTop + 18.0F});
            warningStem.setFillColor(sf::Color::White);
            window.draw(warningStem);
            sf::CircleShape warningDot(2.5F);
            warningDot.setPosition({CombatSystem::mowerStartX + 21.5F, laneTop + 35.0F});
            warningDot.setFillColor(sf::Color::White);
            window.draw(warningDot);
            continue;
        }
        sf::RectangleShape body({48.0F, 34.0F});
        body.setPosition({mower.x, BoardGeometry::cellTop(mower.row) + 54.0F});
        body.setFillColor({202, 48, 42});
        body.setOutlineColor({88, 25, 23});
        body.setOutlineThickness(2.0F);
        window.draw(body);
        for (const float offset : {5.0F, 31.0F}) {
            sf::CircleShape wheel(7.0F);
            wheel.setPosition({mower.x + offset, BoardGeometry::cellTop(mower.row) + 80.0F});
            wheel.setFillColor({45, 45, 45});
            window.draw(wheel);
        }
    }

    for (const Projectile& projectile : session_.combat().projectiles()) {
        sf::CircleShape pea(7.0F);
        pea.setPosition({projectile.x, BoardGeometry::cellTop(projectile.row) + 39.0F});
        if (projectile.type == Projectile::Type::FrozenPea) {
            pea.setFillColor({104, 205, 245});
        } else if (projectile.type == Projectile::Type::FirePea) {
            pea.setFillColor({245, 118, 42});
        } else {
            pea.setFillColor({92, 220, 65});
        }
        pea.setOutlineColor({35, 105, 28});
        pea.setOutlineThickness(2.0F);
        window.draw(pea);
    }

    for (const auto& zombie : session_.combat().zombies()) {
        const float top = BoardGeometry::cellTop(zombie->row()) + 10.0F;
        sf::RectangleShape body({CombatSystem::zombieWidth, 80.0F});
        body.setPosition({zombie->x(), top});
        sf::Color bodyColor{105, 125, 91};
        if (zombie->type() == ZombieType::Conehead) {
            bodyColor = {152, 118, 65};
        } else if (zombie->type() == ZombieType::Buckethead) {
            bodyColor = {98, 111, 116};
        }
        if (zombie->isSlowed()) {
            bodyColor = sf::Color(
                static_cast<std::uint8_t>(bodyColor.r * 0.7F),
                static_cast<std::uint8_t>(bodyColor.g * 0.85F),
                190
            );
        }
        body.setFillColor(bodyColor);
        body.setOutlineColor({48, 61, 42});
        body.setOutlineThickness(3.0F);
        window.draw(body);

        sf::CircleShape head(17.0F);
        head.setPosition({zombie->x() + 7.0F, top - 8.0F});
        head.setFillColor({150, 165, 130});
        window.draw(head);
        drawHealthBar(
            window,
            static_cast<float>(zombie->health()) /
                static_cast<float>(zombie->maxHealth()),
            {zombie->x(), top - 14.0F},
            CombatSystem::zombieWidth
        );
        if (zombie->maxArmor() > 0) {
            drawHealthBar(
                window,
                static_cast<float>(zombie->armor()) /
                    static_cast<float>(zombie->maxArmor()),
                {zombie->x(), top - 7.0F},
                CombatSystem::zombieWidth
            );
        }
    }
}

void GameScreen::drawPlant(
    sf::RenderWindow& window,
    const PlantType type,
    const GridPosition position,
    const bool preview
) const {
    const std::size_t index = toIndex(type);
    const sf::Vector2f targetPosition{
        BoardGeometry::cellLeft(position.column) + 5.0F,
        BoardGeometry::cellTop(position.row) + 5.0F,
    };

    if (plantLoaded_[index]) {
        sf::Sprite sprite(plantTextures_[index]);
        fitSprite(sprite, {BoardGeometry::cellWidth - 10.0F, BoardGeometry::cellHeight - 10.0F});
        sprite.setPosition(targetPosition);
        if (preview) {
            sprite.setColor({255, 255, 255, 145});
        }
        window.draw(sprite);
        return;
    }

    sf::CircleShape fallback(28.0F);
    fallback.setPosition({targetPosition.x + 8.0F, targetPosition.y + 12.0F});
    fallback.setFillColor(plantColor(type, preview ? 145 : 255));
    window.draw(fallback);
}

void GameScreen::drawSuns(sf::RenderWindow& window) const {
    for (const SunToken& token : session_.suns().tokens()) {
        if (sunLoaded_) {
            sf::Sprite sprite(sunTexture_);
            fitSprite(sprite, {SunSystem::tokenSize, SunSystem::tokenSize});
            sprite.setPosition({token.x, token.y});
            window.draw(sprite);
        } else {
            sf::CircleShape sun(SunSystem::tokenSize / 2.0F);
            sun.setPosition({token.x, token.y});
            sun.setFillColor({255, 215, 50});
            window.draw(sun);
        }
    }
}

void GameScreen::drawCards(sf::RenderWindow& window) const {
    for (const CardLayout& layout : cardLayouts()) {
        const std::size_t index = toIndex(layout.type);
        if (cardLoaded_[index]) {
            sf::Sprite card(cardTextures_[index]);
            const sf::Vector2u source = cardTextures_[index].getSize();
            card.setScale({
                cardSize.x / static_cast<float>(source.x),
                cardSize.y / static_cast<float>(source.y),
            });
            card.setPosition(layout.position);
            window.draw(card);
        } else {
            sf::RectangleShape fallback(cardSize);
            fallback.setPosition(layout.position);
            fallback.setFillColor(plantColor(layout.type));
            window.draw(fallback);
        }

        const float cooldown = session_.cards().cooldownProgress(layout.type);
        const bool unaffordable = session_.sun() < session_.cards().definition(layout.type).cost;
        if (cooldown > 0.0F || unaffordable) {
            const float maskHeight = cooldown > 0.0F ? cardSize.y * cooldown : cardSize.y;
            sf::RectangleShape mask({cardSize.x, maskHeight});
            mask.setPosition(layout.position);
            mask.setFillColor({
                20,
                20,
                20,
                static_cast<std::uint8_t>(unaffordable ? 155 : 115),
            });
            window.draw(mask);
        }

        if (selectedPlant_ == layout.type) {
            sf::RectangleShape selected(cardSize);
            selected.setPosition(layout.position);
            selected.setFillColor(sf::Color::Transparent);
            selected.setOutlineColor({255, 244, 94});
            selected.setOutlineThickness(3.0F);
            window.draw(selected);
        }
    }
}

void GameScreen::drawToolbar(sf::RenderWindow& window) const {
    sf::RectangleShape sunPanel({66.0F, 90.0F});
    sunPanel.setPosition({8.0F, 4.0F});
    sunPanel.setFillColor({86, 57, 30, 210});
    sunPanel.setOutlineColor({229, 198, 113});
    sunPanel.setOutlineThickness(2.0F);
    window.draw(sunPanel);

    if (sunLoaded_) {
        sf::Sprite icon(sunTexture_);
        fitSprite(icon, {45.0F, 45.0F});
        icon.setPosition({18.0F, 8.0F});
        window.draw(icon);
    }
    drawNumber(window, session_.sun(), {16.0F, 62.0F});

    drawButton(window, shovelBounds, shovelSelected_ ? sf::Color(185, 150, 54) : sf::Color(85, 62, 37));
    if (shovelLoaded_) {
        sf::Sprite shovel(shovelTexture_);
        fitSprite(shovel, {52.0F, 52.0F});
        shovel.setPosition({shovelBounds.position.x + 5.0F, shovelBounds.position.y + 4.0F});
        window.draw(shovel);
    }

    drawButton(window, almanacBounds, {73, 91, 55, 225});
    sf::RectangleShape bookLeft({25.0F, 26.0F});
    bookLeft.setPosition({799.0F, 26.0F});
    bookLeft.setFillColor({238, 222, 160});
    bookLeft.setOutlineColor({77, 52, 31});
    bookLeft.setOutlineThickness(2.0F);
    window.draw(bookLeft);
    sf::RectangleShape bookRight({25.0F, 26.0F});
    bookRight.setPosition({826.0F, 26.0F});
    bookRight.setFillColor({238, 222, 160});
    bookRight.setOutlineColor({77, 52, 31});
    bookRight.setOutlineThickness(2.0F);
    window.draw(bookRight);

    sf::RectangleShape wavePanel({88.0F, 42.0F});
    wavePanel.setPosition({680.0F, 18.0F});
    wavePanel.setFillColor(session_.level().finalWaveStarted()
                               ? sf::Color(145, 52, 38, 225)
                               : sf::Color(58, 80, 43, 225));
    wavePanel.setOutlineColor({229, 198, 113});
    wavePanel.setOutlineThickness(2.0F);
    window.draw(wavePanel);
    drawNumber(window, session_.level().currentWave(), {696.0F, 28.0F});
    sf::RectangleShape separator({12.0F, 2.0F});
    separator.setPosition({720.0F, 39.0F});
    separator.setRotation(sf::degrees(-55.0F));
    separator.setFillColor({255, 236, 125});
    window.draw(separator);
    drawNumber(window, LevelSystem::waveCount(), {742.0F, 28.0F});

    drawButton(window, pauseBounds, {58, 80, 43, 225});
    if (session_.isPaused()) {
        sf::ConvexShape play(3);
        play.setPoint(0, {891.0F, 27.0F});
        play.setPoint(1, {891.0F, 51.0F});
        play.setPoint(2, {913.0F, 39.0F});
        play.setFillColor(sf::Color::White);
        window.draw(play);
    } else {
        for (float x : {891.0F, 908.0F}) {
            sf::RectangleShape bar({7.0F, 24.0F});
            bar.setPosition({x, 27.0F});
            bar.setFillColor(sf::Color::White);
            window.draw(bar);
        }
    }
}

void GameScreen::drawPauseOverlay(sf::RenderWindow& window) const {
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor({10, 20, 12, 165});
    window.draw(shade);

    drawButton(window, resumeBounds, {62, 128, 67, 245});
    sf::ConvexShape play(3);
    play.setPoint(0, {402.0F, 274.0F});
    play.setPoint(1, {402.0F, 306.0F});
    play.setPoint(2, {432.0F, 290.0F});
    play.setFillColor(sf::Color::White);
    window.draw(play);

    drawButton(window, restartBounds, {133, 91, 46, 245});
    sf::CircleShape restart(17.0F, 28);
    restart.setPosition({528.0F, 273.0F});
    restart.setFillColor(sf::Color::Transparent);
    restart.setOutlineColor(sf::Color::White);
    restart.setOutlineThickness(5.0F);
    window.draw(restart);
    sf::RectangleShape cut({18.0F, 12.0F});
    cut.setPosition({526.0F, 269.0F});
    cut.setFillColor({133, 91, 46});
    window.draw(cut);
    sf::ConvexShape arrow(3);
    arrow.setPoint(0, {523.0F, 280.0F});
    arrow.setPoint(1, {540.0F, 273.0F});
    arrow.setPoint(2, {538.0F, 290.0F});
    arrow.setFillColor(sf::Color::White);
    window.draw(arrow);
}

void GameScreen::drawResultOverlay(sf::RenderWindow& window) const {
    const bool won = session_.state() == GameState::Won;
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor(won ? sf::Color(18, 75, 32, 190) : sf::Color(85, 20, 18, 195));
    window.draw(shade);

    if (won) {
        sf::RectangleShape first({18.0F, 65.0F});
        first.setPosition({405.0F, 180.0F});
        first.setRotation(sf::degrees(-42.0F));
        first.setFillColor({245, 240, 150});
        window.draw(first);
        sf::RectangleShape second({18.0F, 115.0F});
        second.setPosition({452.0F, 228.0F});
        second.setRotation(sf::degrees(-135.0F));
        second.setFillColor({245, 240, 150});
        window.draw(second);
    } else {
        for (const float angle : {45.0F, -45.0F}) {
            sf::RectangleShape bar({20.0F, 120.0F});
            bar.setPosition({474.0F, 175.0F});
            bar.setOrigin({10.0F, 60.0F});
            bar.setRotation(sf::degrees(angle));
            bar.setFillColor({255, 210, 170});
            window.draw(bar);
        }
    }

    drawButton(window, restartBounds, won ? sf::Color(62, 128, 67, 245) : sf::Color(133, 91, 46, 245));
    sf::CircleShape restart(17.0F, 28);
    restart.setPosition({528.0F, 273.0F});
    restart.setFillColor(sf::Color::Transparent);
    restart.setOutlineColor(sf::Color::White);
    restart.setOutlineThickness(5.0F);
    window.draw(restart);
}

void GameScreen::drawHealthBar(
    sf::RenderWindow& window,
    const float ratio,
    const sf::Vector2f position,
    const float width
) const {
    sf::RectangleShape background({width, 5.0F});
    background.setPosition(position);
    background.setFillColor({55, 25, 22, 210});
    window.draw(background);

    sf::RectangleShape health({width * std::clamp(ratio, 0.0F, 1.0F), 5.0F});
    health.setPosition(position);
    health.setFillColor(ratio > 0.35F ? sf::Color(80, 210, 70) : sf::Color(230, 65, 45));
    window.draw(health);
}

void GameScreen::drawNumber(
    sf::RenderWindow& window,
    const int value,
    const sf::Vector2f position
) const {
    static constexpr std::array<std::array<bool, 7>, 10> digits{{
        {{true, true, true, true, true, true, false}},
        {{false, true, true, false, false, false, false}},
        {{true, true, false, true, true, false, true}},
        {{true, true, true, true, false, false, true}},
        {{false, true, true, false, false, true, true}},
        {{true, false, true, true, false, true, true}},
        {{true, false, true, true, true, true, true}},
        {{true, true, true, false, false, false, false}},
        {{true, true, true, true, true, true, true}},
        {{true, true, true, true, false, true, true}},
    }};

    const std::string text = std::to_string(std::max(0, value));
    float offset = 0.0F;
    for (const char character : text) {
        const auto& segments = digits[static_cast<std::size_t>(character - '0')];
        const std::array<sf::FloatRect, 7> shapes{{
            sf::FloatRect({position.x + offset + 3.0F, position.y}, {9.0F, 2.0F}),
            sf::FloatRect({position.x + offset + 12.0F, position.y + 2.0F}, {2.0F, 8.0F}),
            sf::FloatRect({position.x + offset + 12.0F, position.y + 12.0F}, {2.0F, 8.0F}),
            sf::FloatRect({position.x + offset + 3.0F, position.y + 20.0F}, {9.0F, 2.0F}),
            sf::FloatRect({position.x + offset + 1.0F, position.y + 12.0F}, {2.0F, 8.0F}),
            sf::FloatRect({position.x + offset + 1.0F, position.y + 2.0F}, {2.0F, 8.0F}),
            sf::FloatRect({position.x + offset + 3.0F, position.y + 10.0F}, {9.0F, 2.0F}),
        }};
        for (std::size_t index = 0; index < segments.size(); ++index) {
            if (!segments[index]) {
                continue;
            }
            sf::RectangleShape segment(shapes[index].size);
            segment.setPosition(shapes[index].position);
            segment.setFillColor({255, 236, 125});
            window.draw(segment);
        }
        offset += 17.0F;
    }
}

}  // namespace object_project
