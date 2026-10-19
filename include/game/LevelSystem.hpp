#pragma once

#include "game/LevelDefinition.hpp"

#include <cstddef>
#include <vector>

namespace object_project {

class LevelSystem {
public:
    explicit LevelSystem(LevelId levelId = LevelId::Garden);

    std::vector<SpawnEvent> update(float deltaSeconds);
    void selectLevel(LevelId levelId);
    void reset();

    bool allSpawned() const { return nextSpawn_ >= definition().schedule.size(); }
    float elapsedSeconds() const { return elapsedSeconds_; }
    std::size_t totalZombieCount() const { return definition().schedule.size(); }
    int currentWave() const { return currentWave_; }
    int waveCount() const { return definition().waveCount; }
    bool finalWaveStarted() const { return currentWave_ == waveCount(); }
    LevelId levelId() const { return definition_->id; }
    const LevelDefinition& definition() const { return *definition_; }

private:
    const LevelDefinition* definition_{};
    float elapsedSeconds_{};
    std::size_t nextSpawn_{};
    int currentWave_{1};
};

}  // namespace object_project
