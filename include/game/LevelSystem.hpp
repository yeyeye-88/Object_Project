#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace object_project {

struct SpawnEvent {
    float timeSeconds{};
    int row{};
};

class LevelSystem {
public:
    std::vector<int> update(float deltaSeconds);
    void reset();

    bool allSpawned() const { return nextSpawn_ >= schedule_.size(); }
    float elapsedSeconds() const { return elapsedSeconds_; }
    std::size_t totalZombieCount() const { return schedule_.size(); }

private:
    static constexpr std::array<SpawnEvent, 8> schedule_{{
        {5.0F, 2},
        {10.0F, 1},
        {15.0F, 3},
        {21.0F, 0},
        {27.0F, 4},
        {33.0F, 2},
        {39.0F, 1},
        {45.0F, 3},
    }};

    float elapsedSeconds_{};
    std::size_t nextSpawn_{};
};

}  // namespace object_project
