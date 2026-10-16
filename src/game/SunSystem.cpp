#include "game/SunSystem.hpp"

#include <algorithm>

namespace object_project {

SunSystem::SunSystem(const std::uint32_t seed)
    : initialSeed_(seed), random_(seed) {}

void SunSystem::update(const float deltaSeconds) {
    if (deltaSeconds <= 0.0F) {
        return;
    }

    spawnTimer_ += deltaSeconds;
    while (spawnTimer_ >= spawnIntervalSeconds) {
        spawnTimer_ -= spawnIntervalSeconds;
        spawn();
    }

    for (SunToken& token : tokens_) {
        token.ageSeconds += deltaSeconds;
        token.y = std::min(token.targetY, token.y + fallSpeed * deltaSeconds);
    }

    tokens_.erase(
        std::remove_if(
            tokens_.begin(),
            tokens_.end(),
            [](const SunToken& token) {
                return token.ageSeconds >= lifetimeSeconds;
            }
        ),
        tokens_.end()
    );
}

bool SunSystem::collectAt(const float x, const float y) {
    const auto found = std::find_if(
        tokens_.begin(),
        tokens_.end(),
        [x, y](const SunToken& token) {
            return x >= token.x && x <= token.x + tokenSize && y >= token.y &&
                   y <= token.y + tokenSize;
        }
    );

    if (found == tokens_.end()) {
        return false;
    }

    tokens_.erase(found);
    return true;
}

void SunSystem::reset() {
    random_.seed(initialSeed_);
    tokens_.clear();
    spawnTimer_ = 0.0F;
    nextId_ = 1;
}

void SunSystem::spawn() {
    const float minimumX = BoardGeometry::left + 8.0F;
    const float maximumX = BoardGeometry::left +
                           BoardGeometry::columns * BoardGeometry::cellWidth -
                           tokenSize - 8.0F;
    const float minimumY = BoardGeometry::top + BoardGeometry::cellHeight;
    const float maximumY = BoardGeometry::top +
                           BoardGeometry::rows * BoardGeometry::cellHeight -
                           tokenSize;

    std::uniform_real_distribution<float> xDistribution(minimumX, maximumX);
    std::uniform_real_distribution<float> yDistribution(minimumY, maximumY);
    tokens_.push_back({nextId_++, xDistribution(random_), -tokenSize, yDistribution(random_), 0.0F});
}

}  // namespace object_project
