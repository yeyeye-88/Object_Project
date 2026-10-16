#include "game/GameSession.hpp"

namespace object_project {

void GameSession::update(const float deltaSeconds) {
    if (isPaused()) {
        return;
    }
    cards_.update(deltaSeconds);
    suns_.update(deltaSeconds);
}

PlacementResult GameSession::placementStatus(
    const PlantType type,
    const GridPosition position
) const {
    if (isPaused()) {
        return PlacementResult::Paused;
    }
    if (!BoardGeometry::contains(position)) {
        return PlacementResult::InvalidCell;
    }
    if (board_.isOccupied(position)) {
        return PlacementResult::Occupied;
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
    return !isPaused() && board_.remove(position);
}

bool GameSession::collectSunAt(const float x, const float y) {
    if (isPaused() || !suns_.collectAt(x, y)) {
        return false;
    }
    sun_ += SunSystem::sunValue;
    return true;
}

void GameSession::togglePause() {
    state_ = isPaused() ? GameState::Running : GameState::Paused;
}

void GameSession::restart() {
    state_ = GameState::Running;
    sun_ = initialSun;
    board_.reset();
    cards_.reset();
    suns_.reset();
}

}  // namespace object_project
