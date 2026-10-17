#pragma once

#include "game/Zombie.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace object_project {

struct SpawnEvent {
    float timeSeconds{};
    int row{};
    ZombieType type{ZombieType::Normal};
    int wave{1};
};

class LevelSystem {
public:
    std::vector<SpawnEvent> update(float deltaSeconds);
    void reset();

    bool allSpawned() const { return nextSpawn_ >= schedule_.size(); }
    float elapsedSeconds() const { return elapsedSeconds_; }
    std::size_t totalZombieCount() const { return schedule_.size(); }
    int currentWave() const { return currentWave_; }
    static constexpr int waveCount() { return 3; }
    bool finalWaveStarted() const { return currentWave_ == waveCount(); }

private:
    static constexpr std::array<SpawnEvent, 12> schedule_{{
        {5.0F, 2, ZombieType::Normal, 1},
        {10.0F, 1, ZombieType::Normal, 1},
        {15.0F, 3, ZombieType::Normal, 1},
        {22.0F, 0, ZombieType::Normal, 2},
        {25.0F, 4, ZombieType::Conehead, 2},
        {29.0F, 2, ZombieType::Normal, 2},
        {33.0F, 1, ZombieType::Conehead, 2},
        {40.0F, 3, ZombieType::Conehead, 3},
        {43.0F, 0, ZombieType::Buckethead, 3},
        {46.0F, 4, ZombieType::Normal, 3},
        {49.0F, 2, ZombieType::Buckethead, 3},
        {52.0F, 1, ZombieType::Conehead, 3},
    }};

    float elapsedSeconds_{};
    std::size_t nextSpawn_{};
    int currentWave_{1};
};

}  // namespace object_project
