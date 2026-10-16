#pragma once

#include "game/BoardGeometry.hpp"
#include "game/Zombie.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace object_project {

class Board;
class SunSystem;

struct Projectile {
    std::uint64_t id{};
    int row{};
    float x{};
    int damage{};
};

enum class LawnMowerState {
    Ready,
    Active,
    Used,
};

struct LawnMower {
    int row{};
    float x{};
    LawnMowerState state{LawnMowerState::Ready};
};

class CombatSystem {
public:
    static constexpr float zombieWidth = 46.0F;
    static constexpr float projectileSpeed = 240.0F;
    static constexpr int projectileDamage = 20;
    static constexpr float mowerStartX = BoardGeometry::left - 54.0F;
    static constexpr float defenseLineX = BoardGeometry::left - 12.0F;
    static constexpr float mowerSpeed = 520.0F;

    CombatSystem();

    Zombie& spawnNormalZombie(int row, float x = BoardGeometry::left + BoardGeometry::columns * BoardGeometry::cellWidth);
    void update(float deltaSeconds, Board& board, SunSystem& suns);
    void reset();

    bool breached() const { return breached_; }
    bool hasLivingZombies() const { return !zombies_.empty(); }
    const std::vector<std::unique_ptr<Zombie>>& zombies() const { return zombies_; }
    const std::vector<Projectile>& projectiles() const { return projectiles_; }
    const std::array<LawnMower, BoardGeometry::rows>& mowers() const { return mowers_; }

private:
    bool hasZombieAhead(int row, float x) const;
    void updatePlants(float deltaSeconds, Board& board, SunSystem& suns);
    void updateProjectiles(float deltaSeconds);
    void updateZombies(float deltaSeconds, Board& board);
    void updateMowers(float deltaSeconds);
    void removeDead();

    std::vector<std::unique_ptr<Zombie>> zombies_;
    std::vector<Projectile> projectiles_;
    std::array<LawnMower, BoardGeometry::rows> mowers_{};
    std::uint64_t nextProjectileId_{1};
    bool breached_{};
};

}  // namespace object_project
