#include "game/AlmanacData.hpp"

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

    for (std::size_t index = 0; index < plantTypeCount; ++index) {
        const AlmanacEntry& entry = plantAlmanacEntry(static_cast<PlantType>(index));
        passed &= check(!entry.name.empty() && !entry.ability.empty(), "plant entry has a name and ability");
    }
    for (std::size_t index = 0; index < zombieTypeCount; ++index) {
        const AlmanacEntry& entry = zombieAlmanacEntry(static_cast<ZombieType>(index));
        passed &= check(!entry.name.empty() && !entry.stats.empty(), "zombie entry has a name and stats");
    }
    return passed ? 0 : 1;
}
