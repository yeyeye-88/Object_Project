#include "game/Zombie.hpp"

#include <algorithm>

namespace object_project {

Zombie::Zombie(const int row, const float x, const int maxHealth)
    : Entity(maxHealth), row_(row), x_(x) {}

void Zombie::move(const float deltaSeconds) {
    if (deltaSeconds > 0.0F) {
        x_ -= movementSpeed() * deltaSeconds;
    }
}

bool Zombie::updateAttack(const float deltaSeconds, const bool blocked) {
    if (!blocked) {
        attackTimer_ = 0.0F;
        return false;
    }
    if (deltaSeconds <= 0.0F) {
        return false;
    }
    attackTimer_ += deltaSeconds;
    if (attackTimer_ < attackIntervalSeconds()) {
        return false;
    }
    attackTimer_ -= attackIntervalSeconds();
    return true;
}

NormalZombie::NormalZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth) {}

}  // namespace object_project
