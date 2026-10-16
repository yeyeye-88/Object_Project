#include "game/Board.hpp"

#include <iostream>

namespace {

bool check(const bool condition, const char* name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << '\n';
    return condition;
}

}  // namespace

int main() {
    using namespace object_project;
    bool passed = true;
    Board board;

    passed &= check(BoardGeometry::gridAt(250.0F, 80.0F) == GridPosition{0, 0}, "top-left pixel maps to first cell");
    passed &= check(BoardGeometry::gridAt(969.9F, 579.9F) == GridPosition{4, 8}, "bottom-right interior maps to last cell");
    passed &= check(!BoardGeometry::gridAt(249.9F, 80.0F), "left boundary rejects outside point");
    passed &= check(!BoardGeometry::gridAt(970.0F, 580.0F), "exclusive outer boundary is rejected");

    passed &= check(board.place({2, 4}, PlantType::Peashooter), "plant can be placed in an empty cell");
    passed &= check(!board.place({2, 4}, PlantType::WallNut), "occupied cell rejects a second plant");
    passed &= check(
        board.plantAt({2, 4}) != nullptr &&
            board.plantAt({2, 4})->type() == PlantType::Peashooter,
        "placed plant type is preserved"
    );
    passed &= check(board.remove({2, 4}), "occupied cell can be cleared");
    passed &= check(!board.remove({2, 4}), "empty cell cannot be cleared twice");

    board.place({0, 0}, PlantType::Sunflower);
    board.reset();
    passed &= check(!board.isOccupied({0, 0}), "reset clears every cell");
    return passed ? 0 : 1;
}
