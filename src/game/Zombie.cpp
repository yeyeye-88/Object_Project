#include "game/Zombie.hpp"

#include <algorithm>
#include <cmath>

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
    const int armorBeforeDamage = armor_;
    const int armorDamage = std::min(armor_, amount);
    armor_ -= armorDamage;
    if (armorBeforeDamage > 0 && armor_ == 0) {
        onArmorBroken();
    }
    takeDamage(amount - armorDamage);
}

void Zombie::applySlow(const float speedMultiplier, const float durationSeconds) {
    if (speedMultiplier <= 0.0F || speedMultiplier > 1.0F || durationSeconds <= 0.0F) {
        return;
    }
    slowMultiplier_ = std::min(slowMultiplier_, speedMultiplier);
    slowRemainingSeconds_ = std::max(slowRemainingSeconds_, durationSeconds);
}

void Zombie::updateStatusEffects(const float deltaSeconds) {
    if (deltaSeconds <= 0.0F || slowRemainingSeconds_ <= 0.0F) {
        return;
    }
    slowRemainingSeconds_ = std::max(0.0F, slowRemainingSeconds_ - deltaSeconds);
    if (slowRemainingSeconds_ == 0.0F) {
        slowMultiplier_ = 1.0F;
    }
}

void Zombie::move(const float deltaSeconds) {
    if (deltaSeconds > 0.0F) {
        x_ -= movementSpeed() * slowMultiplier_ * deltaSeconds;
    }
}

bool Zombie::updateAttack(const float deltaSeconds, const bool blocked) {
    if (!blocked) {
        attackTimer_ = 0.0F;
        attacking_ = false;
        return false;
    }
    attacking_ = true;
    if (deltaSeconds <= 0.0F) {
        return false;
    }
    attackTimer_ += deltaSeconds;
    const float effectiveInterval = attackIntervalSeconds() / slowMultiplier_;
    if (attackTimer_ < effectiveInterval) {
        return false;
    }
    attackTimer_ -= effectiveInterval;
    return true;
}

NormalZombie::NormalZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, 0) {}

ConeheadZombie::ConeheadZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, maximumArmor) {}

BucketheadZombie::BucketheadZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, maximumArmor) {}

FlagZombie::FlagZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, 0) {}

PoleVaultingZombie::PoleVaultingZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, 0) {}

void PoleVaultingZombie::useVault(const float visualStartX) {
    vaultAvailable_ = false;
    vaultRemainingSeconds_ = vaultAnimationSeconds;
    vaultVisualStartX_ = visualStartX;
}

float PoleVaultingZombie::vaultProgress() const {
    return 1.0F - vaultRemainingSeconds_ / vaultAnimationSeconds;
}

float PoleVaultingZombie::visualX() const {
    if (!isVaulting()) {
        return x();
    }
    const float progress = vaultProgress();
    const float eased = progress * progress * (3.0F - 2.0F * progress);
    return vaultVisualStartX_ + (x() - vaultVisualStartX_) * eased;
}

float PoleVaultingZombie::visualYOffset() const {
    constexpr float pi = 3.14159265358979323846F;
    return isVaulting() ? -52.0F * std::sin(pi * vaultProgress()) : 0.0F;
}

void PoleVaultingZombie::updateSpecialState(const float deltaSeconds) {
    vaultRemainingSeconds_ = std::max(0.0F, vaultRemainingSeconds_ - deltaSeconds);
}

NewspaperZombie::NewspaperZombie(const int row, const float x)
    : Zombie(row, x, maximumHealth, newspaperArmor) {}

void NewspaperZombie::onArmorBroken() {
    transitionRemainingSeconds_ = transitionAnimationSeconds;
}

float NewspaperZombie::transformationProgress() const {
    return 1.0F - transitionRemainingSeconds_ / transitionAnimationSeconds;
}

void NewspaperZombie::updateSpecialState(const float deltaSeconds) {
    transitionRemainingSeconds_ = std::max(0.0F, transitionRemainingSeconds_ - deltaSeconds);
}

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
        case ZombieType::Flag:
            return std::make_unique<FlagZombie>(row, x);
        case ZombieType::PoleVaulting:
            return std::make_unique<PoleVaultingZombie>(row, x);
        case ZombieType::Newspaper:
            return std::make_unique<NewspaperZombie>(row, x);
        case ZombieType::Count:
            break;
    }
    return nullptr;
}

}  // namespace object_project
