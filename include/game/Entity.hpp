#pragma once

#include <algorithm>

namespace object_project {

class Entity {
public:
    virtual ~Entity() = default;

    int health() const { return health_; }
    int maxHealth() const { return maxHealth_; }
    bool isDead() const { return health_ <= 0; }
    void restoreHealth(const int health) {
        health_ = std::clamp(health, 0, maxHealth_);
    }

    void takeDamage(const int amount) {
        if (amount > 0) {
            health_ = std::max(0, health_ - amount);
        }
    }

protected:
    explicit Entity(const int maxHealth) : health_(maxHealth), maxHealth_(maxHealth) {}

private:
    int health_{};
    int maxHealth_{};
};

}  // namespace object_project
