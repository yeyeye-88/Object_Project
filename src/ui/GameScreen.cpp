#include "ui/GameScreen.hpp"

#include "game/BoardGeometry.hpp"
#include "ui/UiText.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <string>

namespace object_project {

namespace {

constexpr float visualBoardLeft = 175.0F;
constexpr float visualCellWidth = 625.0F / static_cast<float>(BoardGeometry::columns);
constexpr float visualScaleX = visualCellWidth / BoardGeometry::cellWidth;

float boardToScreenX(const float x) {
    return visualBoardLeft + (x - BoardGeometry::left) * visualScaleX;
}

float screenToBoardX(const float x) {
    return BoardGeometry::left + (x - visualBoardLeft) / visualScaleX;
}

float visualCellLeft(const int column) {
    return visualBoardLeft + static_cast<float>(column) * visualCellWidth;
}

std::optional<GridPosition> visualGridAt(const float x, const float y) {
    if (x < visualBoardLeft || y < BoardGeometry::top ||
        x >= visualBoardLeft + BoardGeometry::columns * visualCellWidth ||
        y >= BoardGeometry::top + BoardGeometry::rows * BoardGeometry::cellHeight) {
        return std::nullopt;
    }
    return GridPosition{
        static_cast<int>(std::floor((y - BoardGeometry::top) / BoardGeometry::cellHeight)),
        static_cast<int>(std::floor((x - visualBoardLeft) / visualCellWidth)),
    };
}

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

void drawThemedButton(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::FloatRect bounds
) {
    const sf::Vector2u source = texture.getSize();
    const unsigned int frameHeight = source.y / 2U;
    sf::Sprite button(texture);
    button.setTextureRect(sf::IntRect(
        {0, 0},
        {static_cast<int>(source.x), static_cast<int>(frameHeight)}
    ));
    button.setScale({bounds.size.x / source.x, bounds.size.y / frameHeight});
    button.setPosition(bounds.position);
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

void placeBottomCentered(
    sf::Sprite& sprite,
    const sf::FloatRect bounds,
    const sf::Vector2f maximumSize
) {
    fitSprite(sprite, maximumSize);
    const sf::FloatRect local = sprite.getLocalBounds();
    const sf::Vector2f scale = sprite.getScale();
    const float width = local.size.x * scale.x;
    const float height = local.size.y * scale.y;
    sprite.setPosition({
        bounds.position.x + (bounds.size.x - width) * 0.5F - local.position.x * scale.x,
        bounds.position.y + bounds.size.y - height - local.position.y * scale.y,
    });
}

void placeBottomCenteredAtHeight(
    sf::Sprite& sprite,
    const float centerX,
    const float bottomY,
    const float targetHeight
) {
    const sf::Vector2u source = sprite.getTexture().getSize();
    if (source.x == 0 || source.y == 0) {
        return;
    }
    const float scale = targetHeight / static_cast<float>(source.y);
    sprite.setScale({scale, scale});
    sprite.setPosition({
        centerX - static_cast<float>(source.x) * scale * 0.5F,
        bottomY - targetHeight,
    });
}

std::vector<sf::Texture> loadNumberedFrames(
    const std::filesystem::path& directory,
    const int first,
    const int last
) {
    std::vector<sf::Texture> frames;
    frames.reserve(static_cast<std::size_t>(last - first + 1));
    for (int number = first; number <= last; ++number) {
        sf::Texture texture;
        if (!texture.loadFromFile(directory / (std::to_string(number) + ".png"))) {
            frames.clear();
            break;
        }
        texture.setSmooth(true);
        frames.push_back(std::move(texture));
    }
    return frames;
}

}  // namespace

GameScreen::GameScreen(GameSession& session) : session_(session) {
    backgroundLoaded_ = backgroundTexture_.loadFromFile("assets/background/lawn.jpg");

    const std::array<std::string, plantTypeCount> cardPaths{
        "assets/pvz/cards/card_sunflower.png",
        "assets/pvz/cards/card_peashooter.png",
        "assets/pvz/cards/card_wallnut.png",
        "",
        "",
        "assets/pvz/cards/card_snowpea.png",
        "",
        "",
        "assets/pvz/cards/card_cherrybomb.png",
        "",
    };
    const std::array<std::string, plantTypeCount> plantPaths{
        "assets/pvz/plants/sunflower.png",
        "assets/pvz/plants/peashooter.png",
        "assets/pvz/plants/wallnut.png",
        "assets/pvz/plants/repeater.png",
        "",
        "assets/pvz/plants/snowpea.png",
        "assets/pvz/plants/tallnut.gif",
        "",
        "assets/pvz/plants/cherrybomb.png",
        "",
    };
    const std::array<std::string, zombieTypeCount> zombiePaths{
        "assets/pvz/zombies/normal.png",
        "assets/pvz/zombies/conehead.png",
        "assets/pvz/zombies/buckethead.png",
        "assets/pvz/zombies/flag.png",
        "assets/pvz/zombies/pole_vaulting.gif",
        "assets/pvz/zombies/newspaper.gif",
    };

    for (std::size_t index = 0; index < plantTypeCount; ++index) {
        cardLoaded_[index] = !cardPaths[index].empty() &&
                             cardTextures_[index].loadFromFile(cardPaths[index]);
        plantLoaded_[index] = !plantPaths[index].empty() &&
                              plantTextures_[index].loadFromFile(plantPaths[index]);
    }
    for (std::size_t index = 0; index < zombieTypeCount; ++index) {
        zombieLoaded_[index] = zombieTextures_[index].loadFromFile(zombiePaths[index]);
    }

    plantAnimationFrames_[toIndex(PlantType::Sunflower)] = loadNumberedFrames(
        "assets/pvz/animations/plants/sunflower", 0, 17
    );
    plantAnimationFrames_[toIndex(PlantType::Peashooter)] = loadNumberedFrames(
        "assets/pvz/animations/plants/peashooter", 0, 12
    );
    plantAnimationFrames_[toIndex(PlantType::WallNut)] = loadNumberedFrames(
        "assets/pvz/animations/plants/wallnut", 0, 15
    );
    plantAnimationFrames_[toIndex(PlantType::Repeater)] = loadNumberedFrames(
        "assets/pvz/animations/plants/repeater", 0, 14
    );
    plantAnimationFrames_[toIndex(PlantType::Threepeater)] = loadNumberedFrames(
        "assets/pvz/animations/plants/threepeater", 0, 15
    );
    plantAnimationFrames_[toIndex(PlantType::SnowPea)] = loadNumberedFrames(
        "assets/pvz/animations/plants/snowpea", 0, 14
    );
    plantAnimationFrames_[toIndex(PlantType::TallNut)] = loadNumberedFrames(
        "assets/pvz/animations/plants/tallnut", 0, 13
    );
    plantAnimationFrames_[toIndex(PlantType::PotatoMine)] = loadNumberedFrames(
        "assets/pvz/animations/plants/potatomine", 0, 7
    );
    plantAnimationFrames_[toIndex(PlantType::CherryBomb)] = loadNumberedFrames(
        "assets/pvz/animations/plants/cherrybomb", 0, 6
    );
    plantAnimationFrames_[toIndex(PlantType::Torchwood)] = loadNumberedFrames(
        "assets/pvz/animations/plants/torchwood", 0, 8
    );
    zombieAnimationFrames_[static_cast<std::size_t>(ZombieType::Normal)] = loadNumberedFrames(
        "assets/pvz/animations/zombies/normal", 0, 21
    );
    zombieAnimationFrames_[static_cast<std::size_t>(ZombieType::Conehead)] = loadNumberedFrames(
        "assets/pvz/animations/zombies/conehead", 0, 20
    );
    zombieAnimationFrames_[static_cast<std::size_t>(ZombieType::Buckethead)] = loadNumberedFrames(
        "assets/pvz/animations/zombies/buckethead", 0, 14
    );
    zombieAnimationFrames_[static_cast<std::size_t>(ZombieType::Flag)] = loadNumberedFrames(
        "assets/pvz/animations/zombies/flag", 0, 11
    );
    zombieAnimationFrames_[static_cast<std::size_t>(ZombieType::PoleVaulting)] = loadNumberedFrames(
        "assets/pvz/animations/zombies/pole_vaulting", 0, 9
    );
    zombieAnimationFrames_[static_cast<std::size_t>(ZombieType::Newspaper)] = loadNumberedFrames(
        "assets/pvz/animations/zombies/newspaper", 0, 18
    );
    poleWalkingAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/pole_walking", 0, 24
    );
    newspaperEnragedAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/newspaper_enraged", 0, 13
    );
    zombieAttackAnimationFrames_[static_cast<std::size_t>(ZombieType::Normal)] =
        loadNumberedFrames("assets/pvz/animations/zombies/attack/normal", 0, 20);
    zombieAttackAnimationFrames_[static_cast<std::size_t>(ZombieType::Conehead)] =
        loadNumberedFrames("assets/pvz/animations/zombies/attack/conehead", 0, 10);
    zombieAttackAnimationFrames_[static_cast<std::size_t>(ZombieType::Buckethead)] =
        loadNumberedFrames("assets/pvz/animations/zombies/attack/buckethead", 0, 10);
    zombieAttackAnimationFrames_[static_cast<std::size_t>(ZombieType::Flag)] =
        loadNumberedFrames("assets/pvz/animations/zombies/attack/flag", 0, 10);
    zombieAttackAnimationFrames_[static_cast<std::size_t>(ZombieType::PoleVaulting)] =
        loadNumberedFrames("assets/pvz/animations/zombies/attack/pole_vaulting", 0, 13);
    zombieAttackAnimationFrames_[static_cast<std::size_t>(ZombieType::Newspaper)] =
        loadNumberedFrames("assets/pvz/animations/zombies/attack/newspaper", 0, 7);
    newspaperEnragedAttackAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/attack/newspaper_enraged", 0, 6
    );
    poleJumpAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/pole_jump", 0, 16
    );
    newspaperTransitionAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/newspaper_transition", 0, 10
    );
    normalDeathAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/death/normal", 0, 9
    );
    poleDeathAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/death/pole_vaulting", 0, 8
    );
    newspaperDeathAnimationFrames_ = loadNumberedFrames(
        "assets/pvz/animations/zombies/death/newspaper", 0, 10
    );

    const std::array<std::string, 3> threepeaterStemPaths{
        "assets/pvz/plants/threepeater_stem1.png",
        "assets/pvz/plants/threepeater_stem2.png",
        "assets/pvz/plants/threepeater_stem3.png",
    };
    for (std::size_t index = 0; index < threepeaterStemPaths.size(); ++index) {
        threepeaterStemLoaded_[index] = threepeaterStemTextures_[index].loadFromFile(
            threepeaterStemPaths[index]
        );
    }
    threepeaterHeadLoaded_ = threepeaterHeadTexture_.loadFromFile(
        "assets/pvz/plants/threepeater_head.png"
    );
    potatoMineLoaded_ = potatoMineTexture_.loadFromFile(
        "assets/pvz/plants/potatomine_body.png"
    );
    torchwoodBodyLoaded_ = torchwoodBodyTexture_.loadFromFile(
        "assets/pvz/plants/torchwood_body.png"
    );
    torchwoodFireLoaded_ = torchwoodFireTexture_.loadFromFile(
        "assets/pvz/plants/torchwood_fire.png"
    );

    sunLoaded_ = sunTexture_.loadFromFile("assets/sun/sun.png");
    shovelLoaded_ = shovelTexture_.loadFromFile("assets/pvz/ui/reference/shovel.png");
    chooserLoaded_ = chooserTexture_.loadFromFile(
        "assets/pvz/ui/reference/chooser_background.png"
    );
    shovelSlotLoaded_ = shovelSlotTexture_.loadFromFile(
        "assets/pvz/ui/reference/shovel_slot.png"
    );
    overlayPanelLoaded_ = overlayPanelTexture_.loadFromFile(
        "assets/pvz/ui/screens/options_menu.png"
    );
    overlayButtonLoaded_ = overlayButtonTexture_.loadFromFile(
        "assets/pvz/ui/screens/stone_button.png"
    );
    resultPanelLoaded_ = resultPanelTexture_.loadFromFile(
        "assets/pvz/ui/screens/award_screen.jpg"
    );
    zombiesWonLoaded_ = zombiesWonTexture_.loadFromFile(
        "assets/pvz/ui/screens/zombies_won.png"
    );
    mowerLoaded_ = mowerTexture_.loadFromFile("assets/pvz/ui/lawn_mower.png");
    peaLoaded_ = peaTexture_.loadFromFile("assets/pvz/projectiles/pea.png");
    frozenPeaLoaded_ = frozenPeaTexture_.loadFromFile("assets/pvz/projectiles/snow_pea.png");
    feedbackFontLoaded_ = ui::loadChineseFont(feedbackFont_);
    observedWave_ = session_.level().currentWave();
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

bool GameScreen::takeMenuRequest() {
    const bool requested = menuRequested_;
    menuRequested_ = false;
    return requested;
}

std::vector<SoundCue> GameScreen::takeSoundCues() {
    std::vector<SoundCue> cues;
    cues.swap(soundCues_);
    return cues;
}

void GameScreen::update(const float deltaSeconds) {
    animationTime_ += deltaSeconds;
    waveBannerSeconds_ = std::max(0.0F, waveBannerSeconds_ - deltaSeconds);
    for (ZombieDeathEffect& effect : zombieDeathEffects_) {
        effect.age += deltaSeconds;
    }
    zombieDeathEffects_.erase(
        std::remove_if(
            zombieDeathEffects_.begin(),
            zombieDeathEffects_.end(),
            [](const ZombieDeathEffect& effect) { return effect.age >= effect.duration; }
        ),
        zombieDeathEffects_.end()
    );

    const auto& currentZombies = session_.combat().zombies();
    for (const ZombieSnapshot& snapshot : zombieSnapshots_) {
        const bool stillPresent = std::any_of(
            currentZombies.begin(),
            currentZombies.end(),
            [&snapshot](const std::unique_ptr<Zombie>& zombie) {
                return zombie.get() == snapshot.identity;
            }
        );
        if (!stillPresent) {
            zombieDeathEffects_.push_back({
                snapshot.type,
                snapshot.row,
                snapshot.x,
                snapshot.slowed,
            });
        }
    }
    zombieSnapshots_.clear();
    zombieSnapshots_.reserve(currentZombies.size());
    for (const auto& zombie : currentZombies) {
        zombieSnapshots_.push_back({
            zombie.get(),
            zombie->type(),
            zombie->row(),
            zombie->visualX(),
            zombie->isSlowed(),
        });
    }
    for (PulseEffect& effect : pulseEffects_) {
        effect.age += deltaSeconds;
    }
    pulseEffects_.erase(
        std::remove_if(
            pulseEffects_.begin(),
            pulseEffects_.end(),
            [](const PulseEffect& effect) { return effect.age >= effect.duration; }
        ),
        pulseEffects_.end()
    );

    const int currentWave = session_.level().currentWave();
    if (currentWave > observedWave_) {
        observedWave_ = currentWave;
        waveBannerSeconds_ = 1.8F;
        soundCues_.push_back(currentWave == 1 ? SoundCue::Ready : SoundCue::Wave);
    }

    const auto queueOnce = [this](const SoundCue cue) {
        if (std::find(soundCues_.begin(), soundCues_.end(), cue) == soundCues_.end()) {
            soundCues_.push_back(cue);
        }
    };
    for (const CombatEvent event : session_.combat().takeEvents()) {
        switch (event) {
            case CombatEvent::ProjectileFired:
                queueOnce(SoundCue::Attack);
                break;
            case CombatEvent::ProjectileHit:
                queueOnce(SoundCue::Hit);
                break;
            case CombatEvent::Explosion:
                queueOnce(SoundCue::Explosion);
                break;
            case CombatEvent::MowerStarted:
                queueOnce(SoundCue::Mower);
                break;
            case CombatEvent::PoleVaulted:
                queueOnce(SoundCue::PoleVault);
                break;
            case CombatEvent::NewspaperBroken:
                queueOnce(SoundCue::Newspaper);
                break;
            case CombatEvent::ZombieBite:
                queueOnce(SoundCue::Chomp);
                break;
            case CombatEvent::FirePeaIgnited:
                queueOnce(SoundCue::FirePea);
                break;
        }
    }

    const GameState currentState = session_.state();
    if (currentState != observedGameState_) {
        if (currentState == GameState::Won) {
            queueOnce(SoundCue::Win);
        } else if (currentState == GameState::Lost) {
            queueOnce(SoundCue::Lose);
        }
        observedGameState_ = currentState;
    }
}

void GameScreen::resetInteraction() {
    selectedPlant_.reset();
    shovelSelected_ = false;
    hoveredCell_.reset();
    almanacRequested_ = false;
    menuRequested_ = false;
    soundCues_.clear();
    pulseEffects_.clear();
    zombieSnapshots_.clear();
    zombieDeathEffects_.clear();
    waveBannerSeconds_ = 1.2F;
    observedWave_ = 0;
    observedGameState_ = session_.state();
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
            resetInteraction();
        }
        return;
    }

    if (const auto* move = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f worldPosition = window.mapPixelToCoords(move->position);
        hoveredCell_ = visualGridAt(
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
            resetInteraction();
        } else if (contains(menuBounds, position)) {
            menuRequested_ = true;
        }
        return;
    }

    if (session_.state() == GameState::Won || session_.state() == GameState::Lost) {
        if (contains(resultRestartBounds, position)) {
            session_.restart();
            resetInteraction();
        } else if (contains(resultMenuBounds, position)) {
            menuRequested_ = true;
        }
        return;
    }

    if (session_.collectSunAt(screenToBoardX(position.x), position.y)) {
        pulseEffects_.push_back({position, {255, 224, 72}, 0.0F, 0.55F});
        soundCues_.push_back(SoundCue::CollectSun);
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

    const auto cell = visualGridAt(
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
        pulseEffects_.push_back({
            {
                visualCellLeft(cell->column) + visualCellWidth * 0.5F,
                BoardGeometry::cellTop(cell->row) + BoardGeometry::cellHeight * 0.5F,
            },
            plantColor(*selectedPlant_),
            0.0F,
            0.45F,
        });
        soundCues_.push_back(SoundCue::Plant);
        selectedPlant_.reset();
    }
}

void GameScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window);
    drawBoard(window);
    drawCombat(window);
    drawSuns(window);
    drawToolbar(window);
    drawCards(window);
    drawFeedback(window);
    if (session_.isPaused()) {
        drawPauseOverlay(window);
    } else if (session_.state() == GameState::Won || session_.state() == GameState::Lost) {
        drawResultOverlay(window);
    }
}

void GameScreen::drawBackground(sf::RenderWindow& window) const {
    if (backgroundLoaded_) {
        const sf::Vector2u source = backgroundTexture_.getSize();
        const auto drawSection = [&](const int sourceLeft,
                                     const int sourceWidth,
                                     const float targetLeft,
                                     const float targetWidth) {
            sf::Sprite section(backgroundTexture_);
            section.setTextureRect(sf::IntRect(
                {sourceLeft, 0},
                {sourceWidth, static_cast<int>(source.y)}
            ));
            section.setScale({
                targetWidth / static_cast<float>(sourceWidth),
                600.0F / static_cast<float>(source.y),
            });
            section.setPosition({targetLeft, 0.0F});
            window.draw(section);
        };
        drawSection(0, 230, 0.0F, 175.0F);
        drawSection(230, 770, 175.0F, 625.0F);
        drawSection(1000, static_cast<int>(source.x) - 1000, 800.0F, 160.0F);
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
                drawPlant(
                    window,
                    plant->type(),
                    position,
                    false,
                    plant->isPerformingAction() ? plant->actionAnimationProgress() : -1.0F
                );
                drawHealthBar(
                    window,
                    static_cast<float>(plant->health()) /
                        static_cast<float>(plant->maxHealth()),
                    {visualCellLeft(column) + 7.0F,
                     BoardGeometry::cellTop(row) + BoardGeometry::cellHeight - 10.0F},
                    visualCellWidth - 14.0F
                );
            }
        }
    }

    if (!hoveredCell_ || session_.isPaused()) {
        return;
    }

    sf::RectangleShape highlight({visualCellWidth - 4.0F, BoardGeometry::cellHeight - 4.0F});
    highlight.setPosition({
        visualCellLeft(hoveredCell_->column) + 2.0F,
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
        const float mowerX = boardToScreenX(mower.x);
        if (mower.state == LawnMowerState::Used) {
            // The original game leaves the lawn unchanged after a mower is consumed.
            continue;
        }
        if (mowerLoaded_) {
            sf::Sprite sprite(mowerTexture_);
            fitSprite(sprite, {58.0F, 48.0F});
            sprite.setPosition({mowerX - 4.0F, BoardGeometry::cellTop(mower.row) + 48.0F});
            window.draw(sprite);
        } else {
            sf::RectangleShape body({48.0F, 34.0F});
            body.setPosition({mowerX, BoardGeometry::cellTop(mower.row) + 54.0F});
            body.setFillColor({202, 48, 42});
            body.setOutlineColor({88, 25, 23});
            body.setOutlineThickness(2.0F);
            window.draw(body);
        }
    }

    for (const Projectile& projectile : session_.combat().projectiles()) {
        const float projectileX = boardToScreenX(projectile.x);
        sf::RectangleShape trail({18.0F, 4.0F});
        trail.setPosition({projectileX - 14.0F, BoardGeometry::cellTop(projectile.row) + 44.0F});
        trail.setFillColor({205, 255, 185, 105});
        window.draw(trail);
        const bool frozen = projectile.type == Projectile::Type::FrozenPea;
        const sf::Texture* texture = frozen && frozenPeaLoaded_
            ? &frozenPeaTexture_
            : peaLoaded_ ? &peaTexture_ : nullptr;
        if (texture) {
            sf::Sprite pea(*texture);
            fitSprite(pea, {24.0F, 24.0F});
            pea.setPosition({projectileX - 5.0F, BoardGeometry::cellTop(projectile.row) + 34.0F});
            if (projectile.type == Projectile::Type::FirePea) {
                pea.setColor({255, 125, 55});
            }
            window.draw(pea);
        } else {
            sf::CircleShape pea(7.0F);
            pea.setPosition({projectileX, BoardGeometry::cellTop(projectile.row) + 39.0F});
            pea.setFillColor(frozen ? sf::Color(104, 205, 245) : sf::Color(92, 220, 65));
            window.draw(pea);
        }
    }

    for (const auto& zombie : session_.combat().zombies()) {
        const float visualYOffset = zombie->visualYOffset();
        const float top = BoardGeometry::cellTop(zombie->row()) + 6.0F + visualYOffset;
        const float zombieX = boardToScreenX(zombie->visualX());
        sf::RectangleShape body({CombatSystem::zombieWidth, 80.0F});
        body.setPosition({zombieX, top});
        sf::Color bodyColor{105, 125, 91};
        if (zombie->type() == ZombieType::Conehead) {
            bodyColor = {152, 118, 65};
        } else if (zombie->type() == ZombieType::Buckethead) {
            bodyColor = {98, 111, 116};
        } else if (zombie->type() == ZombieType::Flag) {
            bodyColor = {150, 62, 58};
        } else if (zombie->type() == ZombieType::PoleVaulting) {
            bodyColor = {91, 80, 139};
        } else if (zombie->type() == ZombieType::Newspaper) {
            bodyColor = zombie->isEnraged()
                ? sf::Color(190, 62, 48)
                : sf::Color(125, 126, 100);
        }
        if (zombie->isSlowed()) {
            bodyColor = sf::Color(
                static_cast<std::uint8_t>(bodyColor.r * 0.7F),
                static_cast<std::uint8_t>(bodyColor.g * 0.85F),
                190
            );
        }
        const std::size_t zombieIndex = static_cast<std::size_t>(zombie->type());
        const std::vector<sf::Texture>* animation = &zombieAnimationFrames_[zombieIndex];
        float oneShotProgress = -1.0F;
        if (zombie->type() == ZombieType::PoleVaulting && zombie->isVaulting() &&
            !poleJumpAnimationFrames_.empty()) {
            animation = &poleJumpAnimationFrames_;
            oneShotProgress = zombie->vaultProgress();
        } else if (zombie->type() == ZombieType::Newspaper && zombie->isTransforming() &&
                   !newspaperTransitionAnimationFrames_.empty()) {
            animation = &newspaperTransitionAnimationFrames_;
            oneShotProgress = zombie->transformationProgress();
        } else if (zombie->isAttacking() && zombie->type() == ZombieType::Newspaper &&
                   zombie->isEnraged() && !newspaperEnragedAttackAnimationFrames_.empty()) {
            animation = &newspaperEnragedAttackAnimationFrames_;
        } else if (zombie->isAttacking() &&
                   !zombieAttackAnimationFrames_[zombieIndex].empty()) {
            animation = &zombieAttackAnimationFrames_[zombieIndex];
        } else if (zombie->type() == ZombieType::PoleVaulting && !zombie->canVault() &&
                   !poleWalkingAnimationFrames_.empty()) {
            animation = &poleWalkingAnimationFrames_;
        } else if (zombie->type() == ZombieType::Newspaper && zombie->isEnraged() &&
                   !newspaperEnragedAnimationFrames_.empty()) {
            animation = &newspaperEnragedAnimationFrames_;
        }
        const sf::Texture* zombieTexture = nullptr;
        if (!animation->empty()) {
            const std::size_t frame = oneShotProgress >= 0.0F
                ? std::min(
                      static_cast<std::size_t>(oneShotProgress * animation->size()),
                      animation->size() - 1
                  )
                : static_cast<std::size_t>(
                      animationTime_ * 11.0F + zombie->x() * 0.015F
                  ) % animation->size();
            zombieTexture = &(*animation)[frame];
        } else if (zombieLoaded_[zombieIndex]) {
            zombieTexture = &zombieTextures_[zombieIndex];
        }
        if (zombieTexture) {
            sf::Sprite sprite(*zombieTexture);
            const float collisionWidth = CombatSystem::zombieWidth * visualScaleX;
            const bool poleVaulting = zombie->type() == ZombieType::PoleVaulting;
            placeBottomCenteredAtHeight(
                sprite,
                zombieX + collisionWidth * 0.5F - (poleVaulting ? 52.0F : 0.0F),
                BoardGeometry::cellTop(zombie->row()) + 104.0F + visualYOffset,
                poleVaulting ? 182.0F : 132.0F
            );
            if (zombie->isSlowed()) {
                sprite.setColor({180, 225, 255});
            }
            window.draw(sprite);
        } else {
            body.setFillColor(bodyColor);
            body.setOutlineColor({48, 61, 42});
            body.setOutlineThickness(3.0F);
            window.draw(body);

            sf::CircleShape head(17.0F);
            head.setPosition({zombieX + 7.0F, top - 8.0F});
            head.setFillColor({150, 165, 130});
            window.draw(head);
        }
        if (!zombieTexture && zombie->type() == ZombieType::Flag) {
            sf::RectangleShape pole({3.0F, 75.0F});
            pole.setPosition({zombieX + 41.0F, top - 20.0F});
            pole.setFillColor({82, 54, 38});
            window.draw(pole);
            sf::RectangleShape flag({30.0F, 20.0F});
            flag.setPosition({zombieX + 44.0F, top - 18.0F});
            flag.setFillColor({205, 45, 40});
            window.draw(flag);
        } else if (!zombieTexture &&
                   zombie->type() == ZombieType::PoleVaulting && zombie->canVault()) {
            sf::RectangleShape pole({78.0F, 4.0F});
            pole.setPosition({zombieX - 15.0F, top + 38.0F});
            pole.setRotation(sf::degrees(-12.0F));
            pole.setFillColor({128, 86, 49});
            window.draw(pole);
        } else if (!zombieTexture &&
                   zombie->type() == ZombieType::Newspaper && !zombie->isEnraged()) {
            sf::RectangleShape paper({38.0F, 28.0F});
            paper.setPosition({zombieX + 4.0F, top + 38.0F});
            paper.setFillColor({225, 218, 188});
            paper.setOutlineColor({75, 70, 58});
            paper.setOutlineThickness(2.0F);
            window.draw(paper);
        }
        drawHealthBar(
            window,
            static_cast<float>(zombie->health()) /
                static_cast<float>(zombie->maxHealth()),
            {zombieX - 9.0F, top - 14.0F},
            60.0F
        );
        if (zombie->maxArmor() > 0) {
            drawHealthBar(
                window,
                static_cast<float>(zombie->armor()) /
                    static_cast<float>(zombie->maxArmor()),
                {zombieX - 9.0F, top - 7.0F},
                60.0F
            );
        }
    }

    for (const ZombieDeathEffect& effect : zombieDeathEffects_) {
        const std::vector<sf::Texture>* animation = &normalDeathAnimationFrames_;
        if (effect.type == ZombieType::PoleVaulting) {
            animation = &poleDeathAnimationFrames_;
        } else if (effect.type == ZombieType::Newspaper) {
            animation = &newspaperDeathAnimationFrames_;
        }
        if (animation->empty()) {
            continue;
        }
        const float progress = std::clamp(effect.age / effect.duration, 0.0F, 1.0F);
        const std::size_t frame = std::min(
            static_cast<std::size_t>(progress * animation->size()),
            animation->size() - 1
        );
        sf::Sprite sprite((*animation)[frame]);
        const float zombieX = boardToScreenX(effect.x);
        const float collisionWidth = CombatSystem::zombieWidth * visualScaleX;
        const bool poleVaulting = effect.type == ZombieType::PoleVaulting;
        placeBottomCenteredAtHeight(
            sprite,
            zombieX + collisionWidth * 0.5F - (poleVaulting ? 52.0F : 0.0F),
            BoardGeometry::cellTop(effect.row) + 104.0F,
            poleVaulting ? 182.0F : 132.0F
        );
        if (effect.slowed) {
            sprite.setColor({180, 225, 255});
        }
        window.draw(sprite);
    }
}

void GameScreen::drawPlant(
    sf::RenderWindow& window,
    const PlantType type,
    const GridPosition position,
    const bool preview,
    const float actionProgress
) const {
    const std::size_t index = toIndex(type);
    const sf::Vector2f targetPosition{
        visualCellLeft(position.column) + 2.0F,
        BoardGeometry::cellTop(position.row) + 5.0F,
    };

    const sf::Color previewColor = preview ? sf::Color(255, 255, 255, 145) : sf::Color::White;
    if (type == PlantType::Threepeater && plantAnimationFrames_[index].empty() &&
        threepeaterHeadLoaded_) {
        static constexpr std::array<sf::Vector2f, 3> stemOffsets{{
            {7.0F, 31.0F}, {29.0F, 22.0F}, {48.0F, 32.0F},
        }};
        static constexpr std::array<sf::Vector2f, 3> headOffsets{{
            {-1.0F, 15.0F}, {23.0F, 4.0F}, {46.0F, 16.0F},
        }};
        for (std::size_t part = 0; part < stemOffsets.size(); ++part) {
            if (threepeaterStemLoaded_[part]) {
                sf::Sprite stem(threepeaterStemTextures_[part]);
                fitSprite(stem, {28.0F, 48.0F});
                stem.setPosition(targetPosition + stemOffsets[part]);
                stem.setColor(previewColor);
                window.draw(stem);
            }
            sf::Sprite head(threepeaterHeadTexture_);
            fitSprite(head, {37.0F, 31.0F});
            head.setPosition(targetPosition + headOffsets[part]);
            head.setColor(previewColor);
            window.draw(head);
        }
        return;
    }
    if (type == PlantType::PotatoMine && plantAnimationFrames_[index].empty() &&
        potatoMineLoaded_) {
        sf::Sprite sprite(potatoMineTexture_);
        fitSprite(sprite, {68.0F, 50.0F});
        sprite.setPosition({targetPosition.x + 4.0F, targetPosition.y + 35.0F});
        sprite.setColor(previewColor);
        window.draw(sprite);
        return;
    }
    if (type == PlantType::Torchwood && plantAnimationFrames_[index].empty() &&
        torchwoodBodyLoaded_) {
        sf::Sprite body(torchwoodBodyTexture_);
        fitSprite(body, {62.0F, 76.0F});
        body.setPosition({targetPosition.x + 7.0F, targetPosition.y + 12.0F});
        body.setColor(previewColor);
        window.draw(body);
        if (torchwoodFireLoaded_) {
            sf::Sprite fire(torchwoodFireTexture_);
            fitSprite(fire, {46.0F, 33.0F});
            fire.setPosition({targetPosition.x + 15.0F, targetPosition.y - 2.0F});
            fire.setColor(previewColor);
            window.draw(fire);
        }
        return;
    }

    const std::vector<sf::Texture>& animation = plantAnimationFrames_[index];
    const sf::Texture* plantTexture = nullptr;
    if (!animation.empty()) {
        const std::size_t frame = preview
            ? 0
            : actionProgress >= 0.0F
                ? std::min(
                      static_cast<std::size_t>(actionProgress * animation.size()),
                      animation.size() - 1
                  )
                : static_cast<std::size_t>(
                      animationTime_ * 10.0F +
                      static_cast<float>(position.row * 2 + position.column)
                  ) % animation.size();
        plantTexture = &animation[frame];
    } else if (plantLoaded_[index]) {
        plantTexture = &plantTextures_[index];
    }
    if (plantTexture) {
        sf::Sprite sprite(*plantTexture);
        placeBottomCentered(
            sprite,
            {{visualCellLeft(position.column), BoardGeometry::cellTop(position.row)},
             {visualCellWidth, BoardGeometry::cellHeight - 8.0F}},
            {68.0F, 84.0F}
        );
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
        const float pulse = 1.0F + std::sin(animationTime_ * 5.0F + token.x * 0.02F) * 0.08F;
        const float tokenX = boardToScreenX(token.x);
        if (sunLoaded_) {
            sf::Sprite sprite(sunTexture_);
            fitSprite(sprite, {SunSystem::tokenSize, SunSystem::tokenSize});
            sprite.scale({pulse, pulse});
            sprite.setPosition({
                tokenX - SunSystem::tokenSize * (pulse - 1.0F) * 0.5F,
                token.y - SunSystem::tokenSize * (pulse - 1.0F) * 0.5F,
            });
            window.draw(sprite);
        } else {
            sf::CircleShape sun(SunSystem::tokenSize * pulse / 2.0F);
            sun.setPosition({
                tokenX - SunSystem::tokenSize * (pulse - 1.0F) * 0.5F,
                token.y - SunSystem::tokenSize * (pulse - 1.0F) * 0.5F,
            });
            sun.setFillColor({255, 215, 50});
            window.draw(sun);
        }
    }
}

void GameScreen::drawFeedback(sf::RenderWindow& window) const {
    for (const PulseEffect& effect : pulseEffects_) {
        const float progress = effect.age / effect.duration;
        sf::CircleShape ring(16.0F + progress * 42.0F, 40);
        ring.setOrigin({ring.getRadius(), ring.getRadius()});
        ring.setPosition(effect.center);
        ring.setFillColor(sf::Color::Transparent);
        sf::Color color = effect.color;
        color.a = static_cast<std::uint8_t>(210.0F * (1.0F - progress));
        ring.setOutlineColor(color);
        ring.setOutlineThickness(5.0F * (1.0F - progress));
        window.draw(ring);
    }

    if (waveBannerSeconds_ > 0.0F) {
        const float opacity = std::min(1.0F, waveBannerSeconds_ * 2.0F);
        sf::RectangleShape banner({420.0F, 72.0F});
        banner.setPosition({270.0F, 112.0F});
        banner.setFillColor({90, 25, 25, static_cast<std::uint8_t>(190.0F * opacity)});
        banner.setOutlineColor({255, 219, 92, static_cast<std::uint8_t>(240.0F * opacity)});
        banner.setOutlineThickness(4.0F);
        window.draw(banner);
        if (feedbackFontLoaded_) {
            sf::Text text(
                feedbackFont_,
                ui::utf8("第 " + std::to_string(session_.level().currentWave()) + " 波"),
                38
            );
            ui::centerText(text, {{270.0F, 112.0F}, {420.0F, 72.0F}});
            text.setFillColor({255, 236, 150, static_cast<std::uint8_t>(255.0F * opacity)});
            window.draw(text);
        }
    }
}

void GameScreen::drawCards(sf::RenderWindow& window) const {
    for (const CardLayout& layout : cardLayouts()) {
        const std::size_t index = toIndex(layout.type);
        sf::RectangleShape card(cardSize);
        card.setPosition(layout.position);
        card.setFillColor({206, 184, 126});
        card.setOutlineColor({73, 47, 26});
        card.setOutlineThickness(2.0F);
        window.draw(card);
        sf::RectangleShape portrait({cardSize.x - 6.0F, 43.0F});
        portrait.setPosition(layout.position + sf::Vector2f{3.0F, 3.0F});
        portrait.setFillColor({70, 130, 58});
        window.draw(portrait);

        const sf::Texture* previewTexture = nullptr;
        if (!plantAnimationFrames_[index].empty()) {
            previewTexture = &plantAnimationFrames_[index].front();
        } else if (plantLoaded_[index]) {
            previewTexture = &plantTextures_[index];
        } else if (layout.type == PlantType::Threepeater && threepeaterHeadLoaded_) {
            previewTexture = &threepeaterHeadTexture_;
        } else if (layout.type == PlantType::PotatoMine && potatoMineLoaded_) {
            previewTexture = &potatoMineTexture_;
        } else if (layout.type == PlantType::Torchwood && torchwoodBodyLoaded_) {
            previewTexture = &torchwoodBodyTexture_;
        }
        if (previewTexture) {
            sf::Sprite preview(*previewTexture);
            placeBottomCentered(
                preview,
                {{layout.position.x + 3.0F, layout.position.y + 3.0F}, {cardSize.x - 6.0F, 43.0F}},
                {39.0F, 40.0F}
            );
            window.draw(preview);
        }
        drawNumber(
            window,
            session_.cards().definition(layout.type).cost,
            {layout.position.x + 3.0F, layout.position.y + 43.0F}
        );

        const float cooldown = session_.cards().cooldownProgress(layout.type);
        const bool unaffordable = session_.sun() < session_.cards().definition(layout.type).cost;
        const bool unavailable = !session_.isPlantAvailable(layout.type);
        if (cooldown > 0.0F || unaffordable || unavailable) {
            const float maskHeight = cooldown > 0.0F ? cardSize.y * cooldown : cardSize.y;
            sf::RectangleShape mask({cardSize.x, maskHeight});
            mask.setPosition(layout.position);
            mask.setFillColor({
                20,
                20,
                20,
                static_cast<std::uint8_t>(unavailable ? 205 : (unaffordable ? 155 : 115)),
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
    if (chooserLoaded_) {
        sf::Sprite chooser(chooserTexture_);
        const sf::Vector2u source = chooserTexture_.getSize();
        chooser.setScale({590.0F / source.x, 98.0F / source.y});
        window.draw(chooser);
        sf::RectangleShape valueCover({54.0F, 23.0F});
        valueCover.setPosition({10.0F, 66.0F});
        valueCover.setFillColor({205, 180, 127});
        window.draw(valueCover);
    } else {
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
    }
    drawNumber(window, session_.sun(), {16.0F, 62.0F});

    if (shovelSlotLoaded_) {
        sf::Sprite slot(shovelSlotTexture_);
        const sf::Vector2u source = shovelSlotTexture_.getSize();
        slot.setScale({shovelBounds.size.x / source.x, shovelBounds.size.y / source.y});
        slot.setPosition(shovelBounds.position);
        window.draw(slot);
    } else {
        drawButton(window, shovelBounds, shovelSelected_ ? sf::Color(185, 150, 54) : sf::Color(85, 62, 37));
    }
    if (shovelLoaded_) {
        sf::Sprite shovel(shovelTexture_);
        fitSprite(shovel, {52.0F, 52.0F});
        shovel.setPosition({shovelBounds.position.x + 5.0F, shovelBounds.position.y + 4.0F});
        if (shovelSelected_) {
            shovel.setColor({255, 245, 145});
        }
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
    wavePanel.setFillColor(
        session_.level().isEndless()
            ? sf::Color(91, 54, 105, 225)
            : session_.level().finalWaveStarted()
                ? sf::Color(145, 52, 38, 225)
                : sf::Color(58, 80, 43, 225)
    );
    wavePanel.setOutlineColor({229, 198, 113});
    wavePanel.setOutlineThickness(2.0F);
    window.draw(wavePanel);
    drawNumber(window, session_.level().currentWave(), {696.0F, 28.0F});
    sf::RectangleShape separator({12.0F, 2.0F});
    separator.setPosition({720.0F, 39.0F});
    separator.setRotation(sf::degrees(-55.0F));
    separator.setFillColor({255, 236, 125});
    window.draw(separator);
    drawNumber(
        window,
        session_.level().isEndless()
            ? session_.highestEndlessWave()
            : session_.level().waveCount(),
        {742.0F, 28.0F}
    );

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

    if (overlayPanelLoaded_) {
        sf::Sprite panel(overlayPanelTexture_);
        panel.setPosition({275.0F, 55.0F});
        window.draw(panel);
    }
    if (feedbackFontLoaded_) {
        sf::Text title(feedbackFont_, ui::utf8("游戏暂停"), 30);
        title.setFillColor({230, 229, 215});
        ui::centerText(title, {{360.0F, 125.0F}, {240.0F, 52.0F}});
        window.draw(title);
    }

    for (const auto& item : std::array<std::pair<sf::FloatRect, const char*>, 3>{{
             {resumeBounds, "继续游戏"},
             {restartBounds, "重新开始"},
             {menuBounds, "返回选关"},
         }}) {
        if (overlayButtonLoaded_) {
            drawThemedButton(window, overlayButtonTexture_, item.first);
        } else {
            drawButton(window, item.first, {82, 91, 105, 245});
        }
        if (feedbackFontLoaded_) {
            sf::Text label(feedbackFont_, ui::utf8(item.second), 19);
            label.setFillColor({236, 234, 218});
            ui::centerText(label, item.first);
            window.draw(label);
        }
    }
}

void GameScreen::drawResultOverlay(sf::RenderWindow& window) const {
    const bool won = session_.state() == GameState::Won;
    sf::RectangleShape shade({960.0F, 600.0F});
    shade.setFillColor(won ? sf::Color(18, 75, 32, 190) : sf::Color(85, 20, 18, 195));
    window.draw(shade);

    if (won && resultPanelLoaded_) {
        sf::Sprite panel(resultPanelTexture_);
        panel.setScale({0.8F, 0.8F});
        panel.setPosition({160.0F, 55.0F});
        window.draw(panel);
    }
    if (!won && zombiesWonLoaded_) {
        sf::Sprite message(zombiesWonTexture_);
        fitSprite(message, {430.0F, 375.0F});
        const sf::FloatRect local = message.getLocalBounds();
        const sf::Vector2f scale = message.getScale();
        message.setPosition({
            480.0F - local.size.x * scale.x * 0.5F,
            62.0F,
        });
        window.draw(message);
    }
    if (won && feedbackFontLoaded_) {
        sf::Text title(feedbackFont_, ui::utf8("关卡完成"), 36);
        title.setFillColor({255, 226, 126});
        ui::centerText(title, {{260.0F, 68.0F}, {440.0F, 72.0F}});
        window.draw(title);
        sf::Text detail(feedbackFont_, ui::utf8("花园暂时安全了"), 22);
        detail.setFillColor({87, 48, 26});
        ui::centerText(detail, {{350.0F, 340.0F}, {260.0F, 80.0F}});
        window.draw(detail);
    }

    for (const auto& item : std::array<std::pair<sf::FloatRect, const char*>, 2>{{
             {resultRestartBounds, "重新开始"},
             {resultMenuBounds, "返回选关"},
         }}) {
        if (overlayButtonLoaded_) {
            drawThemedButton(window, overlayButtonTexture_, item.first);
        } else {
            drawButton(window, item.first, {82, 91, 105, 245});
        }
        if (feedbackFontLoaded_) {
            sf::Text label(feedbackFont_, ui::utf8(item.second), 19);
            label.setFillColor({236, 234, 218});
            ui::centerText(label, item.first);
            window.draw(label);
        }
    }
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
    health.setFillColor(ratio > 0.35F ? sf::Color(80, 210, 70) : sf::Color(238, 178, 58));
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
