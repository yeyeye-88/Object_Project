#include "game/LevelDefinition.hpp"

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

    const LevelDefinition& tutorial = levelDefinition(LevelId::Tutorial);
    const LevelDefinition& garden = levelDefinition(LevelId::Garden);
    const LevelDefinition& night = levelDefinition(LevelId::NightChallenge);
    const LevelDefinition& endless = levelDefinition(LevelId::EndlessDefense);

    passed &= check(
        !tutorial.name.empty() && !garden.name.empty() && !night.name.empty() &&
            !endless.name.empty(),
        "all selectable levels have names"
    );
    passed &= check(
        tutorial.initialSun > night.initialSun,
        "tutorial starts with more sun than the challenge level"
    );
    passed &= check(
        tutorial.allowsPlant(PlantType::Peashooter) &&
            !tutorial.allowsPlant(PlantType::CherryBomb),
        "tutorial limits the available plant set"
    );
    passed &= check(
        garden.waveCount == 3 && night.waveCount == 4 &&
            !garden.schedule.empty() && !night.schedule.empty(),
        "levels provide independent wave schedules"
    );
    passed &= check(
        night.mode == GameMode::Challenge && night.initialSun == 100 &&
            !night.allowsPlant(PlantType::Sunflower),
        "challenge mode limits sun and available plants"
    );
    passed &= check(
        endless.mode == GameMode::Endless && endless.waveCount == 0 &&
            endless.schedule.empty(),
        "endless mode uses dynamic waves instead of a fixed schedule"
    );
    return passed ? 0 : 1;
}
