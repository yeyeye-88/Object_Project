#pragma once

#include "game/PlantType.hpp"
#include "game/Zombie.hpp"

#include <array>
#include <string_view>

namespace object_project {

struct AlmanacEntry {
    std::string_view name;
    std::string_view role;
    std::string_view stats;
    std::string_view ability;
    std::string_view tip;
};

const AlmanacEntry& plantAlmanacEntry(PlantType type);
const AlmanacEntry& zombieAlmanacEntry(ZombieType type);

}  // namespace object_project
