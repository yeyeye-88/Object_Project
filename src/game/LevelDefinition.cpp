#include "game/LevelDefinition.hpp"

#include <algorithm>
#include <array>

namespace object_project {

namespace {

const std::vector<PlantType> allPlants{
    PlantType::Sunflower,
    PlantType::Peashooter,
    PlantType::WallNut,
    PlantType::Repeater,
    PlantType::Threepeater,
    PlantType::SnowPea,
    PlantType::TallNut,
    PlantType::PotatoMine,
    PlantType::CherryBomb,
    PlantType::Torchwood,
};

const std::array<LevelDefinition, levelCount> definitions{{
    {
        LevelId::Tutorial,
        "SUNNY START",
        "LEARN BASIC DEFENSE.",
        200,
        2,
        {PlantType::Sunflower, PlantType::Peashooter, PlantType::WallNut},
        {
            {6.0F, 2, ZombieType::Normal, 1},
            {13.0F, 1, ZombieType::Normal, 1},
            {20.0F, 3, ZombieType::Normal, 2},
            {25.0F, 2, ZombieType::Conehead, 2},
            {30.0F, 4, ZombieType::Normal, 2},
        },
    },
    {
        LevelId::Garden,
        "GARDEN DEFENSE",
        "FACE ALL SIX ZOMBIE TYPES.",
        150,
        3,
        allPlants,
        {
            {5.0F, 2, ZombieType::Normal, 1},
            {10.0F, 1, ZombieType::Normal, 1},
            {15.0F, 3, ZombieType::Conehead, 1},
            {21.0F, 0, ZombieType::Flag, 2},
            {24.0F, 4, ZombieType::PoleVaulting, 2},
            {28.0F, 2, ZombieType::Newspaper, 2},
            {32.0F, 1, ZombieType::Conehead, 2},
            {39.0F, 3, ZombieType::Flag, 3},
            {42.0F, 0, ZombieType::Buckethead, 3},
            {45.0F, 4, ZombieType::PoleVaulting, 3},
            {48.0F, 2, ZombieType::Newspaper, 3},
            {51.0F, 1, ZombieType::Buckethead, 3},
        },
    },
    {
        LevelId::NightChallenge,
        "NIGHT PRESSURE",
        "LESS SUN. FOUR FAST WAVES.",
        100,
        4,
        allPlants,
        {
            {4.0F, 0, ZombieType::Normal, 1},
            {7.0F, 2, ZombieType::Conehead, 1},
            {10.0F, 4, ZombieType::Newspaper, 1},
            {14.0F, 1, ZombieType::Flag, 2},
            {16.0F, 3, ZombieType::PoleVaulting, 2},
            {19.0F, 0, ZombieType::Conehead, 2},
            {22.0F, 4, ZombieType::Newspaper, 2},
            {26.0F, 2, ZombieType::Flag, 3},
            {28.0F, 1, ZombieType::Buckethead, 3},
            {30.0F, 3, ZombieType::PoleVaulting, 3},
            {33.0F, 0, ZombieType::Newspaper, 3},
            {36.0F, 4, ZombieType::Buckethead, 3},
            {40.0F, 2, ZombieType::Flag, 4},
            {42.0F, 0, ZombieType::Buckethead, 4},
            {44.0F, 1, ZombieType::PoleVaulting, 4},
            {46.0F, 3, ZombieType::Newspaper, 4},
            {48.0F, 4, ZombieType::Buckethead, 4},
        },
    },
}};

}  // namespace

bool LevelDefinition::allowsPlant(const PlantType type) const {
    return std::find(availablePlants.begin(), availablePlants.end(), type) !=
           availablePlants.end();
}

const LevelDefinition& levelDefinition(const LevelId id) {
    const std::size_t index = static_cast<std::size_t>(id);
    return definitions[index < definitions.size() ? index : 0];
}

}  // namespace object_project
