#pragma once

#include "game/PlantType.hpp"
#include "game/Zombie.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

namespace object_project {

enum class LevelId : std::size_t {
    Tutorial = 0,
    Garden,
    NightChallenge,
    Count,
};

constexpr std::size_t levelCount = static_cast<std::size_t>(LevelId::Count);

struct SpawnEvent {
    float timeSeconds{};
    int row{};
    ZombieType type{ZombieType::Normal};
    int wave{1};
};

struct LevelDefinition {
    LevelId id{LevelId::Garden};
    std::string_view name;
    std::string_view description;
    int initialSun{150};
    int waveCount{1};
    std::vector<PlantType> availablePlants;
    std::vector<SpawnEvent> schedule;

    bool allowsPlant(PlantType type) const;
};

const LevelDefinition& levelDefinition(LevelId id);

}  // namespace object_project
