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

ZombieState Zombie::captureState() const {
    ZombieState state{
        type(), row_, x_, health(), armor_, attackTimer_, slowMultiplier_,
        slowRemainingSeconds_, attacking_, false, 0.0F, 0.0F,
    };
    captureSpecialState(state);
    return state;
}

bool Zombie::restoreState(const ZombieState& state) {
    if (state.type != type() || state.row != row_ || state.health < 0 ||
        state.armor < 0 || state.armor > maxArmor_ || state.attackTimer < 0.0F ||
        state.slowMultiplier <= 0.0F || state.slowMultiplier > 1.0F ||
        state.slowRemainingSeconds < 0.0F) {
        return false;
    }
    x_ = state.x;
    restoreHealth(state.health);
    armor_ = state.armor;
    attackTimer_ = state.attackTimer;
    slowMultiplier_ = state.slowMultiplier;
    slowRemainingSeconds_ = state.slowRemainingSeconds;
    attacking_ = state.attacking;
    return restoreSpecialState(state);
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

void PoleVaultingZombie::captureSpecialState(ZombieState& state) const {
    state.specialFlag = vaultAvailable_;
    state.specialA = vaultRemainingSeconds_;
    state.specialB = vaultVisualStartX_;
}

bool PoleVaultingZombie::restoreSpecialState(const ZombieState& state) {
    if (state.specialA < 0.0F || state.specialA > vaultAnimationSeconds) {
        return false;
    }
    vaultAvailable_ = state.specialFlag;
    vaultRemainingSeconds_ = state.specialA;
    vaultVisualStartX_ = state.specialB;
    return true;
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

void NewspaperZombie::captureSpecialState(ZombieState& state) const {
    state.specialA = transitionRemainingSeconds_;
}

bool NewspaperZombie::restoreSpecialState(const ZombieState& state) {
    if (state.specialA < 0.0F || state.specialA > transitionAnimationSeconds) {
        return false;
    }
    transitionRemainingSeconds_ = state.specialA;
    return true;
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
