#include "game/EndlessSave.hpp"

#include <filesystem>
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
    const std::filesystem::path directory =
        std::filesystem::temp_directory_path() / "object_project_endless_save_test";
    const std::filesystem::path path = directory / "progress.dat";

    GameSession session(LevelId::EndlessDefense);
    session.tryPlace(PlantType::Sunflower, {1, 2});
    Zombie& zombie = session.combat().spawnZombie(ZombieType::Buckethead, 3, 620.0F);
    zombie.receiveDamage(80);
    zombie.applySlow(0.5F, 2.0F);
    session.combat().launchProjectile(Projectile::Type::FrozenPea, 3, 430.0F);

    EndlessSaveData expected{7, true, session.captureState()};
    expected.snapshot.highestEndlessWave = 7;
    passed &= check(writeEndlessSave(expected, path), "endless snapshot writes to disk");
    const EndlessSaveData loaded = loadEndlessSave(path);
    passed &= check(
        loaded.hasSnapshot && loaded.highestWave == 7 &&
            loaded.snapshot.plants.size() == 1 && loaded.snapshot.combat.zombies.size() == 1 &&
            loaded.snapshot.combat.zombies.front().armor == zombie.armor() &&
            loaded.snapshot.combat.zombies.front().slowRemainingSeconds > 0.0F &&
            loaded.snapshot.combat.projectiles.size() == 1,
        "endless snapshot preserves complete combat state"
    );

    GameSession restored(LevelId::EndlessDefense);
    passed &= check(
        restored.restoreState(loaded.snapshot) && restored.board().isOccupied({1, 2}),
        "disk snapshot restores into a playable session"
    );
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    return passed ? 0 : 1;
}
