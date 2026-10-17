#include "game/Plant.hpp"

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

    Sunflower sunflower({1, 2});
    passed &= check(
        !sunflower.update(Sunflower::productionIntervalSeconds - 0.1F, false),
        "sunflower does not produce sun early"
    );
    passed &= check(
        sunflower.update(0.1F, false) == PlantAction::ProduceSun,
        "sunflower produces sun on schedule"
    );

    Peashooter peashooter({2, 3});
    passed &= check(
        !peashooter.update(Peashooter::attackIntervalSeconds, false),
        "peashooter waits without a target"
    );
    passed &= check(
        peashooter.update(Peashooter::attackIntervalSeconds, true) == PlantAction::FirePea,
        "peashooter fires when an enemy is ahead"
    );

    WallNut wallNut({3, 4});
    passed &= check(
        wallNut.maxHealth() > sunflower.maxHealth(),
        "wall-nut has more health than sunflower"
    );
    wallNut.takeDamage(100);
    passed &= check(wallNut.health() == WallNut::maximumHealth - 100, "damage reduces plant health");

    Repeater repeater({2, 2});
    passed &= check(
        repeater.update(Repeater::attackIntervalSeconds, true) == PlantAction::FireDoublePea,
        "repeater requests a double shot"
    );

    Threepeater threepeater({2, 2});
    passed &= check(
        threepeater.update(Threepeater::attackIntervalSeconds, true) == PlantAction::FireThreeRows,
        "threepeater requests a three-lane shot"
    );
    return passed ? 0 : 1;
}
