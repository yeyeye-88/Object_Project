#include "game/GameSession.hpp"

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
    GameSession session;

    passed &= check(session.sun() == GameSession::initialSun, "session starts with configured sun");
    passed &= check(session.tryPlace(PlantType::Peashooter, {1, 1}) == PlacementResult::Allowed, "affordable plant can be placed");
    passed &= check(session.sun() == 50, "successful placement spends sun once");
    passed &= check(session.tryPlace(PlantType::WallNut, {1, 1}) == PlacementResult::Occupied, "occupied cell takes precedence");
    passed &= check(session.tryPlace(PlantType::Peashooter, {1, 2}) == PlacementResult::CardCoolingDown, "cooling card cannot be planted");

    const float cooldown = session.cards().remainingCooldown(PlantType::Peashooter);
    session.togglePause();
    session.update(3.0F);
    passed &= check(session.cards().remainingCooldown(PlantType::Peashooter) == cooldown, "pause freezes cooldown");
    passed &= check(session.removePlant({1, 1}) == false, "pause blocks shovel actions");
    session.togglePause();
    passed &= check(session.removePlant({1, 1}), "running session allows shovel actions");

    session.update(SunSystem::spawnIntervalSeconds);
    const SunToken sun = session.suns().tokens().front();
    const int beforeCollection = session.sun();
    passed &= check(session.collectSunAt(sun.x + 1.0F, sun.y + 1.0F), "sun can be collected while running");
    passed &= check(session.sun() == beforeCollection + SunSystem::sunValue, "collection credits sun value");

    session.restart();
    passed &= check(session.state() == GameState::Running, "restart resumes the game");
    passed &= check(session.sun() == GameSession::initialSun, "restart restores initial sun");
    passed &= check(!session.board().isOccupied({1, 1}), "restart clears the board");
    passed &= check(session.suns().tokens().empty(), "restart clears falling suns");
    passed &= check(!session.cards().isCoolingDown(PlantType::Peashooter), "restart clears cooldowns");
    return passed ? 0 : 1;
}
