#pragma once

#include "game/BoardGeometry.hpp"

#include <cstdint>
#include <random>
#include <vector>

namespace object_project {

struct SunToken {
    std::uint64_t id{};
    float x{};
    float y{};
    float targetY{};
    float ageSeconds{};
};

class SunSystem {
public:
    static constexpr int sunValue = 25;
    static constexpr float tokenSize = 64.0F;
    static constexpr float spawnIntervalSeconds = 5.0F;
    static constexpr float lifetimeSeconds = 10.0F;
    static constexpr float fallSpeed = 90.0F;

    explicit SunSystem(std::uint32_t seed = 0xC0FFEEU);

    void update(float deltaSeconds);
    bool collectAt(float x, float y);
    void spawnFromPlant(GridPosition position);
    void reset();

    const std::vector<SunToken>& tokens() const { return tokens_; }

private:
    void spawn();

    const std::uint32_t initialSeed_;
    std::mt19937 random_;
    std::vector<SunToken> tokens_;
    float spawnTimer_{};
    std::uint64_t nextId_{1};
};

}  // namespace object_project
