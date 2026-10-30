#include "game/EndlessSave.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>

namespace object_project {
namespace {

const std::filesystem::path defaultSavePath{"save/endless_progress.dat"};
constexpr const char* magic = "PVZ_ENDLESS_SAVE_V1";
constexpr std::size_t maximumSavedObjects = 4096;

template <typename T>
bool readCount(std::istream& input, T& value) {
    return static_cast<bool>(input >> value) && value <= maximumSavedObjects;
}

}  // namespace

bool writeEndlessSave(const EndlessSaveData& data, const std::filesystem::path& path) {
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
        return false;
    }
    std::ofstream output(path, std::ios::trunc);
    if (!output) {
        return false;
    }
    output << magic << '\n' << std::max(1, data.highestWave) << ' '
           << (data.hasSnapshot ? 1 : 0) << '\n';
    if (!data.hasSnapshot) {
        return static_cast<bool>(output);
    }

    const GameSessionState& state = data.snapshot;
    output << state.sun << ' ' << state.highestEndlessWave << '\n';
    output << state.level.elapsedSeconds << ' ' << state.level.nextSpawn << ' '
           << state.level.currentWave << ' ' << state.level.endlessSpawnCount << ' '
           << state.level.endlessWaveSpawnCount << ' '
           << state.level.nextEndlessSpawnSeconds << '\n';
    for (const float cooldown : state.cardCooldowns) {
        output << cooldown << ' ';
    }
    output << '\n';

    output << state.plants.size() << '\n';
    for (const PlantState& plant : state.plants) {
        output << static_cast<int>(plant.type) << ' ' << plant.position.row << ' '
               << plant.position.column << ' ' << plant.health << ' '
               << plant.actionAnimationRemaining << ' ' << plant.logicTimer << '\n';
    }

    output << state.suns.spawnTimer << ' ' << state.suns.nextId << ' '
           << std::quoted(state.suns.randomState) << '\n';
    output << state.suns.tokens.size() << '\n';
    for (const SunToken& token : state.suns.tokens) {
        output << token.id << ' ' << token.x << ' ' << token.y << ' ' << token.targetY
               << ' ' << token.ageSeconds << '\n';
    }

    output << state.combat.nextProjectileId << ' ' << state.combat.nextAreaDamageId << ' '
           << (state.combat.breached ? 1 : 0) << '\n';
    output << state.combat.mowers.size() << '\n';
    for (const LawnMower& mower : state.combat.mowers) {
        output << mower.row << ' ' << mower.x << ' ' << static_cast<int>(mower.state) << '\n';
    }
    output << state.combat.zombies.size() << '\n';
    for (const ZombieState& zombie : state.combat.zombies) {
        output << static_cast<int>(zombie.type) << ' ' << zombie.row << ' ' << zombie.x << ' '
               << zombie.health << ' ' << zombie.armor << ' ' << zombie.attackTimer << ' '
               << zombie.slowMultiplier << ' ' << zombie.slowRemainingSeconds << ' '
               << (zombie.attacking ? 1 : 0) << ' ' << (zombie.specialFlag ? 1 : 0) << ' '
               << zombie.specialA << ' ' << zombie.specialB << '\n';
    }
    output << state.combat.projectiles.size() << '\n';
    for (const Projectile& projectile : state.combat.projectiles) {
        output << projectile.id << ' ' << projectile.row << ' ' << projectile.x << ' '
               << projectile.damage << ' ' << static_cast<int>(projectile.type) << ' '
               << projectile.remainingRange << '\n';
    }
    output << state.combat.delayedAreaDamages.size() << '\n';
    for (const DelayedAreaDamage& delayed : state.combat.delayedAreaDamages) {
        output << delayed.id << ' ' << delayed.area.centerRow << ' ' << delayed.area.centerX << ' '
               << delayed.area.rowRadius << ' ' << delayed.area.horizontalRadius << ' '
               << delayed.area.damage << ' ' << delayed.remainingSeconds << '\n';
    }
    return static_cast<bool>(output);
}

EndlessSaveData loadEndlessSave(const std::filesystem::path& path) {
    EndlessSaveData result;
    std::ifstream input(path);
    std::string fileMagic;
    int hasSnapshot = 0;
    if (!(input >> fileMagic) || fileMagic != magic ||
        !(input >> result.highestWave >> hasSnapshot) || result.highestWave < 1) {
        return {};
    }
    result.hasSnapshot = hasSnapshot != 0;
    if (!result.hasSnapshot) {
        return result;
    }

    GameSessionState& state = result.snapshot;
    if (!(input >> state.sun >> state.highestEndlessWave) || state.sun < 0 ||
        state.highestEndlessWave < 1 ||
        !(input >> state.level.elapsedSeconds >> state.level.nextSpawn >> state.level.currentWave
                >> state.level.endlessSpawnCount >> state.level.endlessWaveSpawnCount
                >> state.level.nextEndlessSpawnSeconds)) {
        return {result.highestWave, false, {}};
    }
    for (float& cooldown : state.cardCooldowns) {
        if (!(input >> cooldown)) return {result.highestWave, false, {}};
    }

    std::size_t count = 0;
    if (!readCount(input, count)) return {result.highestWave, false, {}};
    state.plants.resize(count);
    for (PlantState& plant : state.plants) {
        int type = 0;
        if (!(input >> type >> plant.position.row >> plant.position.column >> plant.health
                    >> plant.actionAnimationRemaining >> plant.logicTimer) ||
            type < 0 || type >= static_cast<int>(PlantType::Count)) {
            return {result.highestWave, false, {}};
        }
        plant.type = static_cast<PlantType>(type);
    }

    if (!(input >> state.suns.spawnTimer >> state.suns.nextId >> std::quoted(state.suns.randomState)) ||
        !readCount(input, count)) {
        return {result.highestWave, false, {}};
    }
    state.suns.tokens.resize(count);
    for (SunToken& token : state.suns.tokens) {
        if (!(input >> token.id >> token.x >> token.y >> token.targetY >> token.ageSeconds)) {
            return {result.highestWave, false, {}};
        }
    }

    int breached = 0;
    if (!(input >> state.combat.nextProjectileId >> state.combat.nextAreaDamageId >> breached) ||
        !readCount(input, count) || count != state.combat.mowers.size()) {
        return {result.highestWave, false, {}};
    }
    state.combat.breached = breached != 0;
    for (LawnMower& mower : state.combat.mowers) {
        int mowerState = 0;
        if (!(input >> mower.row >> mower.x >> mowerState) || mowerState < 0 || mowerState > 2) {
            return {result.highestWave, false, {}};
        }
        mower.state = static_cast<LawnMowerState>(mowerState);
    }

    if (!readCount(input, count)) return {result.highestWave, false, {}};
    state.combat.zombies.resize(count);
    for (ZombieState& zombie : state.combat.zombies) {
        int type = 0;
        int attacking = 0;
        int specialFlag = 0;
        if (!(input >> type >> zombie.row >> zombie.x >> zombie.health >> zombie.armor
                    >> zombie.attackTimer >> zombie.slowMultiplier >> zombie.slowRemainingSeconds
                    >> attacking >> specialFlag >> zombie.specialA >> zombie.specialB) ||
            type < 0 || type >= static_cast<int>(ZombieType::Count)) {
            return {result.highestWave, false, {}};
        }
        zombie.type = static_cast<ZombieType>(type);
        zombie.attacking = attacking != 0;
        zombie.specialFlag = specialFlag != 0;
    }

    if (!readCount(input, count)) return {result.highestWave, false, {}};
    state.combat.projectiles.resize(count);
    for (Projectile& projectile : state.combat.projectiles) {
        int type = 0;
        if (!(input >> projectile.id >> projectile.row >> projectile.x >> projectile.damage
                    >> type >> projectile.remainingRange) || type < 0 || type > 3) {
            return {result.highestWave, false, {}};
        }
        projectile.type = static_cast<Projectile::Type>(type);
    }

    if (!readCount(input, count)) return {result.highestWave, false, {}};
    state.combat.delayedAreaDamages.resize(count);
    for (DelayedAreaDamage& delayed : state.combat.delayedAreaDamages) {
        if (!(input >> delayed.id >> delayed.area.centerRow >> delayed.area.centerX
                    >> delayed.area.rowRadius >> delayed.area.horizontalRadius
                    >> delayed.area.damage >> delayed.remainingSeconds)) {
            return {result.highestWave, false, {}};
        }
    }
    return result;
}

bool writeEndlessSave(const EndlessSaveData& data) {
    return writeEndlessSave(data, defaultSavePath);
}

EndlessSaveData loadEndlessSave() {
    return loadEndlessSave(defaultSavePath);
}

}  // namespace object_project
