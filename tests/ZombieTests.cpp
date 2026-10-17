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
    return passed ? 0 : 1;
}
