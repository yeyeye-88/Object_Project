#include "game/CardSystem.hpp"

#include <algorithm>

namespace object_project {

CardSystem::CardSystem() {
    definitions_[toIndex(PlantType::Sunflower)] = {50, 7.5F};
    definitions_[toIndex(PlantType::Peashooter)] = {100, 7.5F};
    definitions_[toIndex(PlantType::WallNut)] = {50, 30.0F};
    definitions_[toIndex(PlantType::Repeater)] = {200, 7.5F};
    definitions_[toIndex(PlantType::Threepeater)] = {325, 7.5F};
    definitions_[toIndex(PlantType::SnowPea)] = {175, 7.5F};
    definitions_[toIndex(PlantType::TallNut)] = {125, 30.0F};
    definitions_[toIndex(PlantType::PotatoMine)] = {25, 20.0F};
    definitions_[toIndex(PlantType::CherryBomb)] = {150, 30.0F};
    definitions_[toIndex(PlantType::Torchwood)] = {175, 7.5F};
    definitions_[toIndex(PlantType::PuffShroom)] = {0, 7.5F};
}

const CardDefinition& CardSystem::definition(const PlantType type) const {
    return definitions_[toIndex(type)];
}

bool CardSystem::isCoolingDown(const PlantType type) const {
    return remainingCooldown(type) > 0.0F;
}

float CardSystem::remainingCooldown(const PlantType type) const {
    return remaining_[toIndex(type)];
}

float CardSystem::cooldownProgress(const PlantType type) const {
    const float maximum = definition(type).cooldownSeconds;
    return maximum > 0.0F ? remainingCooldown(type) / maximum : 0.0F;
}

void CardSystem::startCooldown(const PlantType type) {
    remaining_[toIndex(type)] = definition(type).cooldownSeconds;
}

void CardSystem::update(const float deltaSeconds) {
    if (deltaSeconds <= 0.0F) {
        return;
    }

    for (float& remaining : remaining_) {
        remaining = std::max(0.0F, remaining - deltaSeconds);
    }
}

void CardSystem::reset() {
    remaining_.fill(0.0F);
}

}  // namespace object_project
