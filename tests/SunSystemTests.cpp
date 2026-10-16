#include "game/SunSystem.hpp"

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
    SunSystem suns(42U);

    suns.update(SunSystem::spawnIntervalSeconds - 0.1F);
    passed &= check(suns.tokens().empty(), "sun does not spawn early");
    suns.update(0.1F);
    passed &= check(suns.tokens().size() == 1, "sun spawns on schedule");

    const SunToken token = suns.tokens().front();
    passed &= check(token.x >= BoardGeometry::left, "spawned sun stays over the lawn");
    passed &= check(suns.collectAt(token.x + 2.0F, token.y + 2.0F), "click inside sun collects it");
    passed &= check(suns.tokens().empty(), "collected sun is removed");
    passed &= check(!suns.collectAt(token.x, token.y), "same sun cannot be collected twice");

    suns.update(SunSystem::spawnIntervalSeconds);
    const std::uint64_t firstId = suns.tokens().front().id;
    suns.update(SunSystem::lifetimeSeconds);
    bool firstStillExists = false;
    for (const SunToken& current : suns.tokens()) {
        firstStillExists |= current.id == firstId;
    }
    passed &= check(!firstStillExists, "expired sun is removed");
    suns.reset();
    passed &= check(suns.tokens().empty(), "reset clears all suns");
    return passed ? 0 : 1;
}
