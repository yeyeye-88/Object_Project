#include "game/Zombie.hpp"

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

    ConeheadZombie conehead(1, 700.0F);
    conehead.receiveDamage(60);
    passed &= check(
        conehead.armor() == 40 && conehead.health() == ConeheadZombie::maximumHealth,
        "armor absorbs damage before health"
    );
    conehead.receiveDamage(50);
    passed &= check(
        conehead.armor() == 0 && conehead.health() == ConeheadZombie::maximumHealth - 10,
        "overflow damage reaches zombie health"
    );

    const auto buckethead = createZombie(ZombieType::Buckethead, 3, 600.0F);
    passed &= check(
        buckethead != nullptr && buckethead->type() == ZombieType::Buckethead &&
            buckethead->armor() == BucketheadZombie::maximumArmor,
        "zombie factory creates a buckethead with heavy armor"
    );

    NormalZombie slowed(2, 500.0F);
    slowed.applySlow(0.5F, 2.0F);
    slowed.move(1.0F);
    passed &= check(
        slowed.isSlowed() && slowed.x() == 500.0F - NormalZombie::speed * 0.5F,
        "slow reduces zombie movement speed"
    );
    passed &= check(
        !slowed.updateAttack(NormalZombie::attackInterval, true) &&
            slowed.updateAttack(NormalZombie::attackInterval, true),
        "slow increases zombie attack interval"
    );
    slowed.applySlow(0.75F, 3.0F);
    passed &= check(
        slowed.speedMultiplier() == 0.5F && slowed.slowRemainingSeconds() == 3.0F,
        "weaker slow refreshes duration without replacing stronger slow"
    );
    slowed.updateStatusEffects(3.0F);
    passed &= check(!slowed.isSlowed() && slowed.speedMultiplier() == 1.0F, "slow expires cleanly");
    return passed ? 0 : 1;
}
