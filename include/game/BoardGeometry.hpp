#pragma once

#include <optional>

namespace object_project {

struct GridPosition {
    int row{};
    int column{};

    friend bool operator==(const GridPosition& left, const GridPosition& right) {
        return left.row == right.row && left.column == right.column;
    }
};

struct BoardGeometry {
    static constexpr int rows = 5;
    static constexpr int columns = 9;
    static constexpr float left = 250.0F;
    static constexpr float top = 80.0F;
    static constexpr float cellWidth = 80.0F;
    static constexpr float cellHeight = 100.0F;

    static bool contains(GridPosition position);
    static std::optional<GridPosition> gridAt(float x, float y);
    static float cellLeft(int column);
    static float cellTop(int row);
};

}  // namespace object_project
