#include "game/Plant.hpp"

namespace object_project {

Plant::Plant(const PlantType type, const GridPosition position, const int maxHealth)
    : Entity(maxHealth), type_(type), position_(position) {}

Sunflower::Sunflower(const GridPosition position)
    : Plant(PlantType::Sunflower, position, maximumHealth) {}

std::optional<PlantAction> Sunflower::update(
    const float deltaSeconds,
    const bool enemyAhead
) {
    static_cast<void>(enemyAhead);
    if (deltaSeconds <= 0.0F) {
        return std::nullopt;
    }
    productionTimer_ += deltaSeconds;
    if (productionTimer_ < productionIntervalSeconds) {
        return std::nullopt;
    }
    productionTimer_ -= productionIntervalSeconds;
    return PlantAction::ProduceSun;
}

Peashooter::Peashooter(const GridPosition position)
    : Plant(PlantType::Peashooter, position, maximumHealth) {}

std::optional<PlantAction> Peashooter::update(
    const float deltaSeconds,
    const bool enemyAhead
) {
    if (deltaSeconds <= 0.0F || !enemyAhead) {
        return std::nullopt;
    }
    attackTimer_ += deltaSeconds;
    if (attackTimer_ < attackIntervalSeconds) {
        return std::nullopt;
    }
    attackTimer_ -= attackIntervalSeconds;
    return PlantAction::FirePea;
}

WallNut::WallNut(const GridPosition position)
    : Plant(PlantType::WallNut, position, maximumHealth) {}

std::optional<PlantAction> WallNut::update(
    const float deltaSeconds,
    const bool enemyAhead
) {
    static_cast<void>(deltaSeconds);
    static_cast<void>(enemyAhead);
    return std::nullopt;
}

std::unique_ptr<Plant> createPlant(const PlantType type, const GridPosition position) {
    switch (type) {
        case PlantType::Sunflower:
            return std::make_unique<Sunflower>(position);
        case PlantType::Peashooter:
            return std::make_unique<Peashooter>(position);
        case PlantType::WallNut:
            return std::make_unique<WallNut>(position);
        case PlantType::Count:
            break;
    }
    return nullptr;
}

}  // namespace object_project
