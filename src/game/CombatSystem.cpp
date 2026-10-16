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

Zombie& CombatSystem::spawnNormalZombie(const int row, const float x) {
    zombies_.push_back(std::make_unique<NormalZombie>(row, x));
    return *zombies_.back();
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
    updateZombies(deltaSeconds, board);
    board.removeDeadPlants();
    updateMowers(deltaSeconds);
    removeDead();
}

void CombatSystem::reset() {
    zombies_.clear();
    projectiles_.clear();
    nextProjectileId_ = 1;
    breached_ = false;
    for (int row = 0; row < BoardGeometry::rows; ++row) {
        mowers_[static_cast<std::size_t>(row)] = {row, mowerStartX, LawnMowerState::Ready};
    }
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
            const auto action = plant->update(
                deltaSeconds,
                hasZombieAhead(row, originX)
            );
            if (action == PlantAction::ProduceSun) {
                suns.spawnFromPlant(plant->position());
            } else if (action == PlantAction::FirePea) {
                projectiles_.push_back({nextProjectileId_++, row, originX, projectileDamage});
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
            target->takeDamage(projectile.damage);
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
                    zombie->takeDamage(zombie->health());
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
