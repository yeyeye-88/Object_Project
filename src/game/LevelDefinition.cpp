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
        GameMode::Standard,
        "阳光初战",
        "学习阳光、种植与基础防守。",
        200,
        2,
        {PlantType::Sunflower, PlantType::Peashooter, PlantType::WallNut},
        {
            {12.0F, 2, ZombieType::Normal, 1},
            {20.0F, 1, ZombieType::Normal, 1},
            {35.0F, 3, ZombieType::Normal, 2},
            {43.0F, 2, ZombieType::Conehead, 2},
            {51.0F, 4, ZombieType::Normal, 2},
        },
    },
    {
        LevelId::Garden,
        GameMode::Standard,
        "草坪保卫战",
        "迎战六种不同的僵尸。",
        150,
        3,
        allPlants,
        {
            {12.0F, 2, ZombieType::Normal, 1},
            {19.0F, 1, ZombieType::Normal, 1},
            {26.0F, 3, ZombieType::Conehead, 1},
            {41.0F, 0, ZombieType::Flag, 2},
            {46.0F, 4, ZombieType::PoleVaulting, 2},
            {51.0F, 2, ZombieType::Newspaper, 2},
            {56.0F, 1, ZombieType::Conehead, 2},
            {71.0F, 3, ZombieType::Flag, 3},
            {76.0F, 0, ZombieType::Buckethead, 3},
            {81.0F, 4, ZombieType::PoleVaulting, 3},
            {86.0F, 2, ZombieType::Newspaper, 3},
            {91.0F, 1, ZombieType::Buckethead, 3},
        },
    },
    {
        LevelId::NightChallenge,
        GameMode::Challenge,
        "夜幕压力",
        "阳光更少，连续抵挡四波进攻。",
        100,
        4,
        {
            PlantType::Sunflower,
            PlantType::Peashooter,
            PlantType::WallNut,
            PlantType::SnowPea,
            PlantType::TallNut,
            PlantType::PotatoMine,
            PlantType::CherryBomb,
            PlantType::PuffShroom,
        },
        {
            {20.0F, 0, ZombieType::Normal, 1},
            {26.0F, 2, ZombieType::Conehead, 1},
            {32.0F, 4, ZombieType::Newspaper, 1},
            {47.0F, 1, ZombieType::Flag, 2},
            {52.0F, 3, ZombieType::PoleVaulting, 2},
            {57.0F, 0, ZombieType::Conehead, 2},
            {62.0F, 4, ZombieType::Newspaper, 2},
            {77.0F, 2, ZombieType::Flag, 3},
            {82.0F, 1, ZombieType::Buckethead, 3},
            {87.0F, 3, ZombieType::PoleVaulting, 3},
            {92.0F, 0, ZombieType::Newspaper, 3},
            {97.0F, 4, ZombieType::Buckethead, 3},
            {112.0F, 2, ZombieType::Flag, 4},
            {117.0F, 0, ZombieType::Buckethead, 4},
            {122.0F, 1, ZombieType::PoleVaulting, 4},
            {127.0F, 3, ZombieType::Newspaper, 4},
            {132.0F, 4, ZombieType::Buckethead, 4},
        },
    },
    {
        LevelId::EndlessDefense,
        GameMode::Endless,
        "无尽防守",
        "尽可能守住更多波次。",
        200,
        0,
        allPlants,
        {},
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
