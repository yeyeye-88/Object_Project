#pragma once

#include <cstddef>

namespace object_project {

enum class PlantType : std::size_t {
    Sunflower = 0,
    Peashooter,
    WallNut,
    Repeater,
    Threepeater,
    SnowPea,
    TallNut,
    PotatoMine,
    CherryBomb,
    Torchwood,
    PuffShroom,
    Count,
};

constexpr std::size_t plantTypeCount = static_cast<std::size_t>(PlantType::Count);

constexpr std::size_t toIndex(const PlantType type) {
    return static_cast<std::size_t>(type);
}

}  // namespace object_project
