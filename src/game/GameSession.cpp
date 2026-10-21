#include "game/GameSession.hpp"

#include <algorithm>

namespace object_project {

GameSession::GameSession(const LevelId levelId)
    : sun_(levelDefinition(levelId).initialSun), level_(levelId) {}

void GameSession::update(const float deltaSeconds) {
    if (state_ != GameState::Running || deltaSeconds <= 0.0F) {
        return;
    }
    cards_.update(deltaSeconds);
    suns_.update(deltaSeconds);
    for (const SpawnEvent& event : level_.update(deltaSeconds)) {
        combat_.spawnZombie(event.type, event.row);
    }
    if (level_.isEndless()) {
        highestEndlessWave_ = std::max(highestEndlessWave_, level_.currentWave());
    }
    combat_.update(deltaSeconds, board_, suns_);

    if (combat_.breached()) {
        state_ = GameState::Lost;
    } else if (!level_.isEndless() && level_.allSpawned() && !combat_.hasLivingZombies()) {
        state_ = GameState::Won;
    }
}

PlacementResult GameSession::placementStatus(
    const PlantType type,
    const GridPosition position
) const {
    if (state_ == GameState::Paused) {
        return PlacementResult::Paused;
    }
    if (state_ != GameState::Running) {
        return PlacementResult::GameOver;
    }
    if (!BoardGeometry::contains(position)) {
        return PlacementResult::InvalidCell;
    }
    if (board_.isOccupied(position)) {
        return PlacementResult::Occupied;
    }
    if (!isPlantAvailable(type)) {
        return PlacementResult::PlantUnavailable;
    }
    if (cards_.isCoolingDown(type)) {
        return PlacementResult::CardCoolingDown;
    }
    if (sun_ < cards_.definition(type).cost) {
        return PlacementResult::NotEnoughSun;
    }
    return PlacementResult::Allowed;
}

PlacementResult GameSession::tryPlace(
    const PlantType type,
    const GridPosition position
) {
    const PlacementResult status = placementStatus(type, position);
    if (status != PlacementResult::Allowed) {
        return status;
    }

    if (!board_.place(position, type)) {
        return PlacementResult::Occupied;
    }

    sun_ -= cards_.definition(type).cost;
    cards_.startCooldown(type);
    return PlacementResult::Allowed;
}

bool GameSession::removePlant(const GridPosition position) {
    return state_ == GameState::Running && board_.remove(position);
}

bool GameSession::collectSunAt(const float x, const float y) {
    if (state_ != GameState::Running || !suns_.collectAt(x, y)) {
        return false;
    }
    sun_ += SunSystem::sunValue;
    return true;
}

void GameSession::togglePause() {
    if (state_ == GameState::Running) {
        state_ = GameState::Paused;
    } else if (state_ == GameState::Paused) {
        state_ = GameState::Running;
    }
}

void GameSession::startLevel(const LevelId levelId) {
    level_.selectLevel(levelId);
    restart();
}

void GameSession::restart() {
    state_ = GameState::Running;
    sun_ = level_.definition().initialSun;
    board_.reset();
    cards_.reset();
    suns_.reset();
    combat_.reset();
    level_.reset();
}

}  // namespace object_project
