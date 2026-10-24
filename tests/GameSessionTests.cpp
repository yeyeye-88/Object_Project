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

    GameSession pausedCombat;
    Zombie& pausedZombie = pausedCombat.combat().spawnNormalZombie(2, 700.0F);
    const float pausedX = pausedZombie.x();
    pausedCombat.togglePause();
    pausedCombat.update(2.0F);
    passed &= check(pausedZombie.x() == pausedX, "pause freezes zombie movement");

    GameSession losingSession;
    losingSession.combat().spawnNormalZombie(0, CombatSystem::defenseLineX - 1.0F);
    losingSession.update(0.1F);
    losingSession.update(2.0F);
    losingSession.combat().spawnNormalZombie(0, CombatSystem::defenseLineX - 1.0F);
    losingSession.update(0.1F);
    passed &= check(losingSession.state() == GameState::Lost, "second lane breach loses the game");
    passed &= check(
        losingSession.tryPlace(PlantType::Sunflower, {0, 0}) == PlacementResult::GameOver,
        "game-over session rejects planting"
    );

    GameSession winningSession;
    for (int step = 0; step < 1100 && winningSession.state() == GameState::Running; ++step) {
        winningSession.update(0.1F);
        for (const auto& zombie : winningSession.combat().zombies()) {
            zombie->takeDamage(zombie->health());
        }
    }
    passed &= check(winningSession.state() == GameState::Won, "clearing final zombie wins the level");

    GameSession tutorialSession(LevelId::Tutorial);
    passed &= check(
        tutorialSession.sun() == levelDefinition(LevelId::Tutorial).initialSun,
        "session uses the selected level initial sun"
    );
    passed &= check(
        tutorialSession.tryPlace(PlantType::CherryBomb, {0, 0}) ==
            PlacementResult::PlantUnavailable,
        "level restrictions reject unavailable plants"
    );
    tutorialSession.startLevel(LevelId::NightChallenge);
    passed &= check(
        tutorialSession.level().levelId() == LevelId::NightChallenge &&
            tutorialSession.sun() == levelDefinition(LevelId::NightChallenge).initialSun,
        "starting another level resets the session with its configuration"
    );
    passed &= check(
        tutorialSession.tryPlace(PlantType::Sunflower, {0, 0}) == PlacementResult::Allowed &&
            tutorialSession.tryPlace(PlantType::PuffShroom, {1, 0}) == PlacementResult::Allowed,
        "night challenge allows sunflower and free puff-shroom"
    );
    tutorialSession.update(SunSystem::spawnIntervalSeconds * 2.0F);
    passed &= check(
        tutorialSession.suns().tokens().empty(),
        "night challenge does not create sky sun before sunflower production"
    );
    tutorialSession.update(Sunflower::productionIntervalSeconds - 2.0F);
    passed &= check(
        tutorialSession.suns().tokens().size() == 1,
        "night sunflower produces after its doubled interval"
    );

    GameSession endlessSession(LevelId::EndlessDefense);
    for (int step = 0; step < 450; ++step) {
        endlessSession.update(0.1F);
        for (const auto& zombie : endlessSession.combat().zombies()) {
            zombie->takeDamage(zombie->health());
        }
    }
    passed &= check(
        endlessSession.state() == GameState::Running &&
            endlessSession.level().currentWave() >= 2 &&
            endlessSession.highestEndlessWave() >= 2,
        "endless session keeps running and records its highest wave"
    );
    const int recordedWave = endlessSession.highestEndlessWave();
    endlessSession.restart();
    passed &= check(
        endlessSession.highestEndlessWave() == recordedWave,
        "restarting endless mode preserves the in-app wave record"
    );
    return passed ? 0 : 1;
}
