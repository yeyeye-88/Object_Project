#pragma once

#include "game/Entity.hpp"

#include <cstddef>
#include <memory>

namespace object_project {

enum class ZombieType {
    Normal,
    Conehead,
    Buckethead,
    Flag,
    PoleVaulting,
    Newspaper,
    ScreenDoor,
    Football,
    Dancing,
    BackupDancer,
    Count,
};

struct ZombieState {
    ZombieType type{ZombieType::Normal};
    int row{};
    float x{};
    int health{};
    int armor{};
    float attackTimer{};
    float slowMultiplier{1.0F};
    float slowRemainingSeconds{};
    bool attacking{};
    bool specialFlag{};
    float specialA{};
    float specialB{};
};

constexpr std::size_t zombieTypeCount = static_cast<std::size_t>(ZombieType::Count);

class Zombie : public Entity {
public:
    Zombie(int row, float x, int maxHealth, int maxArmor);
    ~Zombie() override = default;

    virtual ZombieType type() const = 0;
    virtual float movementSpeed() const = 0;
    virtual int attackDamage() const = 0;
    virtual float attackIntervalSeconds() const = 0;
    virtual bool canVault() const { return false; }
    virtual void useVault(float) {}
    virtual bool isVaulting() const { return false; }
    virtual float vaultProgress() const { return 0.0F; }
    virtual float visualX() const { return x(); }
    virtual float visualYOffset() const { return 0.0F; }
    virtual bool isEnraged() const { return false; }
    virtual bool isTransforming() const { return false; }
    virtual float transformationProgress() const { return 0.0F; }
    virtual bool isSummoning() const { return false; }
    virtual bool summonReady() const { return false; }
    virtual float summonProgress() const { return 0.0F; }
    virtual void finishSummon() {}
    virtual void updateSpecialState(float) {}

    int row() const { return row_; }
    float x() const { return x_; }
    int armor() const { return armor_; }
    int maxArmor() const { return maxArmor_; }
    bool isSlowed() const { return slowRemainingSeconds_ > 0.0F; }
    bool isAttacking() const { return attacking_; }
    float speedMultiplier() const { return slowMultiplier_; }
    float slowRemainingSeconds() const { return slowRemainingSeconds_; }
    void receiveDamage(int amount);
    void applySlow(float speedMultiplier, float durationSeconds);
    void updateStatusEffects(float deltaSeconds);
    void move(float deltaSeconds);
    void setX(float x) { x_ = x; }
    bool updateAttack(float deltaSeconds, bool blocked);
    ZombieState captureState() const;
    bool restoreState(const ZombieState& state);

private:
    virtual void onArmorBroken() {}
    virtual void captureSpecialState(ZombieState&) const {}
    virtual bool restoreSpecialState(const ZombieState&) { return true; }

    int row_{};
    float x_{};
    float attackTimer_{};
    int armor_{};
    int maxArmor_{};
    float slowMultiplier_{1.0F};
    float slowRemainingSeconds_{};
    bool attacking_{};
};

class NormalZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float speed = 14.0F;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;

    NormalZombie(int row, float x);

    ZombieType type() const override { return ZombieType::Normal; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
};

class ConeheadZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr int maximumArmor = 100;
    static constexpr float speed = 13.0F;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;

    ConeheadZombie(int row, float x);

    ZombieType type() const override { return ZombieType::Conehead; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
};

class BucketheadZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr int maximumArmor = 220;
    static constexpr float speed = 11.0F;
    static constexpr int damage = 25;
    static constexpr float attackInterval = 1.0F;

    BucketheadZombie(int row, float x);

    ZombieType type() const override { return ZombieType::Buckethead; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
};

class FlagZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 120;
    static constexpr float speed = 15.0F;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;

    FlagZombie(int row, float x);

    ZombieType type() const override { return ZombieType::Flag; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
};

class PoleVaultingZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 120;
    static constexpr float vaultSpeed = 24.0F;
    static constexpr float walkingSpeed = 14.0F;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;

    PoleVaultingZombie(int row, float x);

    ZombieType type() const override { return ZombieType::PoleVaulting; }
    float movementSpeed() const override {
        return vaultAvailable_ ? vaultSpeed : walkingSpeed;
    }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
    bool canVault() const override { return vaultAvailable_; }
    void useVault(float visualStartX) override;
    bool isVaulting() const override { return vaultRemainingSeconds_ > 0.0F; }
    float vaultProgress() const override;
    float visualX() const override;
    float visualYOffset() const override;
    void updateSpecialState(float deltaSeconds) override;

private:
    void captureSpecialState(ZombieState& state) const override;
    bool restoreSpecialState(const ZombieState& state) override;
    static constexpr float vaultAnimationSeconds = 0.9F;
    bool vaultAvailable_{true};
    float vaultRemainingSeconds_{};
    float vaultVisualStartX_{};
};

class NewspaperZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr int newspaperArmor = 80;
    static constexpr float calmSpeed = 11.0F;
    static constexpr float enragedSpeed = 22.0F;
    static constexpr int damage = 20;
    static constexpr float calmAttackInterval = 1.2F;
    static constexpr float enragedAttackInterval = 0.55F;

    NewspaperZombie(int row, float x);

    ZombieType type() const override { return ZombieType::Newspaper; }
    float movementSpeed() const override { return isEnraged() ? enragedSpeed : calmSpeed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override {
        return isEnraged() ? enragedAttackInterval : calmAttackInterval;
    }
    bool isEnraged() const override { return armor() == 0; }
    bool isTransforming() const override { return transitionRemainingSeconds_ > 0.0F; }
    float transformationProgress() const override;
    void updateSpecialState(float deltaSeconds) override;

private:
    void onArmorBroken() override;
    void captureSpecialState(ZombieState& state) const override;
    bool restoreSpecialState(const ZombieState& state) override;

    static constexpr float transitionAnimationSeconds = 1.1F;
    float transitionRemainingSeconds_{};
};

class ScreenDoorZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr int maximumArmor = 220;
    static constexpr float speed = 11.0F;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;

    ScreenDoorZombie(int row, float x);
    ZombieType type() const override { return ZombieType::ScreenDoor; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
};

class FootballZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr int maximumArmor = 420;
    static constexpr float speed = 24.0F;
    static constexpr int damage = 25;
    static constexpr float attackInterval = 0.8F;

    FootballZombie(int row, float x);
    ZombieType type() const override { return ZombieType::Football; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
};

class DancingZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 185;
    static constexpr float speed = 12.0F;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;
    static constexpr float firstSummonDelay = 2.0F;
    static constexpr float resummonDelay = 12.0F;
    static constexpr float summonAnimationSeconds = 1.0F;

    DancingZombie(int row, float x);
    ZombieType type() const override { return ZombieType::Dancing; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
    bool isSummoning() const override;
    bool summonReady() const override { return summonTimer_ <= -summonAnimationSeconds; }
    float summonProgress() const override;
    void finishSummon() override;
    void updateSpecialState(float deltaSeconds) override;

private:
    void captureSpecialState(ZombieState& state) const override;
    bool restoreSpecialState(const ZombieState& state) override;
    float summonTimer_{firstSummonDelay};
};

class BackupDancerZombie final : public Zombie {
public:
    static constexpr int maximumHealth = 100;
    static constexpr float speed = DancingZombie::speed;
    static constexpr int damage = 20;
    static constexpr float attackInterval = 1.0F;

    BackupDancerZombie(int row, float x);
    ZombieType type() const override { return ZombieType::BackupDancer; }
    float movementSpeed() const override { return speed; }
    int attackDamage() const override { return damage; }
    float attackIntervalSeconds() const override { return attackInterval; }
    void bindToLeader(const Zombie* leader, int slot) {
        leader_ = leader;
        slot_ = slot;
    }
    void detachFromLeader() { leader_ = nullptr; }
    const Zombie* leader() const { return leader_; }
    int formationSlot() const { return slot_; }

private:
    const Zombie* leader_{};
    int slot_{-1};
};

std::unique_ptr<Zombie> createZombie(ZombieType type, int row, float x);

}  // namespace object_project
