#pragma once

#include "game/BoardGeometry.hpp"
#include "game/Entity.hpp"
#include "game/PlantType.hpp"

#include <memory>
#include <optional>

namespace object_project {

enum class PlantAction {
    ProduceSun,
    FirePea,
    FireDoublePea,
    FireThreeRows,
    FireFrozenPea,
};

class Plant : public Entity {
public:
    Plant(PlantType type, GridPosition position, int maxHealth);
    ~Plant() override = default;

    PlantType type() const { return type_; }
    GridPosition position() const { return position_; }
    virtual bool blocksVaulting() const { return false; }
    virtual std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) = 0;

private:
    PlantType type_;
    GridPosition position_;
};

class Sunflower final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float productionIntervalSeconds = 8.0F;

    explicit Sunflower(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;

private:
    float productionTimer_{};
};

class Peashooter final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit Peashooter(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;

private:
    float attackTimer_{};
};

class WallNut final : public Plant {
public:
    static constexpr int maximumHealth = 500;

    explicit WallNut(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;
};

class Repeater final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit Repeater(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;

private:
    float attackTimer_{};
};

class Threepeater final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.8F;

    explicit Threepeater(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;

private:
    float attackTimer_{};
};

class SnowPea final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit SnowPea(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;

private:
    float attackTimer_{};
};

class TallNut final : public Plant {
public:
    static constexpr int maximumHealth = 900;

    explicit TallNut(GridPosition position);
    bool blocksVaulting() const override { return true; }
    std::optional<PlantAction> update(float deltaSeconds, bool enemyAhead) override;
};

std::unique_ptr<Plant> createPlant(PlantType type, GridPosition position);

}  // namespace object_project
