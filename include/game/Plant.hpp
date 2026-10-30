#pragma once

#include "game/BoardGeometry.hpp"
#include "game/Entity.hpp"
#include "game/PlantType.hpp"

#include <memory>
#include <optional>

namespace object_project {

struct PlantState {
    PlantType type{PlantType::Sunflower};
    GridPosition position{};
    int health{};
    float actionAnimationRemaining{};
    float logicTimer{};
};

struct PlantUpdateContext {
    bool enemyAhead{};
    bool enemyNearby{};
    float sunProductionIntervalMultiplier{1.0F};
};

enum class PlantAction {
    ProduceSun,
    FirePea,
    FireDoublePea,
    FireThreeRows,
    FireFrozenPea,
    ExplodePotatoMine,
    ExplodeCherryBomb,
    FireSpore,
};

class Plant : public Entity {
public:
    Plant(PlantType type, GridPosition position, int maxHealth);
    ~Plant() override = default;

    PlantType type() const { return type_; }
    GridPosition position() const { return position_; }
    bool isPerformingAction() const { return actionAnimationRemaining_ > 0.0F; }
    float actionAnimationProgress() const;
    void beginActionAnimation();
    void updateActionAnimation(float deltaSeconds);
    PlantState captureState() const;
    void restoreState(const PlantState& state);
    virtual bool blocksVaulting() const { return false; }
    virtual std::optional<PlantAction> update(
        float deltaSeconds,
        const PlantUpdateContext& context
    ) = 0;

protected:
    virtual float logicTimer() const { return 0.0F; }
    virtual void restoreLogicTimer(float) {}

private:
    static constexpr float actionAnimationSeconds = 0.45F;
    PlantType type_;
    GridPosition position_;
    float actionAnimationRemaining_{};
};

class Sunflower final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float productionIntervalSeconds = 8.0F;

    explicit Sunflower(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return productionTimer_; }
    void restoreLogicTimer(float value) override { productionTimer_ = value; }
    float productionTimer_{};
};

class Peashooter final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit Peashooter(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return attackTimer_; }
    void restoreLogicTimer(float value) override { attackTimer_ = value; }
    float attackTimer_{};
};

class WallNut final : public Plant {
public:
    static constexpr int maximumHealth = 500;

    explicit WallNut(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;
};

class Repeater final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit Repeater(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return attackTimer_; }
    void restoreLogicTimer(float value) override { attackTimer_ = value; }
    float attackTimer_{};
};

class Threepeater final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit Threepeater(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return attackTimer_; }
    void restoreLogicTimer(float value) override { attackTimer_ = value; }
    float attackTimer_{};
};

class SnowPea final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float attackIntervalSeconds = 1.5F;

    explicit SnowPea(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return attackTimer_; }
    void restoreLogicTimer(float value) override { attackTimer_ = value; }
    float attackTimer_{};
};

class TallNut final : public Plant {
public:
    static constexpr int maximumHealth = 900;

    explicit TallNut(GridPosition position);
    bool blocksVaulting() const override { return true; }
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;
};

class PotatoMine final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float armingSeconds = 3.0F;

    explicit PotatoMine(GridPosition position);
    bool isArmed() const { return armingTimer_ >= armingSeconds; }
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return armingTimer_; }
    void restoreLogicTimer(float value) override { armingTimer_ = value; }
    float armingTimer_{};
};

class CherryBomb final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float fuseSeconds = 0.8F;

    explicit CherryBomb(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return fuseTimer_; }
    void restoreLogicTimer(float value) override { fuseTimer_ = value; }
    float fuseTimer_{};
};

class Torchwood final : public Plant {
public:
    static constexpr int maximumHealth = 150;

    explicit Torchwood(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;
};

class PuffShroom final : public Plant {
public:
    static constexpr int maximumHealth = 100;
    static constexpr int sporeDamage = 20;
    static constexpr float attackIntervalSeconds = 1.5F;
    static constexpr float attackRange = BoardGeometry::cellWidth * 3.0F;

    explicit PuffShroom(GridPosition position);
    std::optional<PlantAction> update(float deltaSeconds, const PlantUpdateContext& context) override;

private:
    float logicTimer() const override { return attackTimer_; }
    void restoreLogicTimer(float value) override { attackTimer_ = value; }
    float attackTimer_{};
};

std::unique_ptr<Plant> createPlant(PlantType type, GridPosition position);

}  // namespace object_project
