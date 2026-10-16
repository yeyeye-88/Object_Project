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
    return passed ? 0 : 1;
}
