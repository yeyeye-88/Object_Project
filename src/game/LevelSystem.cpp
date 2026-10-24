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
        const auto waveSize = [](const int wave) {
            return static_cast<std::size_t>(
                5 + std::min(7, std::max(0, wave - 1) / 2)
            );
        };
        if (endlessWaveSpawnCount_ >= waveSize(currentWave_)) {
            ++currentWave_;
            endlessWaveSpawnCount_ = 0;
        }

        const std::size_t indexInWave = endlessWaveSpawnCount_;
        const std::size_t zombiesThisWave = waveSize(currentWave_);

        ZombieType type = ZombieType::Normal;
        if (currentWave_ == 1) {
            type = indexInWave == zombiesThisWave - 1
                ? ZombieType::Conehead
                : ZombieType::Normal;
        } else if (indexInWave == 0) {
            type = ZombieType::Flag;
        } else {
            const std::size_t unlocked = std::min<std::size_t>(
                zombiePool.size(),
                static_cast<std::size_t>(currentWave_ + 1)
            );
            type = zombiePool[(endlessSpawnCount_ + indexInWave) % unlocked];
        }

        const int row = static_cast<int>(
            (endlessSpawnCount_ * 2 + static_cast<std::size_t>(currentWave_)) %
            static_cast<std::size_t>(BoardGeometry::rows)
        );
        events.push_back({nextEndlessSpawnSeconds_, row, type, currentWave_});
        ++endlessSpawnCount_;
        ++endlessWaveSpawnCount_;

        const bool waveFinished = endlessWaveSpawnCount_ == zombiesThisWave;
        const float interval = waveFinished
            ? 15.0F
            : std::max(
                  1.2F,
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
    endlessWaveSpawnCount_ = 0;
    nextEndlessSpawnSeconds_ = 10.0F;
}

}  // namespace object_project
