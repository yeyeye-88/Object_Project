#include "game/Zombie.hpp"

#include <algorithm>

namespace object_project {

Zombie::Zombie(
    const int row,
    const float x,
    const int maxHealth,
    const int maxArmor
)
    : Entity(maxHealth), row_(row), x_(x), armor_(maxArmor), maxArmor_(maxArmor) {}

void Zombie::receiveDamage(const int amount) {
    if (amount <= 0) {
        return;
    }
    const int armorDamage = std::min(armor_, amount);
    armor_ -= armorDamage;
    takeDamage(amount - armorDamage);
}

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
    : Zombie(row, x, maximumHealth, 0) {}

ConeheadZombie::ConeheadZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, maximumArmor) {}

BucketheadZombie::BucketheadZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, maximumArmor) {}

std::unique_ptr<Zombie> createZombie(
    const ZombieType type,
    const int row,
    const float x
) {
    switch (type) {
        case ZombieType::Normal:
            return std::make_unique<NormalZombie>(row, x);
        case ZombieType::Conehead:
            return std::make_unique<ConeheadZombie>(row, x);
        case ZombieType::Buckethead:
            return std::make_unique<BucketheadZombie>(row, x);
        case ZombieType::Count:
            break;
    }
    return nullptr;
}

}  // namespace object_project
