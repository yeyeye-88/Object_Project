#pragma once

#include "game/LevelDefinition.hpp"

#include <array>
#include <filesystem>

namespace object_project {

std::array<LevelDefinition, levelCount> loadLevelDefinitions(
    const std::filesystem::path& directory
);

}  // namespace object_project
