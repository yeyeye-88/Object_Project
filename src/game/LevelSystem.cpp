#include "game/LevelSystem.hpp"

namespace object_project {

std::vector<SpawnEvent> LevelSystem::update(const float deltaSeconds) {
    std::vector<SpawnEvent> events;
    if (deltaSeconds <= 0.0F) {
        return events;
    }

    elapsedSeconds_ += deltaSeconds;
    while (nextSpawn_ < schedule_.size() &&
           elapsedSeconds_ >= schedule_[nextSpawn_].timeSeconds) {
        events.push_back(schedule_[nextSpawn_]);
        currentWave_ = schedule_[nextSpawn_].wave;
        ++nextSpawn_;
    }
    return events;
}

void LevelSystem::reset() {
    elapsedSeconds_ = 0.0F;
    nextSpawn_ = 0;
    currentWave_ = 1;
}

}  // namespace object_project
