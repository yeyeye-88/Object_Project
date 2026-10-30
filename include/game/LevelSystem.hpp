#pragma once

#include "game/LevelDefinition.hpp"

#include <cstddef>
#include <vector>

namespace object_project {

struct LevelSystemState {
    float elapsedSeconds{};
    std::size_t nextSpawn{};
    int currentWave{1};
    std::size_t endlessSpawnCount{};
    std::size_t endlessWaveSpawnCount{};
    float nextEndlessSpawnSeconds{10.0F};
};

class LevelSystem {
public:
    explicit LevelSystem(LevelId levelId = LevelId::Garden);

    std::vector<SpawnEvent> update(float deltaSeconds);
    void selectLevel(LevelId levelId);
    void reset();
    void startEndlessAtWave(int wave);
    LevelSystemState captureState() const;
    bool restoreState(const LevelSystemState& state);

    bool allSpawned() const {
        return !isEndless() && nextSpawn_ >= definition().schedule.size();
    }
    float elapsedSeconds() const { return elapsedSeconds_; }
    std::size_t totalZombieCount() const {
        return isEndless() ? endlessSpawnCount_ : definition().schedule.size();
    }
    int currentWave() const { return currentWave_; }
    int waveCount() const { return definition().waveCount; }
    bool finalWaveStarted() const {
        return !isEndless() && currentWave_ == waveCount();
    }
    bool isEndless() const { return definition().mode == GameMode::Endless; }
    LevelId levelId() const { return definition_->id; }
    const LevelDefinition& definition() const { return *definition_; }

private:
    std::vector<SpawnEvent> updateEndless();

    const LevelDefinition* definition_{};
    float elapsedSeconds_{};
    std::size_t nextSpawn_{};
    int currentWave_{1};
    std::size_t endlessSpawnCount_{};
    std::size_t endlessWaveSpawnCount_{};
    float nextEndlessSpawnSeconds_{10.0F};
};

}  // namespace object_project
