#pragma once

#include "game/Entity.hpp"

namespace object_project {

enum class ZombieType {
    Normal,
};

class Zombie : public Entity {
public:
    Zombie(int row, float x, int maxHealth);
    ~Zombie() override = default;

    virtual ZombieType type() const = 0;
    virtual float movementSpeed() const = 0;
    virtual int attackDamage() const = 0;
    virtual float attackIntervalSeconds() const = 0;

    int row() const { return row_; }
    float x() const { return x_; }
    void move(float deltaSeconds);
    bool updateAttack(float deltaSeconds, bool blocked);

private:
    int row_{};
    float x_{};
    float attackTimer_{};
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

}  // namespace object_project
