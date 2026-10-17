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
    return passed ? 0 : 1;
}
