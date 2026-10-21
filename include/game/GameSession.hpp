#pragma once

#include "game/Board.hpp"
#include "game/CardSystem.hpp"
#include "game/CombatSystem.hpp"
#include "game/LevelSystem.hpp"
#include "game/SunSystem.hpp"

namespace object_project {

enum class GameState {
    Running,
    Paused,
    Won,
    Lost,
};

enum class PlacementResult {
    Allowed,
    Paused,
    InvalidCell,
    Occupied,
    PlantUnavailable,
    NotEnoughSun,
    CardCoolingDown,
    GameOver,
};

class GameSession {
public:
    static constexpr int initialSun = 150;

    explicit GameSession(LevelId levelId = LevelId::Garden);

    void update(float deltaSeconds);
    PlacementResult placementStatus(PlantType type, GridPosition position) const;
    PlacementResult tryPlace(PlantType type, GridPosition position);
    bool removePlant(GridPosition position);
    bool collectSunAt(float x, float y);

    void togglePause();
    void startLevel(LevelId levelId);
    void restart();

    GameState state() const { return state_; }
    bool isPaused() const { return state_ == GameState::Paused; }
    int sun() const { return sun_; }
    int highestEndlessWave() const { return highestEndlessWave_; }
    bool isPlantAvailable(PlantType type) const {
        return level_.definition().allowsPlant(type);
    }
    const Board& board() const { return board_; }
    const CardSystem& cards() const { return cards_; }
    const SunSystem& suns() const { return suns_; }
    const CombatSystem& combat() const { return combat_; }
    CombatSystem& combat() { return combat_; }
    const LevelSystem& level() const { return level_; }

private:
    GameState state_{GameState::Running};
    int sun_{initialSun};
    Board board_;
    CardSystem cards_;
    SunSystem suns_;
    CombatSystem combat_;
    LevelSystem level_;
    int highestEndlessWave_{1};
};

}  // namespace object_project
