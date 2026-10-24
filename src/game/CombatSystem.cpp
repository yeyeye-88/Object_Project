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
    SunSystem& suns,
    const float sunflowerProductionIntervalMultiplier
) {
    if (deltaSeconds <= 0.0F || breached_) {
        return;
    }

    updatePlants(deltaSeconds, board, suns, sunflowerProductionIntervalMultiplier);
    updateProjectiles(deltaSeconds, board);
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
    events_.clear();
    explosionEvents_.clear();
    projectileImpactEvents_.clear();
    nextProjectileId_ = 1;
    nextAreaDamageId_ = 1;
    breached_ = false;
    for (int row = 0; row < BoardGeometry::rows; ++row) {
        mowers_[static_cast<std::size_t>(row)] = {row, mowerStartX, LawnMowerState::Ready};
    }
}

std::vector<CombatEvent> CombatSystem::takeEvents() {
    std::vector<CombatEvent> events;
    events.swap(events_);
    return events;
}

std::vector<ExplosionEvent> CombatSystem::takeExplosionEvents() {
    std::vector<ExplosionEvent> events;
    events.swap(explosionEvents_);
    return events;
}

std::vector<ProjectileImpactEvent> CombatSystem::takeProjectileImpactEvents() {
    std::vector<ProjectileImpactEvent> events;
    events.swap(projectileImpactEvents_);
    return events;
}

void CombatSystem::launchProjectile(
    const Projectile::Type type,
    const int row,
    const float x,
    const int damage,
    const float maximumRange
) {
    if (row < 0 || row >= BoardGeometry::rows || damage <= 0) {
        return;
    }
    projectiles_.push_back({nextProjectileId_++, row, x, damage, type, maximumRange});
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

bool CombatSystem::hasZombieInRange(
    const int row,
    const float x,
    const float range
) const {
    return std::any_of(
        zombies_.begin(),
        zombies_.end(),
        [row, x, range](const std::unique_ptr<Zombie>& zombie) {
            const float center = zombie->x() + zombieWidth * 0.5F;
            return !zombie->isDead() && zombie->row() == row &&
                   center > x && center <= x + range;
        }
    );
}

bool CombatSystem::hasZombieNear(const int row, const float x, const float radius) const {
    return std::any_of(
        zombies_.begin(),
        zombies_.end(),
        [row, x, radius](const std::unique_ptr<Zombie>& zombie) {
            const float center = zombie->x() + zombieWidth * 0.5F;
            return !zombie->isDead() && zombie->row() == row &&
                   std::abs(center - x) <= radius;
        }
    );
}

bool CombatSystem::hasZombieOverlappingPlant(const GridPosition position) const {
    const float left = plantLeft(position);
    const float right = plantRight(position);
    return std::any_of(
        zombies_.begin(),
        zombies_.end(),
        [position, left, right](const std::unique_ptr<Zombie>& zombie) {
            return !zombie->isDead() && zombie->row() == position.row &&
                   zombie->x() <= right && zombie->x() + zombieWidth >= left;
        }
    );
}

void CombatSystem::damageNearestZombie(
    const int row,
    const float x,
    const float radius,
    const int damage
) {
    Zombie* nearest = nullptr;
    float nearestDistance = radius;
    for (const auto& zombie : zombies_) {
        if (zombie->isDead() || zombie->row() != row) {
            continue;
        }
        const float distance = std::abs(zombie->x() + zombieWidth * 0.5F - x);
        if (distance <= nearestDistance) {
            nearest = zombie.get();
            nearestDistance = distance;
        }
    }
    if (nearest != nullptr) {
        nearest->receiveDamage(damage);
    }
}

void CombatSystem::updatePlants(
    const float deltaSeconds,
    Board& board,
    SunSystem& suns,
    const float sunflowerProductionIntervalMultiplier
) {
    for (int row = 0; row < BoardGeometry::rows; ++row) {
        for (int column = 0; column < BoardGeometry::columns; ++column) {
            Plant* plant = board.plantAt({row, column});
            if (plant == nullptr || plant->isDead()) {
                continue;
            }

            const float originX = BoardGeometry::cellLeft(column) + BoardGeometry::cellWidth * 0.72F;
            const float plantCenterX = BoardGeometry::cellLeft(column) +
                                       BoardGeometry::cellWidth * 0.5F;
            PlantUpdateContext context{
                hasZombieAhead(row, originX),
                hasZombieOverlappingPlant(plant->position()),
                sunflowerProductionIntervalMultiplier,
            };
            if (plant->type() == PlantType::PuffShroom) {
                context.enemyAhead = hasZombieInRange(
                    row,
                    originX,
                    PuffShroom::attackRange
                );
            }
            if (plant->type() == PlantType::Threepeater) {
                context.enemyAhead = context.enemyAhead ||
                    (row > 0 && hasZombieAhead(row - 1, originX)) ||
                    (row + 1 < BoardGeometry::rows && hasZombieAhead(row + 1, originX));
            }
            const GridPosition plantPosition = plant->position();
            plant->updateActionAnimation(deltaSeconds);
            const auto action = plant->update(deltaSeconds, context);
            if (action == PlantAction::ProduceSun) {
                plant->beginActionAnimation();
                suns.spawnFromPlant(plant->position());
            } else if (action == PlantAction::FirePea) {
                plant->beginActionAnimation();
                launchProjectile(Projectile::Type::Pea, row, originX);
                events_.push_back(CombatEvent::ProjectileFired);
            } else if (action == PlantAction::FireDoublePea) {
                plant->beginActionAnimation();
                launchProjectile(Projectile::Type::Pea, row, originX);
                launchProjectile(Projectile::Type::Pea, row, originX - 14.0F);
                events_.push_back(CombatEvent::ProjectileFired);
            } else if (action == PlantAction::FireThreeRows) {
                plant->beginActionAnimation();
                for (int targetRow = row - 1; targetRow <= row + 1; ++targetRow) {
                    if (targetRow >= 0 && targetRow < BoardGeometry::rows) {
                        launchProjectile(Projectile::Type::Pea, targetRow, originX);
                    }
                }
                events_.push_back(CombatEvent::ProjectileFired);
            } else if (action == PlantAction::FireFrozenPea) {
                plant->beginActionAnimation();
                launchProjectile(Projectile::Type::FrozenPea, row, originX);
                events_.push_back(CombatEvent::ProjectileFired);
            } else if (action == PlantAction::FireSpore) {
                plant->beginActionAnimation();
                launchProjectile(
                    Projectile::Type::Spore,
                    row,
                    originX,
                    PuffShroom::sporeDamage,
                    PuffShroom::attackRange
                );
                events_.push_back(CombatEvent::SporeFired);
            } else if (action == PlantAction::ExplodePotatoMine) {
                damageNearestZombie(row, plantCenterX, potatoMineTriggerRadius, potatoMineDamage);
                explosionEvents_.push_back({plantPosition, PlantType::PotatoMine});
                board.remove(plantPosition);
            } else if (action == PlantAction::ExplodeCherryBomb) {
                applyAreaDamage({
                    row,
                    plantCenterX,
                    1,
                    cherryBombHorizontalRadius,
                    cherryBombDamage,
                });
                explosionEvents_.push_back({plantPosition, PlantType::CherryBomb});
                board.remove(plantPosition);
            }
        }
    }
}

void CombatSystem::updateProjectiles(const float deltaSeconds, const Board& board) {
    for (Projectile& projectile : projectiles_) {
        const float previousX = projectile.x;
        projectile.x += projectileSpeed * deltaSeconds;
        if (projectile.remainingRange >= 0.0F) {
            projectile.remainingRange -= projectileSpeed * deltaSeconds;
        }

        if (projectile.type == Projectile::Type::Pea) {
            for (int column = 0; column < BoardGeometry::columns; ++column) {
                const Plant* plant = board.plantAt({projectile.row, column});
                if (plant == nullptr || plant->type() != PlantType::Torchwood) {
                    continue;
                }
                const float torchwoodX = BoardGeometry::cellLeft(column) +
                                         BoardGeometry::cellWidth * 0.5F;
                if (previousX <= torchwoodX && projectile.x >= torchwoodX) {
                    projectile.type = Projectile::Type::FirePea;
                    projectile.damage *= firePeaDamageMultiplier;
                    events_.push_back(CombatEvent::FirePeaIgnited);
                    break;
                }
            }
        }

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
            const int armorBeforeHit = target->armor();
            target->receiveDamage(projectile.damage);
            projectileImpactEvents_.push_back({
                projectile.type,
                projectile.row,
                projectile.x,
            });
            events_.push_back(CombatEvent::ProjectileHit);
            if (target->type() == ZombieType::Newspaper && armorBeforeHit > 0 &&
                target->armor() == 0) {
                events_.push_back(CombatEvent::NewspaperBroken);
            }
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
                return projectile.damage <= 0 || projectile.x > rightEdge ||
                       (projectile.remainingRange >= 0.0F && projectile.remainingRange <= 0.0F);
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
            if (!zombie->isVaulting()) {
                zombie->move(deltaSeconds);
            }
        } else if (zombie->canVault() && !blockingPlant->blocksVaulting()) {
            const float vaultStartX = zombie->x();
            zombie->setX(
                plantLeft(blockingPlant->position()) - zombieWidth - 2.0F
            );
            zombie->useVault(vaultStartX);
            events_.push_back(CombatEvent::PoleVaulted);
            zombie->updateAttack(deltaSeconds, false);
        } else if (zombie->updateAttack(deltaSeconds, true)) {
            blockingPlant->takeDamage(zombie->attackDamage());
            events_.push_back(CombatEvent::ZombieBite);
        }
        zombie->updateStatusEffects(deltaSeconds);
        zombie->updateSpecialState(deltaSeconds);
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
                events_.push_back(CombatEvent::MowerStarted);
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
