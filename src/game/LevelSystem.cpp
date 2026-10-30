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
    std::vector<SpawnEvent> events;
    while (elapsedSeconds_ >= nextEndlessSpawnSeconds_) {
        const auto waveSize = [](const int wave) {
            return static_cast<std::size_t>(
                std::min(90, 10 + std::max(0, wave - 1) * 4)
            );
        };
        if (endlessWaveSpawnCount_ >= waveSize(currentWave_)) {
            ++currentWave_;
            endlessWaveSpawnCount_ = 0;
        }

        const std::size_t indexInWave = endlessWaveSpawnCount_;
        const std::size_t zombiesThisWave = waveSize(currentWave_);

        ZombieType type = ZombieType::Normal;
        if (indexInWave == 0 && currentWave_ > 1) {
            type = ZombieType::Flag;
        } else {
            const std::size_t selector =
                (endlessSpawnCount_ * 3 + indexInWave + static_cast<std::size_t>(currentWave_)) % 12;
            if (currentWave_ >= 4 && selector >= 10) {
                type = ZombieType::Newspaper;
            } else if (currentWave_ >= 3 && selector >= 8) {
                type = ZombieType::PoleVaulting;
            } else if (currentWave_ >= 2 && selector >= 5) {
                type = ZombieType::Buckethead;
            } else if (selector >= 2 || (currentWave_ == 1 && indexInWave + 1 == zombiesThisWave)) {
                type = ZombieType::Conehead;
            }
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
            ? std::max(6.0F, 11.0F - static_cast<float>(currentWave_) * 0.30F)
            : std::max(
                  0.55F,
                  2.10F - static_cast<float>(currentWave_ - 1) * 0.12F
              );
        nextEndlessSpawnSeconds_ += interval;
    }
    return events;
}

void LevelSystem::startEndlessAtWave(const int wave) {
    reset();
    if (!isEndless()) {
        return;
    }
    currentWave_ = std::max(1, wave);
}

LevelSystemState LevelSystem::captureState() const {
    return {
        elapsedSeconds_,
        nextSpawn_,
        currentWave_,
        endlessSpawnCount_,
        endlessWaveSpawnCount_,
        nextEndlessSpawnSeconds_,
    };
}

bool LevelSystem::restoreState(const LevelSystemState& state) {
    if (state.currentWave < 1 || state.elapsedSeconds < 0.0F ||
        state.nextEndlessSpawnSeconds < 0.0F) {
        return false;
    }
    elapsedSeconds_ = state.elapsedSeconds;
    nextSpawn_ = state.nextSpawn;
    currentWave_ = state.currentWave;
    endlessSpawnCount_ = state.endlessSpawnCount;
    endlessWaveSpawnCount_ = state.endlessWaveSpawnCount;
    nextEndlessSpawnSeconds_ = state.nextEndlessSpawnSeconds;
    return true;
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
