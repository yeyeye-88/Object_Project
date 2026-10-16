#include "game/LevelSystem.hpp"

namespace object_project {

std::vector<int> LevelSystem::update(const float deltaSeconds) {
    std::vector<int> rows;
    if (deltaSeconds <= 0.0F) {
        return rows;
    }

    elapsedSeconds_ += deltaSeconds;
    while (nextSpawn_ < schedule_.size() &&
           elapsedSeconds_ >= schedule_[nextSpawn_].timeSeconds) {
        rows.push_back(schedule_[nextSpawn_].row);
        ++nextSpawn_;
    }
    return rows;
}

void LevelSystem::reset() {
    elapsedSeconds_ = 0.0F;
    nextSpawn_ = 0;
}

}  // namespace object_project
