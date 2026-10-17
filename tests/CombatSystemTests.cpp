#include "game/Board.hpp"
#include "game/CombatSystem.hpp"
#include "game/SunSystem.hpp"

#include <iostream>

namespace {

bool check(const bool condition, const char* name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << '\n';
    return condition;
}

}  // namespace

int main() {
    using namespace object_project;
    bool passed = true;
    Board board;
    SunSystem suns(7U);
    CombatSystem combat;

    board.place({1, 1}, PlantType::Sunflower);
    combat.update(Sunflower::productionIntervalSeconds, board, suns);
    passed &= check(suns.tokens().size() == 1, "sunflower adds a collectible sun token");

    board.place({2, 0}, PlantType::Peashooter);
    Zombie& target = combat.spawnNormalZombie(2, 500.0F);
    for (int step = 0; step < 35; ++step) {
        combat.update(0.1F, board, suns);
    }
    passed &= check(target.health() < NormalZombie::maximumHealth, "pea projectile damages same-row zombie");

    board.place({3, 8}, PlantType::WallNut);
    Plant* wallNut = board.plantAt({3, 8});
    combat.spawnNormalZombie(3, BoardGeometry::cellLeft(8) + 20.0F);
    const int healthBeforeAttack = wallNut->health();
    combat.update(NormalZombie::attackInterval, board, suns);
    passed &= check(wallNut->health() < healthBeforeAttack, "blocked zombie attacks plant");

    CombatSystem mowerCombat;
    Board emptyBoard;
    SunSystem emptySuns(9U);
    mowerCombat.spawnNormalZombie(0, CombatSystem::defenseLineX - 1.0F);
    mowerCombat.update(0.1F, emptyBoard, emptySuns);
    passed &= check(
        mowerCombat.mowers()[0].state == LawnMowerState::Active,
        "first breach activates row mower"
    );
    mowerCombat.update(2.0F, emptyBoard, emptySuns);
    passed &= check(
        mowerCombat.mowers()[0].state == LawnMowerState::Used,
        "mower becomes unavailable after crossing lawn"
    );
    mowerCombat.spawnNormalZombie(0, CombatSystem::defenseLineX - 1.0F);
    mowerCombat.update(0.1F, emptyBoard, emptySuns);
    passed &= check(mowerCombat.breached(), "second breach in same row causes defeat condition");

    CombatSystem repeaterCombat;
    Board repeaterBoard;
    SunSystem repeaterSuns(11U);
    repeaterBoard.place({2, 0}, PlantType::Repeater);
    repeaterCombat.spawnZombie(ZombieType::Conehead, 2, 800.0F);
    repeaterCombat.update(Repeater::attackIntervalSeconds, repeaterBoard, repeaterSuns);
    passed &= check(repeaterCombat.projectiles().size() == 2, "repeater fires two projectiles per attack");

    CombatSystem threepeaterCombat;
    Board threepeaterBoard;
    SunSystem threepeaterSuns(13U);
    threepeaterBoard.place({2, 0}, PlantType::Threepeater);
    threepeaterCombat.spawnNormalZombie(1, 800.0F);
    threepeaterCombat.update(Threepeater::attackIntervalSeconds, threepeaterBoard, threepeaterSuns);
    passed &= check(
        threepeaterCombat.projectiles().size() == 3,
        "threepeater fires into its lane and adjacent lanes"
    );

    CombatSystem effectCombat;
    Board effectBoard;
    SunSystem effectSuns(15U);
    Zombie& frozenTarget = effectCombat.spawnNormalZombie(1, 400.0F);
    effectCombat.launchProjectile(Projectile::Type::FrozenPea, 1, 390.0F);
    effectCombat.update(0.01F, effectBoard, effectSuns);
    passed &= check(
        frozenTarget.isSlowed() && frozenTarget.health() < NormalZombie::maximumHealth,
        "frozen projectile damages and slows its target"
    );

    Zombie& centerTarget = effectCombat.spawnZombie(ZombieType::Buckethead, 2, 500.0F);
    Zombie& adjacentTarget = effectCombat.spawnZombie(ZombieType::Conehead, 3, 515.0F);
    Zombie& distantTarget = effectCombat.spawnNormalZombie(4, 700.0F);
    const AreaDamage blast{2, 523.0F, 1, 80.0F, 60};
    effectCombat.applyAreaDamage(blast);
    passed &= check(
        centerTarget.armor() == BucketheadZombie::maximumArmor - 60 &&
            adjacentTarget.armor() == ConeheadZombie::maximumArmor - 60 &&
            distantTarget.health() == NormalZombie::maximumHealth,
        "area damage hits only zombies inside row and horizontal radius"
    );

    const int armorBeforeDelay = centerTarget.armor();
    effectCombat.scheduleAreaDamage({2, 523.0F, 0, 80.0F, 40}, 1.0F);
    effectCombat.update(0.5F, effectBoard, effectSuns);
    passed &= check(centerTarget.armor() == armorBeforeDelay, "delayed area damage waits for its timer");
    effectCombat.update(0.5F, effectBoard, effectSuns);
    passed &= check(
        centerTarget.armor() == armorBeforeDelay - 40 && effectCombat.delayedAreaDamages().empty(),
        "delayed area damage triggers once and is removed"
    );

    CombatSystem snowCombat;
    Board snowBoard;
    SunSystem snowSuns(17U);
    snowBoard.place({1, 0}, PlantType::SnowPea);
    Zombie& snowTarget = snowCombat.spawnNormalZombie(1, 500.0F);
    snowCombat.update(SnowPea::attackIntervalSeconds, snowBoard, snowSuns);
    passed &= check(
        snowCombat.projectiles().size() == 1 &&
            snowCombat.projectiles().front().type == Projectile::Type::FrozenPea,
        "snow pea launches a frozen projectile"
    );
    for (int step = 0; step < 20 && !snowTarget.isSlowed(); ++step) {
        snowCombat.update(0.1F, snowBoard, snowSuns);
    }
    passed &= check(snowTarget.isSlowed(), "snow pea projectile slows a zombie on hit");
    return passed ? 0 : 1;
}
