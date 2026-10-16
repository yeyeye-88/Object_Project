#include "game/Board.hpp"

#include <cmath>

namespace object_project {

bool BoardGeometry::contains(const GridPosition position) {
    return position.row >= 0 && position.row < rows && position.column >= 0 &&
           position.column < columns;
}

std::optional<GridPosition> BoardGeometry::gridAt(const float x, const float y) {
    if (x < left || y < top || x >= left + columns * cellWidth ||
        y >= top + rows * cellHeight) {
        return std::nullopt;
    }

    return GridPosition{
        static_cast<int>(std::floor((y - top) / cellHeight)),
        static_cast<int>(std::floor((x - left) / cellWidth)),
    };
}

float BoardGeometry::cellLeft(const int column) {
    return left + static_cast<float>(column) * cellWidth;
}

float BoardGeometry::cellTop(const int row) {
    return top + static_cast<float>(row) * cellHeight;
}

std::size_t Board::indexOf(const GridPosition position) {
    return static_cast<std::size_t>(
        position.row * BoardGeometry::columns + position.column
    );
}

bool Board::place(const GridPosition position, const PlantType type) {
    if (!BoardGeometry::contains(position) || isOccupied(position)) {
        return false;
    }

    cells_[indexOf(position)] = createPlant(type, position);
    return cells_[indexOf(position)] != nullptr;
}

bool Board::remove(const GridPosition position) {
    if (!BoardGeometry::contains(position) || !isOccupied(position)) {
        return false;
    }

    cells_[indexOf(position)].reset();
    return true;
}

bool Board::isOccupied(const GridPosition position) const {
    return plantAt(position) != nullptr;
}

Plant* Board::plantAt(const GridPosition position) {
    if (!BoardGeometry::contains(position)) {
        return nullptr;
    }
    return cells_[indexOf(position)].get();
}

const Plant* Board::plantAt(const GridPosition position) const {
    if (!BoardGeometry::contains(position)) {
        return nullptr;
    }
    return cells_[indexOf(position)].get();
}

void Board::removeDeadPlants() {
    for (auto& plant : cells_) {
        if (plant && plant->isDead()) {
            plant.reset();
        }
    }
}

void Board::reset() {
    for (auto& plant : cells_) {
        plant.reset();
    }
}

}  // namespace object_project
