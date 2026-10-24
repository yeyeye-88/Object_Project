#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

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
    struct Vase { int row{}; int column{}; bool opened{}; bool plant{}; };
    struct Plant { int row{}; int column{}; float reload{}; };
    struct Zombie { int row{}; float x{}; float health{100.0F}; float speed{18.0F}; };
    struct Projectile { int row{}; float x{}; };
    struct Nut { int row{}; float x{}; bool explosive{}; float angle{}; };

    void resetVasebreaker();
    void resetBowling();
    void updateVasebreaker(float deltaSeconds);
    void updateBowling(float deltaSeconds);
    void drawBoard(sf::RenderWindow& window) const;
    void drawVasebreaker(sf::RenderWindow& window) const;
    void drawBowling(sf::RenderWindow& window) const;
    void drawText(sf::RenderWindow& window, const char* value, unsigned int size,
                  sf::Vector2f position, sf::Color color) const;
    void playBowling(bool impact);

    ArcadeGame game_{ArcadeGame::Vasebreaker};
    bool finished_{};
    bool won_{};
    bool heldPlant_{};
    float spawnTimer_{};
    float nutCooldown_{};
    int spawnedZombies_{};
    int defeatedZombies_{};
    std::vector<Vase> vases_;
    std::vector<Plant> plants_;
    std::vector<Zombie> zombies_;
    std::vector<Projectile> projectiles_;
    std::vector<Nut> nuts_;

    sf::Font font_;
    sf::Texture lawnTexture_;
    sf::Texture vaseTexture_;
    sf::Texture plantTexture_;
    sf::Texture zombieTexture_;
    sf::Texture wallnutTexture_;
    sf::Texture boomWallnutTexture_;
    bool fontLoaded_{};
    bool lawnLoaded_{};
    bool vaseLoaded_{};
    bool plantLoaded_{};
    bool zombieLoaded_{};
    bool wallnutLoaded_{};
    bool boomWallnutLoaded_{};
    sf::SoundBuffer bowlingBuffer_;
    sf::SoundBuffer impactBuffer_;
    std::optional<sf::Sound> bowlingSound_;
    std::optional<sf::Sound> impactSound_;
};

}  // namespace object_project
