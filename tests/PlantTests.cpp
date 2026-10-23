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
        !sunflower.update(Sunflower::productionIntervalSeconds - 0.1F, {}),
        "sunflower does not produce sun early"
    );
    passed &= check(
        sunflower.update(0.1F, {}) == PlantAction::ProduceSun,
        "sunflower produces sun on schedule"
    );

    Peashooter peashooter({2, 3});
    passed &= check(
        !peashooter.update(Peashooter::attackIntervalSeconds, {}),
        "peashooter waits without a target"
    );
    passed &= check(
        peashooter.update(Peashooter::attackIntervalSeconds, {true, false}) == PlantAction::FirePea,
        "peashooter fires when an enemy is ahead"
    );
    peashooter.beginActionAnimation();
    passed &= check(
        peashooter.isPerformingAction() && peashooter.actionAnimationProgress() == 0.0F,
        "plant action animation starts with its gameplay action"
    );
    peashooter.updateActionAnimation(1.0F);
    passed &= check(
        !peashooter.isPerformingAction(),
        "plant action animation finishes independently of its attack timer"
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
        repeater.update(Repeater::attackIntervalSeconds, {true, false}) == PlantAction::FireDoublePea,
        "repeater requests a double shot"
    );

    Threepeater threepeater({2, 2});
    passed &= check(
        threepeater.update(Threepeater::attackIntervalSeconds, {true, false}) == PlantAction::FireThreeRows,
        "threepeater requests a three-lane shot"
    );

    SnowPea snowPea({1, 3});
    passed &= check(
        snowPea.update(SnowPea::attackIntervalSeconds, {true, false}) == PlantAction::FireFrozenPea,
        "snow pea requests a frozen shot"
    );

    TallNut tallNut({2, 4});
    passed &= check(
        tallNut.maxHealth() > wallNut.maxHealth() && tallNut.blocksVaulting(),
        "tall-nut has heavy health and blocks vaulting"
    );

    PotatoMine potatoMine({2, 3});
    passed &= check(
        !potatoMine.update(PotatoMine::armingSeconds - 0.1F, {false, true}),
        "potato mine does not trigger before arming"
    );
    passed &= check(
        potatoMine.update(0.1F, {false, true}) == PlantAction::ExplodePotatoMine,
        "armed potato mine triggers for a nearby zombie"
    );

    CherryBomb cherryBomb({2, 3});
    passed &= check(
        !cherryBomb.update(CherryBomb::fuseSeconds - 0.1F, {}),
        "cherry bomb waits for its fuse"
    );
    passed &= check(
        cherryBomb.update(0.1F, {}) == PlantAction::ExplodeCherryBomb,
        "cherry bomb explodes after its fuse"
    );

    Torchwood torchwood({2, 3});
    passed &= check(!torchwood.update(10.0F, {true, true}), "torchwood has no timed action");
    return passed ? 0 : 1;
}
