#include "game/LevelSystem.hpp"

namespace object_project {

LevelSystem::LevelSystem(const LevelId levelId)
    : definition_(&levelDefinition(levelId)) {}

std::vector<SpawnEvent> LevelSystem::update(const float deltaSeconds) {
    std::vector<SpawnEvent> events;
    if (deltaSeconds <= 0.0F) {
        return events;
    }

    elapsedSeconds_ += deltaSeconds;
    const auto& schedule = definition().schedule;
    while (nextSpawn_ < schedule.size() &&
           elapsedSeconds_ >= schedule[nextSpawn_].timeSeconds) {
        events.push_back(schedule[nextSpawn_]);
        currentWave_ = schedule[nextSpawn_].wave;
        ++nextSpawn_;
    }
    return events;
}

void LevelSystem::selectLevel(const LevelId levelId) {
    definition_ = &levelDefinition(levelId);
    reset();
}

void LevelSystem::reset() {
    elapsedSeconds_ = 0.0F;
    nextSpawn_ = 0;
    currentWave_ = 1;
}

}  // namespace object_project
