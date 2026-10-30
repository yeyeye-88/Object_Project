#include "game/GameSession.hpp"

#include "game/EndlessSave.hpp"

#include <algorithm>

namespace object_project {

GameSession::GameSession(const LevelId levelId, const bool persistentEndlessProgress)
    : sun_(levelDefinition(levelId).initialSun), level_(levelId),
      persistentEndlessProgress_(persistentEndlessProgress) {
    if (persistentEndlessProgress_) {
        const EndlessSaveData saved = loadEndlessSave();
        highestEndlessWave_ = std::max(1, saved.highestWave);
        hasEndlessResume_ = saved.hasSnapshot;
    }
}

void GameSession::update(const float deltaSeconds) {
    if (state_ != GameState::Running || deltaSeconds <= 0.0F) {
        return;
    }
    bool breachIsImminent = false;
    if (level_.isEndless()) {
        for (const auto& zombie : combat_.zombies()) {
            const LawnMower& mower = combat_.mowers()[static_cast<std::size_t>(zombie->row())];
            if (mower.state == LawnMowerState::Used &&
                zombie->x() <= CombatSystem::defenseLineX + 12.0F) {
                breachIsImminent = true;
                break;
            }
        }
    }
    const GameSessionState beforeUpdate = breachIsImminent ? captureState() : GameSessionState{};
    cards_.update(deltaSeconds);
    const bool isNight = level_.levelId() == LevelId::NightChallenge;
    suns_.update(deltaSeconds, !isNight);
    for (const SpawnEvent& event : level_.update(deltaSeconds)) {
        combat_.spawnZombie(event.type, event.row);
    }
    if (level_.isEndless()) {
        highestEndlessWave_ = std::max(highestEndlessWave_, level_.currentWave());
    }
    combat_.update(deltaSeconds, board_, suns_, isNight ? 2.0F : 1.0F);

    if (combat_.breached()) {
        state_ = GameState::Lost;
        if (level_.isEndless() && persistentEndlessProgress_) {
            EndlessSaveData saved{
                highestEndlessWave_,
                true,
                breachIsImminent ? beforeUpdate : captureState(),
            };
            saved.snapshot.combat.breached = false;
            saved.snapshot.highestEndlessWave = highestEndlessWave_;
            hasEndlessResume_ = writeEndlessSave(saved);
        }
    } else if (!level_.isEndless() && level_.allSpawned() && !combat_.hasLivingZombies()) {
        state_ = GameState::Won;
    }
    if (level_.isEndless() && state_ == GameState::Running && persistentEndlessProgress_) {
        endlessSaveTimer_ += deltaSeconds;
        if (endlessSaveTimer_ >= 2.0F) {
            hasEndlessResume_ = writeEndlessSave({highestEndlessWave_, true, captureState()});
            endlessSaveTimer_ = 0.0F;
        }
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
    endlessSaveTimer_ = 0.0F;
}

void GameSession::startEndlessFresh() {
    startLevel(LevelId::EndlessDefense);
    if (persistentEndlessProgress_) {
        hasEndlessResume_ = writeEndlessSave({highestEndlessWave_, true, captureState()});
    }
}

bool GameSession::resumeEndless() {
    if (!persistentEndlessProgress_) {
        return false;
    }
    const EndlessSaveData saved = loadEndlessSave();
    highestEndlessWave_ = std::max(highestEndlessWave_, saved.highestWave);
    if (!saved.hasSnapshot) {
        hasEndlessResume_ = false;
        return false;
    }
    level_.selectLevel(LevelId::EndlessDefense);
    if (!restoreState(saved.snapshot)) {
        hasEndlessResume_ = false;
        return false;
    }
    state_ = GameState::Running;
    endlessSaveTimer_ = 0.0F;
    hasEndlessResume_ = true;
    return true;
}

void GameSession::saveEndlessNow() {
    if (!persistentEndlessProgress_ || !level_.isEndless() || state_ == GameState::Lost) {
        return;
    }
    hasEndlessResume_ = writeEndlessSave({highestEndlessWave_, true, captureState()});
    endlessSaveTimer_ = 0.0F;
}

GameSessionState GameSession::captureState() const {
    return {
        sun_,
        highestEndlessWave_,
        board_.captureState(),
        cards_.captureState(),
        suns_.captureState(),
        combat_.captureState(),
        level_.captureState(),
    };
}

bool GameSession::restoreState(const GameSessionState& state) {
    if (!level_.isEndless() || state.sun < 0 || state.highestEndlessWave < 1 ||
        !board_.restoreState(state.plants) || !cards_.restoreState(state.cardCooldowns) ||
        !suns_.restoreState(state.suns) || !combat_.restoreState(state.combat) ||
        !level_.restoreState(state.level)) {
        restart();
        return false;
    }
    sun_ = state.sun;
    highestEndlessWave_ = std::max(highestEndlessWave_, state.highestEndlessWave);
    state_ = GameState::Running;
    return true;
}

}  // namespace object_project
