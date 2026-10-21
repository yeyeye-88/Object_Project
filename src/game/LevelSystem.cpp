#include "game/LevelSystem.hpp"

#include "game/BoardGeometry.hpp"

#include <algorithm>
#include <array>

namespace object_project {

LevelSystem::LevelSystem(const LevelId levelId)
    : definition_(&levelDefinition(levelId)) {}

std::vector<SpawnEvent> LevelSystem::update(const float deltaSeconds) {
    std::vector<SpawnEvent> events;
    if (deltaSeconds <= 0.0F) {
        return events;
    }

    elapsedSeconds_ += deltaSeconds;
    if (isEndless()) {
        return updateEndless();
    }

    const auto& schedule = definition().schedule;
    while (nextSpawn_ < schedule.size() &&
           elapsedSeconds_ >= schedule[nextSpawn_].timeSeconds) {
        events.push_back(schedule[nextSpawn_]);
        currentWave_ = schedule[nextSpawn_].wave;
        ++nextSpawn_;
    }
    return events;
}

std::vector<SpawnEvent> LevelSystem::updateEndless() {
    static constexpr std::size_t zombiesPerWave = 5;
    static constexpr std::array<ZombieType, 6> zombiePool{
        ZombieType::Normal,
        ZombieType::Conehead,
        ZombieType::Flag,
        ZombieType::PoleVaulting,
        ZombieType::Newspaper,
        ZombieType::Buckethead,
    };

    std::vector<SpawnEvent> events;
    while (elapsedSeconds_ >= nextEndlessSpawnSeconds_) {
        const std::size_t waveIndex = endlessSpawnCount_ / zombiesPerWave;
        currentWave_ = static_cast<int>(waveIndex + 1);
        const std::size_t indexInWave = endlessSpawnCount_ % zombiesPerWave;

        ZombieType type = ZombieType::Normal;
        if (currentWave_ == 1) {
            type = indexInWave == zombiesPerWave - 1
                ? ZombieType::Conehead
                : ZombieType::Normal;
        } else if (indexInWave == 0) {
            type = ZombieType::Flag;
        } else {
            const std::size_t unlocked = std::min<std::size_t>(
                zombiePool.size(),
                static_cast<std::size_t>(currentWave_ + 2)
            );
            type = zombiePool[(endlessSpawnCount_ + waveIndex) % unlocked];
        }

        const int row = static_cast<int>(
            (endlessSpawnCount_ * 2 + waveIndex) %
            static_cast<std::size_t>(BoardGeometry::rows)
        );
        events.push_back({nextEndlessSpawnSeconds_, row, type, currentWave_});
        ++endlessSpawnCount_;

        const float interval = std::max(
            0.9F,
            3.0F - static_cast<float>(currentWave_ - 1) * 0.12F
        );
        nextEndlessSpawnSeconds_ += interval;
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
    endlessSpawnCount_ = 0;
    nextEndlessSpawnSeconds_ = 3.0F;
}

}  // namespace object_project
