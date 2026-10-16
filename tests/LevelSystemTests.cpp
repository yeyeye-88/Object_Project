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
    passed &= check(first.size() == 1 && first.front() == 2, "first zombie spawns in configured row");
    const auto remaining = level.update(40.0F);
    passed &= check(remaining.size() == level.totalZombieCount() - 1, "large update emits every due spawn");
    passed &= check(level.allSpawned(), "level reports all spawns emitted");
    level.reset();
    passed &= check(!level.allSpawned() && level.elapsedSeconds() == 0.0F, "reset restores level schedule");
    return passed ? 0 : 1;
}
