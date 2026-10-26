#include "ui/ArcadeGameScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <random>
#include <string>

namespace object_project {
namespace {
constexpr float lawnLeft = 175.0F;
constexpr float lawnRight = 800.0F;
constexpr float boardTop = 106.0F;
constexpr float laneHeight = 94.0F;
constexpr float cellWidth = (lawnRight - lawnLeft) / 9.0F;
constexpr int bowlingLaunchColumns = 3;
constexpr float stripeX = lawnLeft + bowlingLaunchColumns * cellWidth;
constexpr sf::Vector2f seedPacketSize{52.0F, 66.0F};

constexpr float laneY(const int row) { return boardTop + row * laneHeight + laneHeight * 0.5F; }
constexpr float cellX(const int column) { return lawnLeft + column * cellWidth; }

std::vector<sf::Texture> loadFrames(
    const std::filesystem::path& directory,
    const int first,
    const int last
) {
    std::vector<sf::Texture> frames;
    for (int index = first; index <= last; ++index) {
        sf::Texture texture;
        if (!texture.loadFromFile(directory / (std::to_string(index) + ".png"))) {
            break;
        }
        frames.push_back(std::move(texture));
    }
    return frames;
}

const sf::Texture* animatedFrame(const std::vector<sf::Texture>& frames, const float age, const float fps) {
    if (frames.empty()) return nullptr;
    const auto index = static_cast<std::size_t>(age * fps) % frames.size();
    return &frames[index];
}

void fitSprite(sf::Sprite& sprite, const float width, const float height) {
    const sf::Vector2u size = sprite.getTexture().getSize();
    if (size.x == 0 || size.y == 0) return;
    const float scale = std::min(width / static_cast<float>(size.x), height / static_cast<float>(size.y));
    sprite.setScale({scale, scale});
}

void placeBottomCenteredAtHeight(
    sf::Sprite& sprite,
    const float centerX,
    const float bottomY,
    const float targetHeight
) {
    const sf::Vector2u size = sprite.getTexture().getSize();
    if (size.x == 0 || size.y == 0) return;
    const float scale = targetHeight / static_cast<float>(size.y);
    sprite.setScale({scale, scale});
    sprite.setPosition({
        centerX - static_cast<float>(size.x) * scale * 0.5F,
        bottomY - targetHeight,
    });
}
}  // namespace

ArcadeGameScreen::ArcadeGameScreen()
    : dayLawnTexture_("assets/background/lawn.jpg"),
      nightLawnTexture_("assets/pvz/backgrounds/night_lawn.jpg"),
      vaseTexture_("assets/pvz/minigames/scary_pot.png"),
      peaProjectileTexture_("assets/pvz/projectiles/pea.png"),
      snowProjectileTexture_("assets/pvz/projectiles/snow_pea.png"),
      conveyorBackdropTexture_("assets/pvz/minigames/bowling/conveyor_backdrop.png"),
      conveyorTexture_("assets/pvz/minigames/bowling/conveyor_belt.png"),
      bowlingStripeTexture_("assets/pvz/minigames/bowling/bowling_stripe.png"),
      mowerTexture_("assets/pvz/minigames/bowling/lawn_cleaner.png"),
      flagMeterTexture_("assets/pvz/minigames/bowling/flag_meter.png"),
      flagProgressTexture_("assets/pvz/minigames/bowling/flag_meter_progress.png"),
      flagPartsTexture_("assets/pvz/minigames/bowling/flag_meter_parts.png"),
      hammerTexture_("assets/pvz/minigames/vasebreaker/hammer.png"),
      vaseChunksTexture_("assets/pvz/minigames/vasebreaker/vase_chunks.png") {
    ui::loadChineseFont(font_);
    peashooterFrames_ = loadFrames("assets/pvz/animations/plants/peashooter", 0, 12);
    snowPeaFrames_ = loadFrames("assets/pvz/animations/plants/snowpea", 0, 14);
    squashFrames_ = loadFrames("assets/pvz/animations/plants/squash", 0, 16);
    squashAttackFrames_ = loadFrames("assets/pvz/animations/plants/squash_attack", 0, 3);
    normalZombieFrames_ = loadFrames("assets/pvz/animations/zombies/normal", 0, 21);
    bucketZombieFrames_ = loadFrames("assets/pvz/animations/zombies/buckethead", 0, 14);
    jackboxFrames_ = loadFrames("assets/pvz/animations/zombies/jackbox", 0, 7);
    normalAttackFrames_ = loadFrames("assets/pvz/animations/zombies/attack/normal", 0, 20);
    bucketAttackFrames_ = loadFrames("assets/pvz/animations/zombies/attack/buckethead", 0, 10);
    jackboxAttackFrames_ = loadFrames("assets/pvz/animations/zombies/attack/jackbox", 0, 6);
    normalDeathFrames_ = loadFrames("assets/pvz/animations/zombies/death/normal", 0, 9);
    jackboxDeathFrames_ = loadFrames("assets/pvz/animations/zombies/death/jackbox", 0, 8);
    peaHitFrames_ = loadFrames("assets/pvz/animations/effects/pea_hit", 0, 0);
    wallnutFrames_ = loadFrames("assets/pvz/animations/minigames/wallnut_roll", 0, 7);
    explodeNutFrames_ = loadFrames("assets/pvz/animations/minigames/explodenut_roll", 0, 7);

    const auto loadSound = [](sf::SoundBuffer& buffer, std::optional<sf::Sound>& sound, const char* path) {
        if (buffer.loadFromFile(path)) sound.emplace(buffer);
    };
    loadSound(bowlingBuffer_, bowlingSound_, "assets/pvz/audio/bowling.ogg");
    loadSound(impactBuffer_, impactSound_, "assets/pvz/audio/bowlingimpact.ogg");
    loadSound(impact2Buffer_, impact2Sound_, "assets/pvz/audio/bowlingimpact2.ogg");
    loadSound(vaseBreakBuffer_, vaseBreakSound_, "assets/pvz/audio/vase_breaking.ogg");
    loadSound(plantBuffer_, plantSound_, "assets/pvz/audio/plant1.ogg");
    loadSound(shootBuffer_, shootSound_, "assets/pvz/audio/shoop.ogg");
    loadSound(explosionBuffer_, explosionSound_, "assets/pvz/audio/cherrybomb.ogg");
    loadSound(jackboxBuffer_, jackboxSound_, "assets/pvz/audio/jackinthebox.ogg");
    loadSound(chompBuffer_, chompSound_, "assets/pvz/audio/chomp.ogg");
    start(ArcadeGame::Vasebreaker);
}

void ArcadeGameScreen::start(const ArcadeGame game) {
    game_ = game;
    finished_ = false;
    won_ = false;
    elapsed_ = 0.0F;
    hammerTimer_ = 0.0F;
    hoveredCell_.reset();
    if (game_ == ArcadeGame::Vasebreaker) resetVasebreaker();
    else resetBowling();
}

void ArcadeGameScreen::resetVasebreaker() {
    selectedPlant_.reset();
    selectedNut_.reset();
    seedPackets_.clear();
    plants_.clear();
    zombies_.clear();
    projectiles_.clear();
    nuts_.clear();
    conveyorCards_.clear();
    shatterEffects_.clear();
    hitEffects_.clear();
    deathEffects_.clear();
    vases_.clear();

    std::vector<VaseContent> contents;
    contents.insert(contents.end(), 5, VaseContent::Peashooter);
    contents.insert(contents.end(), 5, VaseContent::SnowPea);
    contents.insert(contents.end(), 5, VaseContent::Squash);
    contents.insert(contents.end(), 6, VaseContent::NormalZombie);
    contents.insert(contents.end(), 3, VaseContent::Buckethead);
    contents.push_back(VaseContent::Jackbox);
    std::mt19937 random(1337U);
    std::shuffle(contents.begin(), contents.end(), random);

    int greenRemaining = 2;
    for (int row = 0; row < 5; ++row) {
        for (int offset = 0; offset < 5; ++offset) {
            const VaseContent content = contents[static_cast<std::size_t>(row * 5 + offset)];
            const bool isPlant = content == VaseContent::Peashooter ||
                content == VaseContent::SnowPea || content == VaseContent::Squash;
            const bool green = isPlant && greenRemaining-- > 0;
            vases_.push_back({row, offset + 4, content, green, false});
        }
    }
}

void ArcadeGameScreen::resetBowling() {
    selectedPlant_.reset();
    selectedNut_.reset();
    plants_.clear();
    zombies_.clear();
    projectiles_.clear();
    nuts_.clear();
    vases_.clear();
    seedPackets_.clear();
    shatterEffects_.clear();
    hitEffects_.clear();
    deathEffects_.clear();
    conveyorCards_.clear();
    mowerReady_.fill(true);
    spawnTimer_ = 8.0F;
    conveyorTimer_ = 0.2F;
    spawnedZombies_ = 0;
    defeatedZombies_ = 0;
}

bool ArcadeGameScreen::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* movement = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f point = window.mapPixelToCoords(movement->position);
        const int row = static_cast<int>(std::floor((point.y - boardTop) / laneHeight));
        const int column = static_cast<int>(std::floor((point.x - lawnLeft) / cellWidth));
        if (row >= 0 && row < 5 && column >= 0 && column < 9) {
            hoveredCell_ = Cell{row, column};
        } else {
            hoveredCell_.reset();
        }
    }
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) return true;
        if (key->code == sf::Keyboard::Key::R) start(game_);
    }
    const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
    if (!click || click->button != sf::Mouse::Button::Left) return false;
    const sf::Vector2f point = window.mapPixelToCoords(click->position);
    if (point.x < 132.0F && point.y < 65.0F) return true;
    if (finished_) {
        start(game_);
        return false;
    }

    if (game_ == ArcadeGame::WallnutBowling) {
        for (auto card = conveyorCards_.begin(); card != conveyorCards_.end(); ++card) {
            if (sf::FloatRect({card->x, 8.0F}, seedPacketSize).contains(point)) {
                selectedNut_ = card->explosive;
                conveyorCards_.erase(card);
                return false;
            }
        }
        const int row = static_cast<int>((point.y - boardTop) / laneHeight);
        const int column = static_cast<int>((point.x - lawnLeft) / cellWidth);
        if (selectedNut_ && row >= 0 && row < 5 && column >= 0 &&
            column < bowlingLaunchColumns) {
            nuts_.push_back({row, cellX(column) + cellWidth * 0.5F,
                             *selectedNut_, row == 4 ? -1 : 1, 0.0F});
            selectedNut_.reset();
            playSound(bowlingSound_);
        }
        return false;
    }

    for (auto packet = seedPackets_.begin(); packet != seedPackets_.end(); ++packet) {
        const sf::FloatRect bounds(
            {cellX(packet->column) + (cellWidth - seedPacketSize.x) * 0.5F,
             laneY(packet->row) - seedPacketSize.y * 0.5F},
            seedPacketSize
        );
        if (bounds.contains(point)) {
            selectedPlant_ = packet->type;
            seedPackets_.erase(packet);
            return false;
        }
    }

    const int row = static_cast<int>((point.y - boardTop) / laneHeight);
    const int column = static_cast<int>((point.x - lawnLeft) / cellWidth);
    if (selectedPlant_ && row >= 0 && row < 5 && column >= 0 && column < 9) {
        if (canPlaceAt({row, column})) {
            plants_.push_back({row, column, *selectedPlant_, 0.5F, 100.0F});
            selectedPlant_.reset();
            playSound(plantSound_);
        }
        return false;
    }

    for (Vase& vase : vases_) {
        const sf::FloatRect bounds(
            {cellX(vase.column) + 5.0F, laneY(vase.row) - 44.0F}, {60.0F, 88.0F}
        );
        if (vase.opened || !bounds.contains(point)) continue;
        vase.opened = true;
        lastClick_ = point;
        hammerTimer_ = 0.22F;
        shatterEffects_.push_back({{cellX(vase.column) - 10.0F, laneY(vase.row) - 48.0F}, 0.0F});
        playSound(vaseBreakSound_);
        VasePlant plantType{};
        bool plant = true;
        switch (vase.content) {
            case VaseContent::Peashooter: plantType = VasePlant::Peashooter; break;
            case VaseContent::SnowPea: plantType = VasePlant::SnowPea; break;
            case VaseContent::Squash: plantType = VasePlant::Squash; break;
            case VaseContent::NormalZombie:
                plant = false;
                zombies_.push_back({vase.row, cellX(vase.column) + 35.0F, VaseZombie::Normal,
                                    100.0F, 18.0F, 0.0F, 0.0F});
                break;
            case VaseContent::Buckethead:
                plant = false;
                zombies_.push_back({vase.row, cellX(vase.column) + 35.0F, VaseZombie::Buckethead,
                                    300.0F, 15.0F, 0.0F, 0.0F});
                break;
            case VaseContent::Jackbox:
                plant = false;
                zombies_.push_back({vase.row, cellX(vase.column) + 35.0F, VaseZombie::Jackbox,
                                    150.0F, 17.0F, 0.0F, 0.0F});
                playSound(jackboxSound_);
                break;
        }
        if (plant) seedPackets_.push_back({vase.row, vase.column, plantType, 0.0F});
        break;
    }
    return false;
}

void ArcadeGameScreen::update(const float deltaSeconds) {
    elapsed_ += deltaSeconds;
    hammerTimer_ = std::max(0.0F, hammerTimer_ - deltaSeconds);
    for (ShatterEffect& effect : shatterEffects_) effect.age += deltaSeconds;
    for (HitEffect& effect : hitEffects_) effect.age += deltaSeconds;
    for (DeathEffect& effect : deathEffects_) effect.age += deltaSeconds;
    shatterEffects_.erase(
        std::remove_if(shatterEffects_.begin(), shatterEffects_.end(), [](const ShatterEffect& effect) {
            return effect.age > 0.45F;
        }), shatterEffects_.end()
    );
    hitEffects_.erase(
        std::remove_if(hitEffects_.begin(), hitEffects_.end(), [](const HitEffect& effect) {
            return effect.age > 0.22F;
        }), hitEffects_.end()
    );
    deathEffects_.erase(
        std::remove_if(deathEffects_.begin(), deathEffects_.end(), [](const DeathEffect& effect) {
            return effect.age > 0.85F;
        }), deathEffects_.end()
    );
    if (finished_) return;
    if (game_ == ArcadeGame::Vasebreaker) updateVasebreaker(deltaSeconds);
    else updateBowling(deltaSeconds);
}

void ArcadeGameScreen::updateVasebreaker(const float deltaSeconds) {
    for (SeedPacket& packet : seedPackets_) packet.age += deltaSeconds;
    for (Plant& plant : plants_) {
        plant.reload -= deltaSeconds;
        plant.damageFlash = std::max(0.0F, plant.damageFlash - deltaSeconds);
        const float plantCenter = cellX(plant.column) + cellWidth * 0.5F;
        if (plant.type == VasePlant::Squash) {
            if (plant.triggered) {
                plant.actionTimer -= deltaSeconds;
                if (plant.actionTimer <= 0.0F) plant.health = 0.0F;
                continue;
            }
            auto target = std::find_if(zombies_.begin(), zombies_.end(), [&](const Zombie& zombie) {
                return zombie.row == plant.row && std::abs(zombie.x - plantCenter) < 95.0F;
            });
            if (target != zombies_.end()) {
                target->health = 0.0F;
                plant.triggered = true;
                plant.actionTimer = 0.48F;
                playSound(impact2Sound_);
            }
            continue;
        }
        const bool hasTarget = std::any_of(zombies_.begin(), zombies_.end(), [&](const Zombie& zombie) {
            return zombie.row == plant.row && zombie.x > plantCenter;
        });
        if (hasTarget && plant.reload <= 0.0F) {
            projectiles_.push_back({plant.row, plantCenter + 24.0F, plant.type == VasePlant::SnowPea});
            plant.reload = 1.35F;
            playSound(shootSound_);
        }
    }

    for (Projectile& projectile : projectiles_) {
        projectile.x += 280.0F * deltaSeconds;
        for (Zombie& zombie : zombies_) {
            if (zombie.row != projectile.row || std::abs(zombie.x - projectile.x) >= 28.0F) continue;
            zombie.health -= 35.0F;
            zombie.damageFlash = 0.12F;
            if (projectile.frozen) zombie.speed = std::min(zombie.speed, 10.0F);
            hitEffects_.push_back({{zombie.x + 8.0F, laneY(zombie.row) - 20.0F},
                                   0.0F, projectile.frozen});
            projectile.x = 1100.0F;
            playSound(impactSound_);
            break;
        }
    }

    for (Zombie& zombie : zombies_) {
        zombie.age += deltaSeconds;
        zombie.attackTimer -= deltaSeconds;
        zombie.damageFlash = std::max(0.0F, zombie.damageFlash - deltaSeconds);
        zombie.attacking = false;
        Plant* blocker = nullptr;
        for (Plant& plant : plants_) {
            if (plant.row == zombie.row && std::abs((cellX(plant.column) + 35.0F) - zombie.x) < 35.0F) {
                blocker = &plant;
                break;
            }
        }
        if (blocker != nullptr) {
            zombie.attacking = true;
            if (zombie.attackTimer <= 0.0F) {
                blocker->health -= 24.0F;
                blocker->damageFlash = 0.16F;
                zombie.attackTimer = 1.0F;
                playSound(chompSound_);
            }
        } else {
            zombie.x -= zombie.speed * deltaSeconds;
        }
        if (zombie.type == VaseZombie::Jackbox && zombie.age > 8.5F && zombie.health > 0.0F) {
            zombie.health = 0.0F;
            for (Plant& plant : plants_) {
                if (std::abs(plant.row - zombie.row) <= 1 &&
                    std::abs((cellX(plant.column) + 35.0F) - zombie.x) < 115.0F) {
                    plant.health = 0.0F;
                }
            }
            playSound(explosionSound_);
        }
        if (zombie.x < lawnLeft) {
            finished_ = true;
            won_ = false;
        }
    }

    plants_.erase(std::remove_if(plants_.begin(), plants_.end(), [](const Plant& plant) {
        return plant.health <= 0.0F;
    }), plants_.end());
    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(), [](const Projectile& shot) {
        return shot.x > 900.0F;
    }), projectiles_.end());
    for (const Zombie& zombie : zombies_) {
        if (zombie.health <= 0.0F) {
            deathEffects_.push_back({zombie.row, zombie.x, zombie.type, 0.0F});
        }
    }
    zombies_.erase(std::remove_if(zombies_.begin(), zombies_.end(), [](const Zombie& zombie) {
        return zombie.health <= 0.0F;
    }), zombies_.end());
    const bool opened = std::all_of(vases_.begin(), vases_.end(), [](const Vase& vase) {
        return vase.opened;
    });
    if (opened && zombies_.empty() && seedPackets_.empty() && !selectedPlant_) {
        finished_ = true;
        won_ = true;
    }
}

void ArcadeGameScreen::updateBowling(const float deltaSeconds) {
    conveyorTimer_ -= deltaSeconds;
    if (conveyorTimer_ <= 0.0F && conveyorCards_.size() < 6) {
        const int produced = spawnedZombies_ + defeatedZombies_ + static_cast<int>(conveyorCards_.size());
        conveyorCards_.push_back({685.0F, produced > 0 && produced % 5 == 0});
        conveyorTimer_ = 3.0F;
    }
    std::sort(conveyorCards_.begin(), conveyorCards_.end(), [](const ConveyorCard& left, const ConveyorCard& right) {
        return left.x < right.x;
    });
    for (std::size_t index = 0; index < conveyorCards_.size(); ++index) {
        const float target = 250.0F + static_cast<float>(index) * 69.0F;
        conveyorCards_[index].x = std::max(target, conveyorCards_[index].x - 42.0F * deltaSeconds);
    }

    spawnTimer_ -= deltaSeconds;
    if (spawnedZombies_ < 24 && spawnTimer_ <= 0.0F) {
        const int row = (spawnedZombies_ * 3 + spawnedZombies_ / 4) % 5;
        const bool bucket = spawnedZombies_ >= 7 && spawnedZombies_ % 4 == 0;
        zombies_.push_back({row, 865.0F, bucket ? VaseZombie::Buckethead : VaseZombie::Normal,
                            bucket ? 300.0F : 100.0F, 16.0F + spawnedZombies_ * 0.18F,
                            0.0F, 0.0F});
        ++spawnedZombies_;
        spawnTimer_ = std::max(1.5F, 3.2F - spawnedZombies_ * 0.055F);
    }

    for (Nut& nut : nuts_) {
        nut.age += deltaSeconds;
        nut.x += 300.0F * deltaSeconds;
        for (Zombie& zombie : zombies_) {
            if (zombie.row != nut.row || std::abs(zombie.x - nut.x) >= 38.0F) continue;
            if (nut.explosive) {
                for (Zombie& nearby : zombies_) {
                    if (std::abs(nearby.row - zombie.row) <= 1 && std::abs(nearby.x - zombie.x) < 145.0F) {
                        nearby.health = 0.0F;
                    }
                }
                nut.x = 1100.0F;
                playSound(explosionSound_);
            } else {
                zombie.health -= 100.0F;
                zombie.damageFlash = 0.12F;
                hitEffects_.push_back({{zombie.x, laneY(zombie.row) - 18.0F}, 0.0F, false});
                if (nut.row == 0) nut.bounceDirection = 1;
                if (nut.row == 4) nut.bounceDirection = -1;
                nut.row = std::clamp(nut.row + nut.bounceDirection, 0, 4);
                nut.x += 45.0F;
                playSound((defeatedZombies_ % 2 == 0) ? impactSound_ : impact2Sound_);
            }
            break;
        }
    }

    for (Zombie& zombie : zombies_) {
        zombie.age += deltaSeconds;
        zombie.damageFlash = std::max(0.0F, zombie.damageFlash - deltaSeconds);
        zombie.x -= zombie.speed * deltaSeconds;
        if (zombie.x >= lawnLeft + 5.0F) continue;
        if (mowerReady_[static_cast<std::size_t>(zombie.row)]) {
            mowerReady_[static_cast<std::size_t>(zombie.row)] = false;
            for (Zombie& sameLane : zombies_) if (sameLane.row == zombie.row) sameLane.health = 0.0F;
        } else {
            finished_ = true;
            won_ = false;
        }
    }
    const auto before = zombies_.size();
    for (const Zombie& zombie : zombies_) {
        if (zombie.health <= 0.0F) {
            deathEffects_.push_back({zombie.row, zombie.x, zombie.type, 0.0F});
        }
    }
    zombies_.erase(std::remove_if(zombies_.begin(), zombies_.end(), [](const Zombie& zombie) {
        return zombie.health <= 0.0F;
    }), zombies_.end());
    defeatedZombies_ += static_cast<int>(before - zombies_.size());
    nuts_.erase(std::remove_if(nuts_.begin(), nuts_.end(), [](const Nut& nut) {
        return nut.x > 1000.0F;
    }), nuts_.end());
    if (spawnedZombies_ == 24 && zombies_.empty()) {
        finished_ = true;
        won_ = true;
    }
}

void ArcadeGameScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window, game_ == ArcadeGame::Vasebreaker);
    if (game_ == ArcadeGame::Vasebreaker) drawVasebreaker(window);
    else drawBowling(window);

    sf::RectangleShape back({112.0F, 46.0F});
    back.setPosition({10.0F, 10.0F});
    back.setFillColor({83, 47, 22});
    back.setOutlineColor({238, 194, 91});
    back.setOutlineThickness(2.0F);
    window.draw(back);
    drawText(window, "返回", 22U, {39.0F, 17.0F}, sf::Color::White);

    if (!finished_) return;
    sf::RectangleShape veil({960.0F, 600.0F});
    veil.setFillColor({10, 12, 8, 175});
    window.draw(veil);
    drawText(window, won_ ? "挑战成功" : "挑战失败", 52U, {365.0F, 230.0F},
             won_ ? sf::Color(255, 230, 90) : sf::Color(255, 150, 100));
    drawText(window, "点击重新开始，Esc 返回", 25U, {340.0F, 315.0F}, sf::Color::White);
}

void ArcadeGameScreen::drawBackground(sf::RenderWindow& window, const bool night) const {
    const sf::Texture& texture = night ? nightLawnTexture_ : dayLawnTexture_;
    const sf::Vector2u source = texture.getSize();
    const auto drawSection = [&](const int sourceLeft, const int sourceWidth,
                                 const float targetLeft, const float targetWidth) {
        sf::Sprite sprite(texture);
        sprite.setTextureRect(sf::IntRect({sourceLeft, 0}, {sourceWidth, static_cast<int>(source.y)}));
        sprite.setScale({targetWidth / static_cast<float>(sourceWidth), 600.0F / source.y});
        sprite.setPosition({targetLeft, 0.0F});
        window.draw(sprite);
    };
    drawSection(0, 230, 0.0F, lawnLeft);
    drawSection(230, 770, lawnLeft, lawnRight - lawnLeft);
    drawSection(1000, static_cast<int>(source.x) - 1000, lawnRight, 160.0F);
}

void ArcadeGameScreen::drawVasebreaker(sf::RenderWindow& window) const {
    sf::RectangleShape heading({960.0F, 82.0F});
    heading.setFillColor({27, 43, 24, 210});
    window.draw(heading);
    drawText(window, "砸罐子", 36U, {420.0F, 7.0F}, {255, 226, 118});
    drawText(window, selectedPlant_ ? "点击草坪空格放置卡片中的植物" : "用锤子敲开罐子，植物卡片需点击后再放置",
             19U, selectedPlant_ ? sf::Vector2f{328.0F, 51.0F} : sf::Vector2f{272.0F, 51.0F}, sf::Color::White);

    for (const Vase& vase : vases_) {
        if (vase.opened) continue;
        sf::Sprite sprite(vaseTexture_);
        sprite.setTextureRect(sf::IntRect({vase.green ? 80 : 0, 101}, {80, 101}));
        sprite.setScale({0.70F, 0.70F});
        sprite.setPosition({cellX(vase.column) + 7.0F, laneY(vase.row) - 40.0F});
        window.draw(sprite);
    }
    for (const SeedPacket& packet : seedPackets_) {
        const std::size_t index = static_cast<std::size_t>(packet.type);
        const auto& frames = index == 0 ? peashooterFrames_ : index == 1 ? snowPeaFrames_ : squashFrames_;
        if (!frames.empty() &&
            !(packet.age > 12.0F && static_cast<int>(packet.age * 5.0F) % 2 == 0)) {
            drawSeedPacket(
                window,
                {cellX(packet.column) + (cellWidth - seedPacketSize.x) * 0.5F,
                 laneY(packet.row) - seedPacketSize.y * 0.5F},
                frames.front()
            );
        }
    }
    drawPlacementPreview(window);
    for (const Plant& plant : plants_) drawPlant(window, plant);
    for (const Projectile& projectile : projectiles_) {
        const sf::Texture& texture = projectile.frozen ? snowProjectileTexture_ : peaProjectileTexture_;
        sf::Sprite shot(texture);
        fitSprite(shot, 24.0F, 24.0F);
        shot.setPosition({projectile.x - 12.0F, laneY(projectile.row) - 12.0F});
        window.draw(shot);
    }
    for (const Zombie& zombie : zombies_) drawZombie(window, zombie);
    for (const DeathEffect& effect : deathEffects_) {
        const auto& frames = effect.type == VaseZombie::Jackbox ? jackboxDeathFrames_ : normalDeathFrames_;
        if (frames.empty()) continue;
        const std::size_t frame = std::min(
            static_cast<std::size_t>(effect.age / 0.85F * frames.size()), frames.size() - 1
        );
        sf::Sprite sprite(frames[frame]);
        placeBottomCenteredAtHeight(sprite, effect.x, laneY(effect.row) + 47.0F, 142.0F);
        window.draw(sprite);
    }
    for (const HitEffect& effect : hitEffects_) {
        if (peaHitFrames_.empty()) continue;
        sf::Sprite hit(peaHitFrames_.front());
        fitSprite(hit, 42.0F, 42.0F);
        hit.setPosition(effect.position);
        if (effect.frozen) hit.setColor({175, 225, 255, 230});
        window.draw(hit);
    }
    for (const ShatterEffect& effect : shatterEffects_) {
        sf::Sprite chunks(vaseChunksTexture_);
        const int frame = std::min(2, static_cast<int>(effect.age / 0.15F));
        chunks.setTextureRect(sf::IntRect({frame * 96, 0}, {96, 96}));
        chunks.setPosition(effect.position);
        window.draw(chunks);
    }
    if (hammerTimer_ > 0.0F) {
        sf::Sprite hammer(hammerTexture_);
        hammer.setOrigin({12.0F, 52.0F});
        hammer.setRotation(sf::degrees(-28.0F + hammerTimer_ * 180.0F));
        hammer.setPosition(lastClick_);
        window.draw(hammer);
    }
}

void ArcadeGameScreen::drawBowling(sf::RenderWindow& window) const {
    sf::Sprite backdrop(conveyorBackdropTexture_);
    backdrop.setPosition({215.0F, 0.0F});
    window.draw(backdrop);
    sf::Sprite belt(conveyorTexture_);
    const int beltFrame = static_cast<int>(elapsed_ * 12.0F) % 6;
    belt.setTextureRect(sf::IntRect({0, beltFrame * 16}, {502, 16}));
    belt.setPosition({222.0F, 64.0F});
    window.draw(belt);
    for (const ConveyorCard& item : conveyorCards_) {
        const auto& frames = item.explosive ? explodeNutFrames_ : wallnutFrames_;
        if (!frames.empty()) drawSeedPacket(window, {item.x, 5.0F}, frames.front());
    }

    sf::Sprite stripe(bowlingStripeTexture_);
    stripe.setPosition({stripeX - 11.0F, boardTop - 4.0F});
    stripe.setScale({1.0F, 0.93F});
    window.draw(stripe);
    if (selectedNut_) {
        drawText(window, *selectedNut_ ? "爆炸坚果已选中：点击红线左侧发射" : "坚果已选中：点击红线左侧发射",
                 17U, {270.0F, 83.0F}, sf::Color::White);
    }
    drawPlacementPreview(window);

    for (int row = 0; row < 5; ++row) {
        if (!mowerReady_[static_cast<std::size_t>(row)]) continue;
        sf::Sprite mower(mowerTexture_);
        fitSprite(mower, 58.0F, 48.0F);
        mower.setPosition({122.0F, laneY(row) - 25.0F});
        window.draw(mower);
    }
    for (const Nut& nut : nuts_) {
        const auto& frames = nut.explosive ? explodeNutFrames_ : wallnutFrames_;
        if (const sf::Texture* texture = animatedFrame(frames, nut.age, 12.0F)) {
            sf::Sprite sprite(*texture);
            fitSprite(sprite, 69.0F, 69.0F);
            sprite.setOrigin({35.5F, 35.5F});
            sprite.setPosition({nut.x, laneY(nut.row)});
            window.draw(sprite);
        }
    }
    for (const Zombie& zombie : zombies_) drawZombie(window, zombie);
    for (const DeathEffect& effect : deathEffects_) {
        if (normalDeathFrames_.empty()) continue;
        const std::size_t frame = std::min(
            static_cast<std::size_t>(effect.age / 0.85F * normalDeathFrames_.size()),
            normalDeathFrames_.size() - 1
        );
        sf::Sprite sprite(normalDeathFrames_[frame]);
        placeBottomCenteredAtHeight(sprite, effect.x, laneY(effect.row) + 47.0F, 142.0F);
        window.draw(sprite);
    }
    for (const HitEffect& effect : hitEffects_) {
        if (peaHitFrames_.empty()) continue;
        sf::Sprite hit(peaHitFrames_.front());
        fitSprite(hit, 42.0F, 42.0F);
        hit.setPosition(effect.position);
        window.draw(hit);
    }

    sf::Sprite meter(flagMeterTexture_);
    meter.setPosition({785.0F, 545.0F});
    meter.setScale({1.0F, 0.85F});
    window.draw(meter);
    const float progress = static_cast<float>(defeatedZombies_) / 24.0F;
    sf::Sprite fill(flagProgressTexture_);
    const sf::Vector2u fillSize = flagProgressTexture_.getSize();
    fill.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(fillSize.x * progress), static_cast<int>(fillSize.y)}));
    fill.setPosition({807.0F, 566.0F});
    window.draw(fill);
    sf::Sprite parts(flagPartsTexture_);
    parts.setPosition({817.0F, 545.0F});
    window.draw(parts);
}

void ArcadeGameScreen::drawPlant(sf::RenderWindow& window, const Plant& plant) const {
    const std::vector<sf::Texture>* frames = &peashooterFrames_;
    if (plant.type == VasePlant::SnowPea) frames = &snowPeaFrames_;
    else if (plant.type == VasePlant::Squash) frames = &squashFrames_;
    if (plant.type == VasePlant::Squash && plant.triggered && !squashAttackFrames_.empty()) {
        frames = &squashAttackFrames_;
    }
    const sf::Texture* texture = animatedFrame(*frames, elapsed_ + plant.row * 0.13F, 9.0F);
    if (texture == nullptr) return;
    sf::Sprite sprite(*texture);
    fitSprite(sprite, 70.0F, plant.type == VasePlant::Squash ? 92.0F : 78.0F);
    const sf::FloatRect bounds = sprite.getGlobalBounds();
    sprite.setPosition({cellX(plant.column) + (cellWidth - bounds.size.x) * 0.5F,
                        laneY(plant.row) - bounds.size.y * 0.56F});
    if (plant.damageFlash > 0.0F) sprite.setColor({255, 255, 210});
    window.draw(sprite);
}

void ArcadeGameScreen::drawZombie(sf::RenderWindow& window, const Zombie& zombie) const {
    const std::vector<sf::Texture>* frames = &normalZombieFrames_;
    if (zombie.type == VaseZombie::Buckethead) frames = &bucketZombieFrames_;
    else if (zombie.type == VaseZombie::Jackbox) frames = &jackboxFrames_;
    if (zombie.attacking) {
        if (zombie.type == VaseZombie::Normal && !normalAttackFrames_.empty()) {
            frames = &normalAttackFrames_;
        } else if (zombie.type == VaseZombie::Buckethead && !bucketAttackFrames_.empty()) {
            frames = &bucketAttackFrames_;
        } else if (zombie.type == VaseZombie::Jackbox && !jackboxAttackFrames_.empty()) {
            frames = &jackboxAttackFrames_;
        }
    }
    const sf::Texture* texture = animatedFrame(*frames, zombie.age + zombie.row * 0.17F, 11.0F);
    if (texture == nullptr) return;
    sf::Sprite sprite(*texture);
    const float height = zombie.type == VaseZombie::Jackbox ? 138.0F : 145.0F;
    placeBottomCenteredAtHeight(sprite, zombie.x, laneY(zombie.row) + 47.0F, height);
    if (zombie.damageFlash > 0.0F) sprite.setColor({255, 255, 210});
    window.draw(sprite);
}

void ArcadeGameScreen::drawSeedPacket(
    sf::RenderWindow& window,
    const sf::Vector2f position,
    const sf::Texture& portrait,
    const bool selected
) const {
    sf::RectangleShape card(seedPacketSize);
    card.setPosition(position);
    card.setFillColor({206, 184, 126});
    card.setOutlineColor(selected ? sf::Color(255, 244, 94) : sf::Color(73, 47, 26));
    card.setOutlineThickness(selected ? 3.0F : 2.0F);
    window.draw(card);

    sf::RectangleShape portraitPanel({seedPacketSize.x - 6.0F, seedPacketSize.y - 8.0F});
    portraitPanel.setPosition(position + sf::Vector2f{3.0F, 3.0F});
    portraitPanel.setFillColor({70, 130, 58});
    window.draw(portraitPanel);

    sf::Sprite sprite(portrait);
    fitSprite(sprite, seedPacketSize.x - 9.0F, seedPacketSize.y - 11.0F);
    const sf::FloatRect bounds = sprite.getGlobalBounds();
    sprite.setPosition({
        position.x + (seedPacketSize.x - bounds.size.x) * 0.5F,
        position.y + seedPacketSize.y - bounds.size.y - 4.0F,
    });
    window.draw(sprite);
}

bool ArcadeGameScreen::canPlaceAt(const Cell cell) const {
    if (cell.row < 0 || cell.row >= 5 || cell.column < 0 || cell.column >= 9) return false;
    if (game_ == ArcadeGame::WallnutBowling) return cell.column < bowlingLaunchColumns;
    const bool occupied = std::any_of(plants_.begin(), plants_.end(), [&](const Plant& plant) {
        return plant.row == cell.row && plant.column == cell.column;
    });
    const bool covered = std::any_of(vases_.begin(), vases_.end(), [&](const Vase& vase) {
        return !vase.opened && vase.row == cell.row && vase.column == cell.column;
    });
    return !occupied && !covered;
}

void ArcadeGameScreen::drawPlacementPreview(sf::RenderWindow& window) const {
    const bool hasSelection = game_ == ArcadeGame::WallnutBowling
        ? selectedNut_.has_value()
        : selectedPlant_.has_value();
    if (!hasSelection || !hoveredCell_) return;

    const bool allowed = canPlaceAt(*hoveredCell_);
    sf::RectangleShape highlight({cellWidth - 4.0F, laneHeight - 4.0F});
    highlight.setPosition({cellX(hoveredCell_->column) + 2.0F,
                           boardTop + hoveredCell_->row * laneHeight + 2.0F});
    highlight.setFillColor(allowed ? sf::Color(100, 255, 110, 55) : sf::Color(255, 70, 70, 75));
    highlight.setOutlineColor(allowed ? sf::Color(190, 255, 190) : sf::Color(255, 150, 150));
    highlight.setOutlineThickness(2.0F);
    window.draw(highlight);

    if (!allowed) return;
    if (game_ == ArcadeGame::WallnutBowling) {
        const auto& frames = *selectedNut_ ? explodeNutFrames_ : wallnutFrames_;
        if (frames.empty()) return;
        sf::Sprite preview(frames.front());
        fitSprite(preview, 64.0F, 64.0F);
        preview.setColor({255, 255, 255, 155});
        const sf::FloatRect bounds = preview.getGlobalBounds();
        preview.setPosition({cellX(hoveredCell_->column) + (cellWidth - bounds.size.x) * 0.5F,
                             laneY(hoveredCell_->row) - bounds.size.y * 0.5F});
        window.draw(preview);
        return;
    }

    const std::vector<sf::Texture>* frames = &peashooterFrames_;
    if (*selectedPlant_ == VasePlant::SnowPea) frames = &snowPeaFrames_;
    else if (*selectedPlant_ == VasePlant::Squash) frames = &squashFrames_;
    if (frames->empty()) return;
    sf::Sprite preview(frames->front());
    fitSprite(preview, 70.0F, *selectedPlant_ == VasePlant::Squash ? 92.0F : 78.0F);
    preview.setColor({180, 255, 180, 165});
    const sf::FloatRect bounds = preview.getGlobalBounds();
    preview.setPosition({cellX(hoveredCell_->column) + (cellWidth - bounds.size.x) * 0.5F,
                         laneY(hoveredCell_->row) - bounds.size.y * 0.56F});
    window.draw(preview);
}

void ArcadeGameScreen::drawText(sf::RenderWindow& window, const char* value,
                                const unsigned int size, const sf::Vector2f position,
                                const sf::Color color) const {
    sf::Text text(font_, ui::utf8(value), size);
    text.setPosition(position);
    text.setFillColor(color);
    text.setOutlineColor({45, 28, 14});
    text.setOutlineThickness(1.0F);
    window.draw(text);
}

void ArcadeGameScreen::playSound(std::optional<sf::Sound>& sound) {
    if (!sound) return;
    sound->setVolume(72.0F);
    sound->play();
}

}  // namespace object_project
