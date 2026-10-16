#pragma once

#include "game/Board.hpp"
#include "game/CardSystem.hpp"
#include "game/SunSystem.hpp"

namespace object_project {

enum class GameState {
    Running,
    Paused,
};

enum class PlacementResult {
    Allowed,
    Paused,
    InvalidCell,
    Occupied,
    NotEnoughSun,
    CardCoolingDown,
};

class GameSession {
public:
    static constexpr int initialSun = 150;

    void update(float deltaSeconds);
    PlacementResult placementStatus(PlantType type, GridPosition position) const;
    PlacementResult tryPlace(PlantType type, GridPosition position);
    bool removePlant(GridPosition position);
    bool collectSunAt(float x, float y);

    void togglePause();
    void restart();

    GameState state() const { return state_; }
    bool isPaused() const { return state_ == GameState::Paused; }
    int sun() const { return sun_; }
    const Board& board() const { return board_; }
    const CardSystem& cards() const { return cards_; }
    const SunSystem& suns() const { return suns_; }

private:
    GameState state_{GameState::Running};
    int sun_{initialSun};
    Board board_;
    CardSystem cards_;
    SunSystem suns_;
};

}  // namespace object_project
