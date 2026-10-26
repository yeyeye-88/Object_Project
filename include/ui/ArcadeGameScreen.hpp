#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <vector>

namespace object_project {

enum class ArcadeGame { Vasebreaker, WallnutBowling };

class ArcadeGameScreen {
public:
    ArcadeGameScreen();

    void start(ArcadeGame game);
    bool handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float deltaSeconds);
    void draw(sf::RenderWindow& window) const;

private:
    enum class VaseContent { Peashooter, SnowPea, Squash, NormalZombie, Buckethead, Jackbox };
    enum class VasePlant { Peashooter, SnowPea, Squash };
    enum class VaseZombie { Normal, Buckethead, Jackbox };

    struct Vase { int row{}; int column{}; VaseContent content{}; bool green{}; bool opened{}; };
    struct SeedPacket { int row{}; int column{}; VasePlant type{}; float age{}; };
    struct Cell { int row{}; int column{}; };
    struct Plant {
        int row{};
        int column{};
        VasePlant type{};
        float reload{};
        float health{100.0F};
        float damageFlash{};
        float actionTimer{};
        bool triggered{};
    };
    struct Zombie {
        int row{};
        float x{};
        VaseZombie type{};
        float health{100.0F};
        float speed{18.0F};
        float age{};
        float attackTimer{};
        float damageFlash{};
        bool attacking{};
    };
    struct Projectile { int row{}; float x{}; bool frozen{}; };
    struct Nut { int row{}; float x{}; bool explosive{}; int bounceDirection{1}; float age{}; };
    struct ConveyorCard { float x{}; bool explosive{}; };
    struct ShatterEffect { sf::Vector2f position{}; float age{}; };
    struct HitEffect { sf::Vector2f position{}; float age{}; bool frozen{}; };
    struct DeathEffect { int row{}; float x{}; VaseZombie type{}; float age{}; };

    void resetVasebreaker();
    void resetBowling();
    void updateVasebreaker(float deltaSeconds);
    void updateBowling(float deltaSeconds);
    void drawBackground(sf::RenderWindow& window, bool night) const;
    void drawVasebreaker(sf::RenderWindow& window) const;
    void drawBowling(sf::RenderWindow& window) const;
    void drawZombie(sf::RenderWindow& window, const Zombie& zombie) const;
    void drawPlant(sf::RenderWindow& window, const Plant& plant) const;
    void drawSeedPacket(sf::RenderWindow& window, sf::Vector2f position,
                        const sf::Texture& portrait, bool selected = false) const;
    void drawPlacementPreview(sf::RenderWindow& window) const;
    bool canPlaceAt(Cell cell) const;
    void drawText(sf::RenderWindow& window, const char* value, unsigned int size,
                  sf::Vector2f position, sf::Color color) const;
    void playSound(std::optional<sf::Sound>& sound);

    ArcadeGame game_{ArcadeGame::Vasebreaker};
    bool finished_{};
    bool won_{};
    float elapsed_{};
    float spawnTimer_{};
    float conveyorTimer_{};
    int spawnedZombies_{};
    int defeatedZombies_{};
    std::optional<VasePlant> selectedPlant_;
    std::optional<bool> selectedNut_;
    std::optional<Cell> hoveredCell_;
    sf::Vector2f lastClick_{};
    float hammerTimer_{};
    std::array<bool, 5> mowerReady_{};
    std::vector<Vase> vases_;
    std::vector<SeedPacket> seedPackets_;
    std::vector<Plant> plants_;
    std::vector<Zombie> zombies_;
    std::vector<Projectile> projectiles_;
    std::vector<Nut> nuts_;
    std::vector<ConveyorCard> conveyorCards_;
    std::vector<ShatterEffect> shatterEffects_;
    std::vector<HitEffect> hitEffects_;
    std::vector<DeathEffect> deathEffects_;

    sf::Font font_;
    sf::Texture dayLawnTexture_;
    sf::Texture nightLawnTexture_;
    sf::Texture vaseTexture_;
    sf::Texture peaProjectileTexture_;
    sf::Texture snowProjectileTexture_;
    sf::Texture conveyorBackdropTexture_;
    sf::Texture conveyorTexture_;
    sf::Texture bowlingStripeTexture_;
    sf::Texture mowerTexture_;
    sf::Texture flagMeterTexture_;
    sf::Texture flagProgressTexture_;
    sf::Texture flagPartsTexture_;
    sf::Texture hammerTexture_;
    sf::Texture vaseChunksTexture_;
    std::vector<sf::Texture> peashooterFrames_;
    std::vector<sf::Texture> snowPeaFrames_;
    std::vector<sf::Texture> squashFrames_;
    std::vector<sf::Texture> squashAttackFrames_;
    std::vector<sf::Texture> normalZombieFrames_;
    std::vector<sf::Texture> bucketZombieFrames_;
    std::vector<sf::Texture> jackboxFrames_;
    std::vector<sf::Texture> normalAttackFrames_;
    std::vector<sf::Texture> bucketAttackFrames_;
    std::vector<sf::Texture> jackboxAttackFrames_;
    std::vector<sf::Texture> normalDeathFrames_;
    std::vector<sf::Texture> jackboxDeathFrames_;
    std::vector<sf::Texture> peaHitFrames_;
    std::vector<sf::Texture> wallnutFrames_;
    std::vector<sf::Texture> explodeNutFrames_;

    sf::SoundBuffer bowlingBuffer_;
    sf::SoundBuffer impactBuffer_;
    sf::SoundBuffer impact2Buffer_;
    sf::SoundBuffer vaseBreakBuffer_;
    sf::SoundBuffer plantBuffer_;
    sf::SoundBuffer shootBuffer_;
    sf::SoundBuffer explosionBuffer_;
    sf::SoundBuffer jackboxBuffer_;
    sf::SoundBuffer chompBuffer_;
    std::optional<sf::Sound> bowlingSound_;
    std::optional<sf::Sound> impactSound_;
    std::optional<sf::Sound> impact2Sound_;
    std::optional<sf::Sound> vaseBreakSound_;
    std::optional<sf::Sound> plantSound_;
    std::optional<sf::Sound> shootSound_;
    std::optional<sf::Sound> explosionSound_;
    std::optional<sf::Sound> jackboxSound_;
    std::optional<sf::Sound> chompSound_;
};

}  // namespace object_project
