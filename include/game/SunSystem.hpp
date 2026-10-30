#pragma once

#include "game/BoardGeometry.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace object_project {

struct SunToken {
    std::uint64_t id{};
    float x{};
    float y{};
    float targetY{};
    float ageSeconds{};
};

struct SunSystemState {
    std::vector<SunToken> tokens;
    float spawnTimer{};
    std::uint64_t nextId{1};
    std::string randomState;
};

class SunSystem {
public:
    static constexpr int sunValue = 25;
    static constexpr float tokenSize = 64.0F;
    static constexpr float spawnIntervalSeconds = 5.0F;
    static constexpr float lifetimeSeconds = 10.0F;
    static constexpr float fallSpeed = 90.0F;

    explicit SunSystem(std::uint32_t seed = 0xC0FFEEU);

    void update(float deltaSeconds, bool skySunEnabled = true);
    bool collectAt(float x, float y);
    void spawnFromPlant(GridPosition position);
    void reset();
    SunSystemState captureState() const;
    bool restoreState(const SunSystemState& state);

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
