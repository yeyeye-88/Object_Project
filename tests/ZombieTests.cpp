#include "game/Zombie.hpp"

#include <cmath>
#include <iostream>

namespace {

bool check(const bool condition, const char* name) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << '\n';
    return condition;
}

}  // namespace

int main() {
    using namespace object_project;
    bool passed = true;

    ConeheadZombie conehead(1, 700.0F);
    conehead.receiveDamage(60);
    passed &= check(
        conehead.armor() == 40 && conehead.health() == ConeheadZombie::maximumHealth,
        "armor absorbs damage before health"
    );
    conehead.receiveDamage(50);
    passed &= check(
        conehead.armor() == 0 && conehead.health() == ConeheadZombie::maximumHealth - 10,
        "overflow damage reaches zombie health"
    );

    const auto buckethead = createZombie(ZombieType::Buckethead, 3, 600.0F);
    passed &= check(
        buckethead != nullptr && buckethead->type() == ZombieType::Buckethead &&
            buckethead->armor() == BucketheadZombie::maximumArmor,
        "zombie factory creates a buckethead with heavy armor"
    );

    NormalZombie slowed(2, 500.0F);
    slowed.applySlow(0.5F, 2.0F);
    slowed.move(1.0F);
    passed &= check(
        slowed.isSlowed() && slowed.x() == 500.0F - NormalZombie::speed * 0.5F,
        "slow reduces zombie movement speed"
    );
    passed &= check(
        !slowed.updateAttack(NormalZombie::attackInterval, true) &&
            slowed.updateAttack(NormalZombie::attackInterval, true),
        "slow increases zombie attack interval"
    );
    passed &= check(slowed.isAttacking(), "blocked zombie exposes its attack animation state");
    slowed.updateAttack(0.1F, false);
    passed &= check(!slowed.isAttacking(), "moving zombie leaves its attack animation state");
    slowed.applySlow(0.75F, 3.0F);
    passed &= check(
        slowed.speedMultiplier() == 0.5F && slowed.slowRemainingSeconds() == 3.0F,
        "weaker slow refreshes duration without replacing stronger slow"
    );
    slowed.updateStatusEffects(3.0F);
    passed &= check(!slowed.isSlowed() && slowed.speedMultiplier() == 1.0F, "slow expires cleanly");

    const auto flag = createZombie(ZombieType::Flag, 1, 600.0F);
    passed &= check(
        flag != nullptr && flag->type() == ZombieType::Flag,
        "zombie factory creates a flag zombie"
    );

    PoleVaultingZombie poleVaulting(2, 600.0F);
    passed &= check(
        poleVaulting.canVault() && poleVaulting.movementSpeed() == PoleVaultingZombie::vaultSpeed,
        "pole-vaulting zombie starts with its vault"
    );
    poleVaulting.setX(320.0F);
    poleVaulting.useVault(400.0F);
    passed &= check(
        !poleVaulting.canVault() && poleVaulting.isVaulting() &&
            poleVaulting.movementSpeed() == PoleVaultingZombie::walkingSpeed &&
            poleVaulting.visualX() == 400.0F,
        "pole-vaulting zombie slows after using its vault"
    );
    poleVaulting.updateSpecialState(0.45F);
    passed &= check(
        std::abs(poleVaulting.visualX() - 360.0F) < 0.01F &&
            poleVaulting.visualYOffset() < -50.0F,
        "pole-vaulting visual follows a smooth jump arc"
    );
    poleVaulting.updateSpecialState(0.55F);
    passed &= check(!poleVaulting.isVaulting(), "pole-vaulting animation state expires");

    NewspaperZombie newspaper(3, 600.0F);
    const float calmSpeed = newspaper.movementSpeed();
    const float calmAttackInterval = newspaper.attackIntervalSeconds();
    newspaper.receiveDamage(NewspaperZombie::newspaperArmor);
    passed &= check(
        newspaper.isEnraged() && newspaper.isTransforming() &&
            newspaper.movementSpeed() > calmSpeed &&
            newspaper.attackIntervalSeconds() < calmAttackInterval,
        "newspaper zombie enrages after its newspaper breaks"
    );
    newspaper.updateSpecialState(2.0F);
    passed &= check(!newspaper.isTransforming(), "newspaper transition state expires");
    return passed ? 0 : 1;
}
