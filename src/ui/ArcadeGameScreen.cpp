#include "ui/ArcadeGameScreen.hpp"

#include "ui/UiText.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace object_project {
namespace {
constexpr float boardLeft = 150.0F;
constexpr float boardTop = 100.0F;
constexpr float laneHeight = 92.0F;
constexpr float laneY(const int row) { return boardTop + row * laneHeight + laneHeight * 0.5F; }
}

ArcadeGameScreen::ArcadeGameScreen()
    : fontLoaded_(ui::loadChineseFont(font_)),
      lawnLoaded_(lawnTexture_.loadFromFile("assets/background/lawn.jpg")),
      vaseLoaded_(vaseTexture_.loadFromFile("assets/pvz/minigames/scary_pot.png")),
      plantLoaded_(plantTexture_.loadFromFile("assets/pvz/plants/peashooter.png")),
      zombieLoaded_(zombieTexture_.loadFromFile("assets/pvz/zombies/normal.png")),
      wallnutLoaded_(wallnutTexture_.loadFromFile("assets/pvz/minigames/wallnut_roll.gif")),
      boomWallnutLoaded_(boomWallnutTexture_.loadFromFile(
          "assets/pvz/minigames/boom_wallnut_roll.gif"
      )) {
    if (bowlingBuffer_.loadFromFile("assets/pvz/audio/bowling.ogg")) {
        bowlingSound_.emplace(bowlingBuffer_);
    }
    if (impactBuffer_.loadFromFile("assets/pvz/audio/bowlingimpact.ogg")) {
        impactSound_.emplace(impactBuffer_);
    }
    start(ArcadeGame::Vasebreaker);
}

void ArcadeGameScreen::start(const ArcadeGame game) {
    game_ = game;
    finished_ = false;
    won_ = false;
    if (game_ == ArcadeGame::Vasebreaker) {
        resetVasebreaker();
    } else {
        resetBowling();
    }
}

void ArcadeGameScreen::resetVasebreaker() {
    heldPlant_ = false;
    plants_.clear();
    zombies_.clear();
    projectiles_.clear();
    nuts_.clear();
    vases_.clear();
    for (int row = 0; row < 5; ++row) {
        for (int column = 0; column < 4; ++column) {
            vases_.push_back({row, column, false, (row * 4 + column) % 3 == 0});
        }
    }
}

void ArcadeGameScreen::resetBowling() {
    heldPlant_ = false;
    plants_.clear();
    zombies_.clear();
    projectiles_.clear();
    nuts_.clear();
    spawnTimer_ = 2.0F;
    nutCooldown_ = 0.0F;
    spawnedZombies_ = 0;
    defeatedZombies_ = 0;
}

bool ArcadeGameScreen::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            return true;
        }
        if (key->code == sf::Keyboard::Key::R) {
            start(game_);
        }
    }
    const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
    if (!click || click->button != sf::Mouse::Button::Left) {
        return false;
    }
    const sf::Vector2f point = window.mapPixelToCoords(click->position);
    if (point.x < 135.0F && point.y < 70.0F) {
        return true;
    }
    if (finished_) {
        start(game_);
        return false;
    }

    const int row = static_cast<int>((point.y - boardTop) / laneHeight);
    if (row < 0 || row >= 5) {
        return false;
    }
    if (game_ == ArcadeGame::WallnutBowling) {
        if (point.x >= boardLeft && point.x <= 310.0F && nutCooldown_ <= 0.0F) {
            const bool explosive = (defeatedZombies_ + static_cast<int>(nuts_.size()) + 1) % 5 == 0;
            nuts_.push_back({row, boardLeft + 35.0F, explosive, 0.0F});
            nutCooldown_ = 0.75F;
            playBowling(false);
        }
        return false;
    }

    if (heldPlant_ && point.x >= 180.0F && point.x < 500.0F) {
        const int column = std::clamp(static_cast<int>((point.x - 180.0F) / 78.0F), 0, 3);
        const bool occupied = std::any_of(plants_.begin(), plants_.end(), [row, column](const Plant& p) {
            return p.row == row && p.column == column;
        });
        if (!occupied) {
            plants_.push_back({row, column, 0.6F});
            heldPlant_ = false;
        }
        return false;
    }
    for (Vase& vase : vases_) {
        const sf::FloatRect bounds(
            {545.0F + vase.column * 82.0F, laneY(vase.row) - 42.0F},
            {66.0F, 84.0F}
        );
        if (!vase.opened && bounds.contains(point)) {
            vase.opened = true;
            if (vase.plant) {
                heldPlant_ = true;
            } else {
                const float health = 90.0F + static_cast<float>((vase.row + vase.column) % 3) * 35.0F;
                zombies_.push_back({vase.row, 570.0F + vase.column * 82.0F, health, 17.0F});
            }
            break;
        }
    }
    return false;
}

void ArcadeGameScreen::update(const float deltaSeconds) {
    if (finished_) {
        return;
    }
    if (game_ == ArcadeGame::Vasebreaker) {
        updateVasebreaker(deltaSeconds);
    } else {
        updateBowling(deltaSeconds);
    }
}

void ArcadeGameScreen::updateVasebreaker(const float deltaSeconds) {
    for (Plant& plant : plants_) {
        plant.reload -= deltaSeconds;
        const bool hasTarget = std::any_of(zombies_.begin(), zombies_.end(), [&plant](const Zombie& z) {
            return z.row == plant.row && z.x > 180.0F + plant.column * 78.0F;
        });
        if (hasTarget && plant.reload <= 0.0F) {
            projectiles_.push_back({plant.row, 238.0F + plant.column * 78.0F});
            plant.reload = 1.35F;
        }
    }
    for (Projectile& projectile : projectiles_) {
        projectile.x += 280.0F * deltaSeconds;
        for (Zombie& zombie : zombies_) {
            if (zombie.row == projectile.row && std::abs(zombie.x - projectile.x) < 25.0F) {
                zombie.health -= 35.0F;
                projectile.x = 1100.0F;
                break;
            }
        }
    }
    for (Zombie& zombie : zombies_) {
        zombie.x -= zombie.speed * deltaSeconds;
        if (zombie.x < 145.0F) {
            finished_ = true;
            won_ = false;
        }
    }
    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(), [](const Projectile& p) {
        return p.x > 950.0F;
    }), projectiles_.end());
    zombies_.erase(std::remove_if(zombies_.begin(), zombies_.end(), [](const Zombie& z) {
        return z.health <= 0.0F;
    }), zombies_.end());
    const bool opened = std::all_of(vases_.begin(), vases_.end(), [](const Vase& vase) {
        return vase.opened;
    });
    if (opened && zombies_.empty() && !heldPlant_) {
        finished_ = true;
        won_ = true;
    }
}

void ArcadeGameScreen::updateBowling(const float deltaSeconds) {
    nutCooldown_ = std::max(0.0F, nutCooldown_ - deltaSeconds);
    spawnTimer_ -= deltaSeconds;
    if (spawnedZombies_ < 24 && spawnTimer_ <= 0.0F) {
        const int row = (spawnedZombies_ * 3 + spawnedZombies_ / 5) % 5;
        const float health = 70.0F + static_cast<float>(spawnedZombies_ / 8) * 35.0F;
        zombies_.push_back({row, 915.0F, health, 18.0F + spawnedZombies_ * 0.25F});
        ++spawnedZombies_;
        spawnTimer_ = std::max(1.15F, 2.6F - spawnedZombies_ * 0.045F);
    }
    for (Nut& nut : nuts_) {
        nut.x += 300.0F * deltaSeconds;
        nut.angle += 360.0F * deltaSeconds;
        for (Zombie& zombie : zombies_) {
            if (zombie.row != nut.row || std::abs(zombie.x - nut.x) >= 37.0F) {
                continue;
            }
            if (nut.explosive) {
                for (Zombie& nearby : zombies_) {
                    if (std::abs(nearby.row - zombie.row) <= 1 && std::abs(nearby.x - zombie.x) < 150.0F) {
                        nearby.health = 0.0F;
                    }
                }
            } else {
                zombie.health -= 100.0F;
                nut.row = std::clamp(nut.row + (nut.row < 2 ? 1 : -1), 0, 4);
            }
            nut.x = nut.explosive ? 1100.0F : nut.x + 45.0F;
            playBowling(true);
            break;
        }
    }
    for (Zombie& zombie : zombies_) {
        zombie.x -= zombie.speed * deltaSeconds;
        if (zombie.x < 135.0F) {
            finished_ = true;
            won_ = false;
        }
    }
    const auto before = zombies_.size();
    zombies_.erase(std::remove_if(zombies_.begin(), zombies_.end(), [](const Zombie& z) {
        return z.health <= 0.0F;
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
    drawBoard(window);
    if (game_ == ArcadeGame::Vasebreaker) {
        drawVasebreaker(window);
    } else {
        drawBowling(window);
    }
    sf::RectangleShape back({115.0F, 48.0F});
    back.setPosition({10.0F, 10.0F});
    back.setFillColor({83, 47, 22});
    back.setOutlineColor({238, 194, 91});
    back.setOutlineThickness(2.0F);
    window.draw(back);
    drawText(window, "返回", 22U, {39.0F, 18.0F}, sf::Color::White);
    if (finished_) {
        sf::RectangleShape veil({960.0F, 600.0F});
        veil.setFillColor({10, 12, 8, 170});
        window.draw(veil);
        drawText(window, won_ ? "挑战成功" : "挑战失败", 52U, {365.0F, 230.0F},
                 won_ ? sf::Color(255, 230, 90) : sf::Color(255, 150, 100));
        drawText(window, "点击重新开始，Esc 返回", 25U, {340.0F, 315.0F}, sf::Color::White);
    }
}

void ArcadeGameScreen::drawBoard(sf::RenderWindow& window) const {
    if (lawnLoaded_) {
        sf::Sprite lawn(lawnTexture_);
        const sf::Vector2u size = lawnTexture_.getSize();
        lawn.setScale({960.0F / size.x, 600.0F / size.y});
        window.draw(lawn);
    }
    sf::RectangleShape top({960.0F, 88.0F});
    top.setFillColor({42, 55, 24, 220});
    window.draw(top);
    for (int row = 0; row <= 5; ++row) {
        sf::RectangleShape line({780.0F, 1.0F});
        line.setPosition({boardLeft, boardTop + row * laneHeight});
        line.setFillColor({230, 255, 185, 80});
        window.draw(line);
    }
}

void ArcadeGameScreen::drawVasebreaker(sf::RenderWindow& window) const {
    drawText(window, "砸罐子", 34U, {420.0F, 23.0F}, {255, 232, 130});
    drawText(window, heldPlant_ ? "已获得豌豆射手：点击左侧空地放置" : "点击罐子逐步揭开内容",
             19U, {285.0F, 66.0F}, sf::Color::White);
    for (const Vase& vase : vases_) {
        if (vase.opened || !vaseLoaded_) {
            continue;
        }
        sf::Sprite sprite(vaseTexture_);
        sprite.setTextureRect(sf::IntRect({0, 101}, {80, 101}));
        sprite.setScale({0.78F, 0.78F});
        sprite.setPosition({545.0F + vase.column * 82.0F, laneY(vase.row) - 40.0F});
        window.draw(sprite);
    }
    for (const Plant& plant : plants_) {
        if (!plantLoaded_) continue;
        sf::Sprite sprite(plantTexture_);
        const sf::Vector2u size = plantTexture_.getSize();
        sprite.setScale({65.0F / size.x, 76.0F / size.y});
        sprite.setPosition({185.0F + plant.column * 78.0F, laneY(plant.row) - 39.0F});
        window.draw(sprite);
    }
    for (const Projectile& projectile : projectiles_) {
        sf::CircleShape pea(7.0F);
        pea.setPosition({projectile.x, laneY(projectile.row) - 7.0F});
        pea.setFillColor({165, 233, 42});
        pea.setOutlineColor({40, 80, 20});
        pea.setOutlineThickness(2.0F);
        window.draw(pea);
    }
    for (const Zombie& zombie : zombies_) {
        if (!zombieLoaded_) continue;
        sf::Sprite sprite(zombieTexture_);
        const sf::Vector2u size = zombieTexture_.getSize();
        sprite.setScale({72.0F / size.x, 105.0F / size.y});
        sprite.setPosition({zombie.x - 36.0F, laneY(zombie.row) - 58.0F});
        window.draw(sprite);
    }
}

void ArcadeGameScreen::drawBowling(sf::RenderWindow& window) const {
    drawText(window, "坚果保龄球", 34U, {385.0F, 20.0F}, {255, 232, 130});
    const std::string status = "击倒 " + std::to_string(defeatedZombies_) + " / 24";
    drawText(window, status.c_str(), 21U, {740.0F, 31.0F}, sf::Color::White);
    sf::RectangleShape launch({155.0F, 460.0F});
    launch.setPosition({boardLeft, boardTop});
    launch.setFillColor({92, 25, 22, 62});
    launch.setOutlineColor({220, 80, 70});
    launch.setOutlineThickness(3.0F);
    window.draw(launch);
    drawText(window, nutCooldown_ <= 0.0F ? "点击任意行发射" : "坚果准备中", 18U,
             {160.0F, 67.0F}, sf::Color::White);
    for (const Nut& nut : nuts_) {
        const sf::Texture& texture = nut.explosive ? boomWallnutTexture_ : wallnutTexture_;
        const bool loaded = nut.explosive ? boomWallnutLoaded_ : wallnutLoaded_;
        if (!loaded) continue;
        sf::Sprite sprite(texture);
        sprite.setOrigin({35.5F, 35.5F});
        sprite.setPosition({nut.x, laneY(nut.row)});
        sprite.setRotation(sf::degrees(nut.angle));
        window.draw(sprite);
    }
    for (const Zombie& zombie : zombies_) {
        if (!zombieLoaded_) continue;
        sf::Sprite sprite(zombieTexture_);
        const sf::Vector2u size = zombieTexture_.getSize();
        sprite.setScale({72.0F / size.x, 105.0F / size.y});
        sprite.setPosition({zombie.x - 36.0F, laneY(zombie.row) - 58.0F});
        window.draw(sprite);
    }
}

void ArcadeGameScreen::drawText(sf::RenderWindow& window, const char* value,
                                const unsigned int size, const sf::Vector2f position,
                                const sf::Color color) const {
    if (!fontLoaded_) return;
    sf::Text text(font_, ui::utf8(value), size);
    text.setPosition(position);
    text.setFillColor(color);
    text.setOutlineColor({45, 28, 14});
    text.setOutlineThickness(1.0F);
    window.draw(text);
}

void ArcadeGameScreen::playBowling(const bool impact) {
    auto& sound = impact ? impactSound_ : bowlingSound_;
    if (sound) {
        sound->setVolume(70.0F);
        sound->play();
    }
}

}  // namespace object_project
