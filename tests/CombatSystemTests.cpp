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
    const auto repeaterEvents = repeaterCombat.takeEvents();
    passed &= check(
        repeaterEvents.size() == 1 && repeaterEvents.front() == CombatEvent::ProjectileFired,
        "one plant attack emits one sound event even when it launches two peas"
    );

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

    CombatSystem mineCombat;
    Board mineBoard;
    SunSystem mineSuns(19U);
    mineBoard.place({2, 3}, PlantType::PotatoMine);
    const float mineCenter = BoardGeometry::cellLeft(3) + BoardGeometry::cellWidth * 0.5F;
    mineCombat.spawnNormalZombie(2, mineCenter - CombatSystem::zombieWidth * 0.5F);
    mineCombat.spawnNormalZombie(
        2,
        mineCenter - CombatSystem::zombieWidth * 0.5F + 20.0F
    );
    mineCombat.update(PotatoMine::armingSeconds, mineBoard, mineSuns);
    passed &= check(
        mineCombat.zombies().size() == 1 && !mineBoard.isOccupied({2, 3}),
        "armed potato mine destroys only one nearby zombie and removes itself"
    );

    CombatSystem cherryCombat;
    Board cherryBoard;
    SunSystem cherrySuns(21U);
    cherryBoard.place({2, 4}, PlantType::CherryBomb);
    const float cherryCenter = BoardGeometry::cellLeft(4) + BoardGeometry::cellWidth * 0.5F;
    cherryCombat.spawnZombie(
        ZombieType::Buckethead,
        1,
        cherryCenter - CombatSystem::zombieWidth * 0.5F
    );
    cherryCombat.spawnZombie(
        ZombieType::Conehead,
        3,
        cherryCenter - CombatSystem::zombieWidth * 0.5F
    );
    cherryCombat.spawnNormalZombie(4, cherryCenter - CombatSystem::zombieWidth * 0.5F);
    cherryCombat.update(CherryBomb::fuseSeconds, cherryBoard, cherrySuns);
    passed &= check(
        cherryCombat.zombies().size() == 1 &&
            cherryCombat.zombies().front()->row() == 4 &&
            !cherryBoard.isOccupied({2, 4}),
        "cherry bomb damages three lanes and removes itself"
    );

    CombatSystem torchCombat;
    Board torchBoard;
    SunSystem torchSuns(23U);
    torchBoard.place({2, 0}, PlantType::Peashooter);
    torchBoard.place({2, 2}, PlantType::Torchwood);
    Zombie& fireTarget = torchCombat.spawnNormalZombie(2, 800.0F);
    torchCombat.update(Peashooter::attackIntervalSeconds, torchBoard, torchSuns);
    passed &= check(
        torchCombat.projectiles().size() == 1 &&
            torchCombat.projectiles().front().type == Projectile::Type::FirePea &&
            torchCombat.projectiles().front().damage ==
                CombatSystem::projectileDamage * CombatSystem::firePeaDamageMultiplier,
        "torchwood converts a normal pea into a stronger fire pea"
    );
    torchCombat.update(0.5F, torchBoard, torchSuns);
    passed &= check(
        fireTarget.health() == NormalZombie::maximumHealth - 40,
        "fire pea deals doubled damage"
    );

    CombatSystem frozenTorchCombat;
    Board frozenTorchBoard;
    SunSystem frozenTorchSuns(25U);
    frozenTorchBoard.place({1, 2}, PlantType::Torchwood);
    frozenTorchCombat.launchProjectile(
        Projectile::Type::FrozenPea,
        1,
        BoardGeometry::cellLeft(1)
    );
    frozenTorchCombat.update(0.6F, frozenTorchBoard, frozenTorchSuns);
    passed &= check(
        frozenTorchCombat.projectiles().front().type == Projectile::Type::FrozenPea &&
            frozenTorchCombat.projectiles().front().damage == CombatSystem::projectileDamage,
        "torchwood does not transform frozen peas"
    );

    CombatSystem vaultCombat;
    Board vaultBoard;
    SunSystem vaultSuns(27U);
    vaultBoard.place({2, 4}, PlantType::WallNut);
    Plant* vaultedPlant = vaultBoard.plantAt({2, 4});
    Zombie& vaulter = vaultCombat.spawnZombie(
        ZombieType::PoleVaulting,
        2,
        BoardGeometry::cellLeft(4) + 20.0F
    );
    const int vaultedHealth = vaultedPlant->health();
    vaultCombat.update(0.1F, vaultBoard, vaultSuns);
    passed &= check(
        !vaulter.canVault() && vaultedPlant->health() == vaultedHealth &&
            vaulter.x() < BoardGeometry::cellLeft(4),
        "pole-vaulting zombie jumps over its first ordinary plant"
    );

    CombatSystem tallNutCombat;
    Board tallNutBoard;
    SunSystem tallNutSuns(29U);
    tallNutBoard.place({2, 4}, PlantType::TallNut);
    Plant* blockingTallNut = tallNutBoard.plantAt({2, 4});
    Zombie& blockedVaulter = tallNutCombat.spawnZombie(
        ZombieType::PoleVaulting,
        2,
        BoardGeometry::cellLeft(4) + 20.0F
    );
    tallNutCombat.update(PoleVaultingZombie::attackInterval, tallNutBoard, tallNutSuns);
    passed &= check(
        blockedVaulter.canVault() &&
            blockingTallNut->health() == TallNut::maximumHealth - PoleVaultingZombie::damage,
        "tall-nut blocks vaulting and is attacked instead"
    );
    return passed ? 0 : 1;
}
