#pragma once

#include "game/BoardGeometry.hpp"
#include "game/Plant.hpp"
#include "game/PlantType.hpp"

#include <array>
#include <memory>
#include <optional>

namespace object_project {

class Board {
public:
    bool place(GridPosition position, PlantType type);
    bool remove(GridPosition position);
    bool isOccupied(GridPosition position) const;
    Plant* plantAt(GridPosition position);
    const Plant* plantAt(GridPosition position) const;
    void removeDeadPlants();
    void reset();

private:
    static constexpr std::size_t cellCount =
        static_cast<std::size_t>(BoardGeometry::rows * BoardGeometry::columns);

    static std::size_t indexOf(GridPosition position);

    std::array<std::unique_ptr<Plant>, cellCount> cells_{};
};

}  // namespace object_project
