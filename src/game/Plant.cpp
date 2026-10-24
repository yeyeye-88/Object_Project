#include "game/Plant.hpp"

#include <algorithm>

namespace object_project {

Plant::Plant(const PlantType type, const GridPosition position, const int maxHealth)
    : Entity(maxHealth), type_(type), position_(position) {}

float Plant::actionAnimationProgress() const {
    return 1.0F - actionAnimationRemaining_ / actionAnimationSeconds;
}

void Plant::beginActionAnimation() {
    actionAnimationRemaining_ = actionAnimationSeconds;
}

void Plant::updateActionAnimation(const float deltaSeconds) {
    actionAnimationRemaining_ = std::max(0.0F, actionAnimationRemaining_ - deltaSeconds);
}

Sunflower::Sunflower(const GridPosition position)
    : Plant(PlantType::Sunflower, position, maximumHealth) {}

std::optional<PlantAction> Sunflower::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    static_cast<void>(context);
    if (deltaSeconds <= 0.0F) {
        return std::nullopt;
    }
    productionTimer_ += deltaSeconds;
    const float interval = productionIntervalSeconds *
                           std::max(1.0F, context.sunProductionIntervalMultiplier);
    if (productionTimer_ < interval) {
        return std::nullopt;
    }
    productionTimer_ -= interval;
    return PlantAction::ProduceSun;
}

Peashooter::Peashooter(const GridPosition position)
    : Plant(PlantType::Peashooter, position, maximumHealth) {}

std::optional<PlantAction> Peashooter::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    if (deltaSeconds <= 0.0F || !context.enemyAhead) {
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
    const PlantUpdateContext& context
) {
    static_cast<void>(deltaSeconds);
    static_cast<void>(context);
    return std::nullopt;
}

Repeater::Repeater(const GridPosition position)
    : Plant(PlantType::Repeater, position, maximumHealth) {}

std::optional<PlantAction> Repeater::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    if (deltaSeconds <= 0.0F || !context.enemyAhead) {
        return std::nullopt;
    }
    attackTimer_ += deltaSeconds;
    if (attackTimer_ < attackIntervalSeconds) {
        return std::nullopt;
    }
    attackTimer_ -= attackIntervalSeconds;
    return PlantAction::FireDoublePea;
}

Threepeater::Threepeater(const GridPosition position)
    : Plant(PlantType::Threepeater, position, maximumHealth) {}

std::optional<PlantAction> Threepeater::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    if (deltaSeconds <= 0.0F || !context.enemyAhead) {
        return std::nullopt;
    }
    attackTimer_ += deltaSeconds;
    if (attackTimer_ < attackIntervalSeconds) {
        return std::nullopt;
    }
    attackTimer_ -= attackIntervalSeconds;
    return PlantAction::FireThreeRows;
}

SnowPea::SnowPea(const GridPosition position)
    : Plant(PlantType::SnowPea, position, maximumHealth) {}

std::optional<PlantAction> SnowPea::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    if (deltaSeconds <= 0.0F || !context.enemyAhead) {
        return std::nullopt;
    }
    attackTimer_ += deltaSeconds;
    if (attackTimer_ < attackIntervalSeconds) {
        return std::nullopt;
    }
    attackTimer_ -= attackIntervalSeconds;
    return PlantAction::FireFrozenPea;
}

TallNut::TallNut(const GridPosition position)
    : Plant(PlantType::TallNut, position, maximumHealth) {}

std::optional<PlantAction> TallNut::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    static_cast<void>(deltaSeconds);
    static_cast<void>(context);
    return std::nullopt;
}

PotatoMine::PotatoMine(const GridPosition position)
    : Plant(PlantType::PotatoMine, position, maximumHealth) {}

std::optional<PlantAction> PotatoMine::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    if (deltaSeconds > 0.0F && !isArmed()) {
        armingTimer_ += deltaSeconds;
    }
    if (isArmed() && context.enemyNearby) {
        return PlantAction::ExplodePotatoMine;
    }
    return std::nullopt;
}

CherryBomb::CherryBomb(const GridPosition position)
    : Plant(PlantType::CherryBomb, position, maximumHealth) {}

std::optional<PlantAction> CherryBomb::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    static_cast<void>(context);
    if (deltaSeconds <= 0.0F) {
        return std::nullopt;
    }
    fuseTimer_ += deltaSeconds;
    return fuseTimer_ >= fuseSeconds
        ? std::optional<PlantAction>{PlantAction::ExplodeCherryBomb}
        : std::nullopt;
}

Torchwood::Torchwood(const GridPosition position)
    : Plant(PlantType::Torchwood, position, maximumHealth) {}

std::optional<PlantAction> Torchwood::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    static_cast<void>(deltaSeconds);
    static_cast<void>(context);
    return std::nullopt;
}

PuffShroom::PuffShroom(const GridPosition position)
    : Plant(PlantType::PuffShroom, position, maximumHealth) {}

std::optional<PlantAction> PuffShroom::update(
    const float deltaSeconds,
    const PlantUpdateContext& context
) {
    if (deltaSeconds <= 0.0F || !context.enemyAhead) {
        return std::nullopt;
    }
    attackTimer_ += deltaSeconds;
    if (attackTimer_ < attackIntervalSeconds) {
        return std::nullopt;
    }
    attackTimer_ -= attackIntervalSeconds;
    return PlantAction::FireSpore;
}

std::unique_ptr<Plant> createPlant(const PlantType type, const GridPosition position) {
    switch (type) {
        case PlantType::Sunflower:
            return std::make_unique<Sunflower>(position);
        case PlantType::Peashooter:
            return std::make_unique<Peashooter>(position);
        case PlantType::WallNut:
            return std::make_unique<WallNut>(position);
        case PlantType::Repeater:
            return std::make_unique<Repeater>(position);
        case PlantType::Threepeater:
            return std::make_unique<Threepeater>(position);
        case PlantType::SnowPea:
            return std::make_unique<SnowPea>(position);
        case PlantType::TallNut:
            return std::make_unique<TallNut>(position);
        case PlantType::PotatoMine:
            return std::make_unique<PotatoMine>(position);
        case PlantType::CherryBomb:
            return std::make_unique<CherryBomb>(position);
        case PlantType::Torchwood:
            return std::make_unique<Torchwood>(position);
        case PlantType::PuffShroom:
            return std::make_unique<PuffShroom>(position);
        case PlantType::Count:
            break;
    }
    return nullptr;
}

}  // namespace object_project
