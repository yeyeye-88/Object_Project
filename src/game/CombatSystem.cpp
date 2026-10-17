#include "game/CombatSystem.hpp"

#include "game/Board.hpp"
#include "game/Plant.hpp"
#include "game/SunSystem.hpp"

#include <algorithm>
#include <cmath>

namespace object_project {

namespace {

constexpr float projectileWidth = 14.0F;
constexpr float mowerWidth = 58.0F;

float plantLeft(const GridPosition position) {
    return BoardGeometry::cellLeft(position.column) + 8.0F;
}

float plantRight(const GridPosition position) {
    return BoardGeometry::cellLeft(position.column) + BoardGeometry::cellWidth - 8.0F;
}

}  // namespace

CombatSystem::CombatSystem() {
    reset();
}

Zombie& CombatSystem::spawnZombie(
    const ZombieType type,
    const int row,
    const float x
) {
    zombies_.push_back(createZombie(type, row, x));
    return *zombies_.back();
}

Zombie& CombatSystem::spawnNormalZombie(const int row, const float x) {
    return spawnZombie(ZombieType::Normal, row, x);
}

void CombatSystem::update(
    const float deltaSeconds,
    Board& board,
    SunSystem& suns
) {
    if (deltaSeconds <= 0.0F || breached_) {
        return;
    }

    updatePlants(deltaSeconds, board, suns);
    updateProjectiles(deltaSeconds);
    updateDelayedAreaDamages(deltaSeconds);
    updateZombies(deltaSeconds, board);
    board.removeDeadPlants();
    updateMowers(deltaSeconds);
    removeDead();
}

void CombatSystem::reset() {
    zombies_.clear();
    projectiles_.clear();
    delayedAreaDamages_.clear();
    nextProjectileId_ = 1;
    nextAreaDamageId_ = 1;
    breached_ = false;
    for (int row = 0; row < BoardGeometry::rows; ++row) {
        mowers_[static_cast<std::size_t>(row)] = {row, mowerStartX, LawnMowerState::Ready};
    }
}

void CombatSystem::launchProjectile(
    const Projectile::Type type,
    const int row,
    const float x,
    const int damage
) {
    if (row < 0 || row >= BoardGeometry::rows || damage <= 0) {
        return;
    }
    projectiles_.push_back({nextProjectileId_++, row, x, damage, type});
}

void CombatSystem::applyAreaDamage(const AreaDamage& area) {
    if (area.rowRadius < 0 || area.horizontalRadius < 0.0F || area.damage <= 0) {
        return;
    }
    for (const auto& zombie : zombies_) {
        if (zombie->isDead() ||
            std::abs(zombie->row() - area.centerRow) > area.rowRadius) {
            continue;
        }
        const float zombieCenter = zombie->x() + zombieWidth * 0.5F;
        if (std::abs(zombieCenter - area.centerX) <= area.horizontalRadius) {
            zombie->receiveDamage(area.damage);
        }
    }
}

void CombatSystem::scheduleAreaDamage(const AreaDamage& area, const float delaySeconds) {
    if (delaySeconds <= 0.0F) {
        applyAreaDamage(area);
        return;
    }
    if (area.rowRadius < 0 || area.horizontalRadius < 0.0F || area.damage <= 0) {
        return;
    }
    delayedAreaDamages_.push_back({nextAreaDamageId_++, area, delaySeconds});
}

bool CombatSystem::hasZombieAhead(const int row, const float x) const {
    return std::any_of(
        zombies_.begin(),
        zombies_.end(),
        [row, x](const std::unique_ptr<Zombie>& zombie) {
            return !zombie->isDead() && zombie->row() == row && zombie->x() > x;
        }
    );
}

void CombatSystem::updatePlants(
    const float deltaSeconds,
    Board& board,
    SunSystem& suns
) {
    for (int row = 0; row < BoardGeometry::rows; ++row) {
        for (int column = 0; column < BoardGeometry::columns; ++column) {
            Plant* plant = board.plantAt({row, column});
            if (plant == nullptr || plant->isDead()) {
                continue;
            }

            const float originX = BoardGeometry::cellLeft(column) + BoardGeometry::cellWidth * 0.72F;
            bool enemyAhead = hasZombieAhead(row, originX);
            if (plant->type() == PlantType::Threepeater) {
                enemyAhead = enemyAhead ||
                    (row > 0 && hasZombieAhead(row - 1, originX)) ||
                    (row + 1 < BoardGeometry::rows && hasZombieAhead(row + 1, originX));
            }
            const auto action = plant->update(
                deltaSeconds,
                enemyAhead
            );
            if (action == PlantAction::ProduceSun) {
                suns.spawnFromPlant(plant->position());
            } else if (action == PlantAction::FirePea) {
                launchProjectile(Projectile::Type::Pea, row, originX);
            } else if (action == PlantAction::FireDoublePea) {
                launchProjectile(Projectile::Type::Pea, row, originX);
                launchProjectile(Projectile::Type::Pea, row, originX - 14.0F);
            } else if (action == PlantAction::FireThreeRows) {
                for (int targetRow = row - 1; targetRow <= row + 1; ++targetRow) {
                    if (targetRow >= 0 && targetRow < BoardGeometry::rows) {
                        launchProjectile(Projectile::Type::Pea, targetRow, originX);
                    }
                }
            }
        }
    }
}

void CombatSystem::updateProjectiles(const float deltaSeconds) {
    for (Projectile& projectile : projectiles_) {
        projectile.x += projectileSpeed * deltaSeconds;

        Zombie* target = nullptr;
        for (const auto& zombie : zombies_) {
            if (zombie->isDead() || zombie->row() != projectile.row) {
                continue;
            }
            if (projectile.x + projectileWidth >= zombie->x() &&
                projectile.x <= zombie->x() + zombieWidth &&
                (target == nullptr || zombie->x() < target->x())) {
                target = zombie.get();
            }
        }
        if (target != nullptr) {
            target->receiveDamage(projectile.damage);
            if (projectile.type == Projectile::Type::FrozenPea) {
                target->applySlow(frozenPeaSpeedMultiplier, frozenPeaDurationSeconds);
            }
            projectile.damage = 0;
        }
    }

    const float rightEdge = BoardGeometry::left +
                            BoardGeometry::columns * BoardGeometry::cellWidth;
    projectiles_.erase(
        std::remove_if(
            projectiles_.begin(),
            projectiles_.end(),
            [rightEdge](const Projectile& projectile) {
                return projectile.damage <= 0 || projectile.x > rightEdge;
            }
        ),
        projectiles_.end()
    );
}

void CombatSystem::updateDelayedAreaDamages(const float deltaSeconds) {
    for (DelayedAreaDamage& delayed : delayedAreaDamages_) {
        delayed.remainingSeconds -= deltaSeconds;
        if (delayed.remainingSeconds <= 0.0F) {
            applyAreaDamage(delayed.area);
        }
    }
    delayedAreaDamages_.erase(
        std::remove_if(
            delayedAreaDamages_.begin(),
            delayedAreaDamages_.end(),
            [](const DelayedAreaDamage& delayed) {
                return delayed.remainingSeconds <= 0.0F;
            }
        ),
        delayedAreaDamages_.end()
    );
}

void CombatSystem::updateZombies(const float deltaSeconds, Board& board) {
    for (const auto& zombie : zombies_) {
        if (zombie->isDead()) {
            continue;
        }

        Plant* blockingPlant = nullptr;
        for (int column = BoardGeometry::columns - 1; column >= 0; --column) {
            Plant* plant = board.plantAt({zombie->row(), column});
            if (plant == nullptr || plant->isDead()) {
                continue;
            }
            if (zombie->x() <= plantRight(plant->position()) &&
                zombie->x() + zombieWidth >= plantLeft(plant->position())) {
                blockingPlant = plant;
                break;
            }
        }

        if (blockingPlant == nullptr) {
            zombie->updateAttack(deltaSeconds, false);
            zombie->move(deltaSeconds);
        } else if (zombie->updateAttack(deltaSeconds, true)) {
            blockingPlant->takeDamage(zombie->attackDamage());
        }
        zombie->updateStatusEffects(deltaSeconds);
    }
}

void CombatSystem::updateMowers(const float deltaSeconds) {
    const float rightEdge = BoardGeometry::left +
                            BoardGeometry::columns * BoardGeometry::cellWidth;
    for (LawnMower& mower : mowers_) {
        if (mower.state == LawnMowerState::Ready) {
            const bool shouldTrigger = std::any_of(
                zombies_.begin(),
                zombies_.end(),
                [&mower](const std::unique_ptr<Zombie>& zombie) {
                    return !zombie->isDead() && zombie->row() == mower.row &&
                           zombie->x() <= defenseLineX;
                }
            );
            if (shouldTrigger) {
                mower.state = LawnMowerState::Active;
            }
        }

        if (mower.state == LawnMowerState::Active) {
            mower.x += mowerSpeed * deltaSeconds;
            for (const auto& zombie : zombies_) {
                if (!zombie->isDead() && zombie->row() == mower.row &&
                    zombie->x() <= mower.x + mowerWidth &&
                    zombie->x() + zombieWidth >= mower.x) {
                    zombie->receiveDamage(zombie->armor() + zombie->health());
                }
            }
            if (mower.x > rightEdge) {
                mower.state = LawnMowerState::Used;
            }
        }

        if (mower.state == LawnMowerState::Used) {
            breached_ = breached_ || std::any_of(
                zombies_.begin(),
                zombies_.end(),
                [&mower](const std::unique_ptr<Zombie>& zombie) {
                    return !zombie->isDead() && zombie->row() == mower.row &&
                           zombie->x() <= defenseLineX;
                }
            );
        }
    }
}

void CombatSystem::removeDead() {
    zombies_.erase(
        std::remove_if(
            zombies_.begin(),
            zombies_.end(),
            [](const std::unique_ptr<Zombie>& zombie) {
                return zombie->isDead();
            }
        ),
        zombies_.end()
    );
}

}  // namespace object_project
