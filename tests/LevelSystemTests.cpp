#include "game/LevelSystem.hpp"

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
    LevelSystem level;

    passed &= check(level.update(4.9F).empty(), "level does not spawn zombies early");
    const auto first = level.update(0.1F);
    passed &= check(
        first.size() == 1 && first.front().row == 2 &&
            first.front().type == ZombieType::Normal && first.front().wave == 1,
        "first wave starts with a normal zombie in the configured row"
    );
    const auto secondWave = level.update(28.0F);
    passed &= check(
        !secondWave.empty() && level.currentWave() == 2,
        "later events advance to wave two"
    );
    const auto finalWave = level.update(30.0F);
    passed &= check(
        !finalWave.empty() && level.finalWaveStarted(),
        "the final events announce wave three"
    );
    passed &= check(level.allSpawned(), "level reports all spawns emitted");
    level.reset();
    passed &= check(
        !level.allSpawned() && level.elapsedSeconds() == 0.0F && level.currentWave() == 1,
        "reset restores level schedule and wave counter"
    );

    level.selectLevel(LevelId::Tutorial);
    passed &= check(
        level.levelId() == LevelId::Tutorial && level.waveCount() == 2 &&
            level.totalZombieCount() == levelDefinition(LevelId::Tutorial).schedule.size(),
        "selecting a level replaces the active schedule"
    );

    level.selectLevel(LevelId::EndlessDefense);
    passed &= check(level.update(2.9F).empty(), "endless mode waits for its first spawn");
    const auto endlessFirst = level.update(0.1F);
    passed &= check(
        endlessFirst.size() == 1 && endlessFirst.front().wave == 1 &&
            !level.allSpawned() && !level.finalWaveStarted(),
        "endless mode starts dynamically and never reports a final wave"
    );
    for (int step = 0; step < 20 && level.currentWave() < 2; ++step) {
        level.update(1.0F);
    }
    passed &= check(
        level.currentWave() >= 2 && level.totalZombieCount() >= 6,
        "endless generation advances beyond the first wave"
    );
    return passed ? 0 : 1;
}
