#pragma once

#include "game/PlantType.hpp"

#include <array>

namespace object_project {

struct CardDefinition {
    int cost{};
    float cooldownSeconds{};
};

class CardSystem {
public:
    CardSystem();

    const CardDefinition& definition(PlantType type) const;
    bool isCoolingDown(PlantType type) const;
    float remainingCooldown(PlantType type) const;
    float cooldownProgress(PlantType type) const;
    void startCooldown(PlantType type);
    void update(float deltaSeconds);
    void reset();

private:
    std::array<CardDefinition, plantTypeCount> definitions_{};
    std::array<float, plantTypeCount> remaining_{};
};

}  // namespace object_project
