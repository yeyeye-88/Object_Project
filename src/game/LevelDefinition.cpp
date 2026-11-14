#include "game/LevelDefinition.hpp"

#include "game/LevelLoader.hpp"

#include <algorithm>
#include <array>
#include <iostream>

namespace object_project {

namespace {

std::array<LevelDefinition, levelCount> safeFallbackDefinitions() {
    const std::vector<PlantType> basicPlants{
        PlantType::Sunflower,
        PlantType::Peashooter,
        PlantType::WallNut,
    };
    return {{
        {LevelId::Tutorial, GameMode::Standard, "阳光初战", "关卡配置缺失。", 200, 1,
         basicPlants, {}},
        {LevelId::Garden, GameMode::Standard, "草坪保卫战", "关卡配置缺失。", 150, 1,
         basicPlants, {}},
        {LevelId::NightChallenge, GameMode::Challenge, "夜幕压力", "关卡配置缺失。", 100,
         1, basicPlants, {}},
        {LevelId::EndlessDefense, GameMode::Endless, "无尽防守", "关卡配置缺失。", 200, 0,
         basicPlants, {}},
    }};
}

const std::array<LevelDefinition, levelCount>& definitions() {
    static const std::array<LevelDefinition, levelCount> loaded = [] {
        try {
            return loadLevelDefinitions("assets/config/levels");
        } catch (const std::exception& error) {
            std::cerr << "Failed to load external level data: " << error.what() << '\n';
            return safeFallbackDefinitions();
        }
    }();
    return loaded;
}

}  // namespace

bool LevelDefinition::allowsPlant(const PlantType type) const {
    return std::find(availablePlants.begin(), availablePlants.end(), type) !=
           availablePlants.end();
}

const LevelDefinition& levelDefinition(const LevelId id) {
    const auto& available = definitions();
    const std::size_t index = static_cast<std::size_t>(id);
    return available[index < available.size() ? index : 0];
}

}  // namespace object_project
