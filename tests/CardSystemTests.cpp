#include "game/CardSystem.hpp"

#include <cmath>
#include <iostream>

namespace {

bool check(const bool condition, const char* name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << '\n';
    return condition;
}

bool near(const float left, const float right) {
    return std::fabs(left - right) < 0.001F;
}

}  // namespace

int main() {
    using namespace object_project;
    bool passed = true;
    CardSystem cards;

    passed &= check(cards.definition(PlantType::Sunflower).cost == 50, "sunflower costs 50 sun");
    passed &= check(cards.definition(PlantType::Repeater).cost == 200, "repeater costs 200 sun");
    passed &= check(cards.definition(PlantType::Threepeater).cost == 325, "threepeater costs 325 sun");
    passed &= check(!cards.isCoolingDown(PlantType::Peashooter), "cards start ready");
    cards.startCooldown(PlantType::Peashooter);
    passed &= check(cards.isCoolingDown(PlantType::Peashooter), "planting starts cooldown");
    passed &= check(near(cards.cooldownProgress(PlantType::Peashooter), 1.0F), "new cooldown is full");
    cards.update(2.5F);
    passed &= check(near(cards.remainingCooldown(PlantType::Peashooter), 5.0F), "cooldown advances by delta time");
    cards.update(20.0F);
    passed &= check(!cards.isCoolingDown(PlantType::Peashooter), "cooldown clamps at zero");
    cards.startCooldown(PlantType::WallNut);
    cards.reset();
    passed &= check(!cards.isCoolingDown(PlantType::WallNut), "reset readies all cards");
    return passed ? 0 : 1;
}
