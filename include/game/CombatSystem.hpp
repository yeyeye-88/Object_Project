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
    enum class Type {
        Pea,
        FrozenPea,
        FirePea,
    } type{Type::Pea};
};

struct AreaDamage {
    int centerRow{};
    float centerX{};
    int rowRadius{};
    float horizontalRadius{};
    int damage{};
};

struct DelayedAreaDamage {
    std::uint64_t id{};
    AreaDamage area{};
    float remainingSeconds{};
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

enum class CombatEvent {
    ProjectileFired,
    ProjectileHit,
    Explosion,
    MowerStarted,
    PoleVaulted,
    NewspaperBroken,
    ZombieBite,
    FirePeaIgnited,
};

class CombatSystem {
public:
    static constexpr float zombieWidth = 46.0F;
    static constexpr float projectileSpeed = 240.0F;
    static constexpr int projectileDamage = 20;
    static constexpr float mowerStartX = BoardGeometry::left - 54.0F;
    static constexpr float defenseLineX = BoardGeometry::left - 12.0F;
    static constexpr float mowerSpeed = 520.0F;
    static constexpr float frozenPeaSpeedMultiplier = 0.5F;
    static constexpr float frozenPeaDurationSeconds = 3.0F;
    static constexpr float potatoMineTriggerRadius = BoardGeometry::cellWidth * 0.65F;
    static constexpr int potatoMineDamage = 300;
    static constexpr int cherryBombDamage = 500;
    static constexpr float cherryBombHorizontalRadius = BoardGeometry::cellWidth * 1.5F;
    static constexpr int firePeaDamageMultiplier = 2;

    CombatSystem();

    Zombie& spawnZombie(
        ZombieType type,
        int row,
        float x = BoardGeometry::left + BoardGeometry::columns * BoardGeometry::cellWidth
    );
    Zombie& spawnNormalZombie(
        int row,
        float x = BoardGeometry::left + BoardGeometry::columns * BoardGeometry::cellWidth
    );
    void update(float deltaSeconds, Board& board, SunSystem& suns);
    void reset();
    void launchProjectile(
        Projectile::Type type,
        int row,
        float x,
        int damage = projectileDamage
    );
    void applyAreaDamage(const AreaDamage& area);
    void scheduleAreaDamage(const AreaDamage& area, float delaySeconds);
    std::vector<CombatEvent> takeEvents();

    bool breached() const { return breached_; }
    bool hasLivingZombies() const { return !zombies_.empty(); }
    const std::vector<std::unique_ptr<Zombie>>& zombies() const { return zombies_; }
    const std::vector<Projectile>& projectiles() const { return projectiles_; }
    const std::vector<DelayedAreaDamage>& delayedAreaDamages() const {
        return delayedAreaDamages_;
    }
    const std::array<LawnMower, BoardGeometry::rows>& mowers() const { return mowers_; }

private:
    bool hasZombieAhead(int row, float x) const;
    bool hasZombieNear(int row, float x, float radius) const;
    void damageNearestZombie(int row, float x, float radius, int damage);
    void updatePlants(float deltaSeconds, Board& board, SunSystem& suns);
    void updateProjectiles(float deltaSeconds, const Board& board);
    void updateDelayedAreaDamages(float deltaSeconds);
    void updateZombies(float deltaSeconds, Board& board);
    void updateMowers(float deltaSeconds);
    void removeDead();

    std::vector<std::unique_ptr<Zombie>> zombies_;
    std::vector<Projectile> projectiles_;
    std::vector<DelayedAreaDamage> delayedAreaDamages_;
    std::vector<CombatEvent> events_;
    std::array<LawnMower, BoardGeometry::rows> mowers_{};
    std::uint64_t nextProjectileId_{1};
    std::uint64_t nextAreaDamageId_{1};
    bool breached_{};
};

}  // namespace object_project
