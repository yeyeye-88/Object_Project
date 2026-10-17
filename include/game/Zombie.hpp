#pragma once

#include "game/Entity.hpp"

#include <cstddef>
#include <memory>

namespace object_project {

enum class ZombieType {
    Normal,
    Conehead,
    Buckethead,
    Count,
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

    int row() const { return row_; }
    float x() const { return x_; }
    int armor() const { return armor_; }
    int maxArmor() const { return maxArmor_; }
    void receiveDamage(int amount);
    void move(float deltaSeconds);
    bool updateAttack(float deltaSeconds, bool blocked);

private:
    int row_{};
    float x_{};
    float attackTimer_{};
    int armor_{};
    int maxArmor_{};
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

std::unique_ptr<Zombie> createZombie(ZombieType type, int row, float x);

}  // namespace object_project
