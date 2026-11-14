#include "game/LevelLoader.hpp"

#include "game/BoardGeometry.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace object_project {

namespace {

std::string trim(std::string value) {
    const auto notSpace = [](const unsigned char character) {
        return !std::isspace(character);
    };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

std::vector<std::string> split(const std::string& value, const char separator) {
    std::vector<std::string> result;
    std::stringstream stream(value);
    std::string part;
    while (std::getline(stream, part, separator)) {
        result.push_back(trim(part));
    }
    return result;
}

int parseInt(const std::string& value, const std::string& location) {
    std::size_t consumed = 0;
    const int parsed = std::stoi(value, &consumed);
    if (consumed != value.size()) {
        throw std::runtime_error(location + ": invalid integer '" + value + "'");
    }
    return parsed;
}

float parseFloat(const std::string& value, const std::string& location) {
    std::size_t consumed = 0;
    const float parsed = std::stof(value, &consumed);
    if (consumed != value.size()) {
        throw std::runtime_error(location + ": invalid number '" + value + "'");
    }
    return parsed;
}

template <typename Value>
Value lookup(
    const std::unordered_map<std::string, Value>& values,
    const std::string& name,
    const std::string& location
) {
    const auto found = values.find(name);
    if (found == values.end()) {
        throw std::runtime_error(location + ": unknown value '" + name + "'");
    }
    return found->second;
}

LevelDefinition loadLevelFile(
    const std::filesystem::path& path,
    const LevelId expectedId
) {
    static const std::unordered_map<std::string, LevelId> levelIds{
        {"tutorial", LevelId::Tutorial}, {"garden", LevelId::Garden},
        {"night_challenge", LevelId::NightChallenge},
        {"endless_defense", LevelId::EndlessDefense},
    };
    static const std::unordered_map<std::string, GameMode> gameModes{
        {"standard", GameMode::Standard}, {"challenge", GameMode::Challenge},
        {"endless", GameMode::Endless},
    };
    static const std::unordered_map<std::string, PlantType> plants{
        {"sunflower", PlantType::Sunflower}, {"peashooter", PlantType::Peashooter},
        {"wallnut", PlantType::WallNut}, {"repeater", PlantType::Repeater},
        {"threepeater", PlantType::Threepeater}, {"snowpea", PlantType::SnowPea},
        {"tallnut", PlantType::TallNut}, {"potato_mine", PlantType::PotatoMine},
        {"cherry_bomb", PlantType::CherryBomb}, {"torchwood", PlantType::Torchwood},
        {"puffshroom", PlantType::PuffShroom},
    };
    static const std::unordered_map<std::string, ZombieType> zombies{
        {"normal", ZombieType::Normal}, {"conehead", ZombieType::Conehead},
        {"buckethead", ZombieType::Buckethead}, {"flag", ZombieType::Flag},
        {"pole_vaulting", ZombieType::PoleVaulting}, {"newspaper", ZombieType::Newspaper},
        {"screen_door", ZombieType::ScreenDoor}, {"football", ZombieType::Football},
        {"dancing", ZombieType::Dancing}, {"backup_dancer", ZombieType::BackupDancer},
    };

    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("cannot open " + path.string());
    }

    LevelDefinition result;
    bool hasId = false;
    bool hasMode = false;
    bool hasName = false;
    bool hasDescription = false;
    bool hasInitialSun = false;
    bool hasWaveCount = false;
    bool hasPlants = false;
    std::string line;
    int lineNumber = 0;
    float previousSpawnTime = -1.0F;
    while (std::getline(input, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty() || line.front() == '#') {
            continue;
        }
        const std::size_t equals = line.find('=');
        const std::string location = path.string() + ':' + std::to_string(lineNumber);
        if (equals == std::string::npos) {
            throw std::runtime_error(location + ": expected key=value");
        }
        const std::string key = trim(line.substr(0, equals));
        const std::string value = trim(line.substr(equals + 1));
        if (key == "id") {
            result.id = lookup(levelIds, value, location);
            hasId = true;
        } else if (key == "mode") {
            result.mode = lookup(gameModes, value, location);
            hasMode = true;
        } else if (key == "name") {
            result.name = value;
            hasName = !value.empty();
        } else if (key == "description") {
            result.description = value;
            hasDescription = !value.empty();
        } else if (key == "initial_sun") {
            result.initialSun = parseInt(value, location);
            hasInitialSun = true;
        } else if (key == "wave_count") {
            result.waveCount = parseInt(value, location);
            hasWaveCount = true;
        } else if (key == "plants") {
            result.availablePlants.clear();
            for (const std::string& plant : split(value, ',')) {
                result.availablePlants.push_back(lookup(plants, plant, location));
            }
            hasPlants = !result.availablePlants.empty();
        } else if (key == "spawn") {
            const std::vector<std::string> fields = split(value, ',');
            if (fields.size() != 4) {
                throw std::runtime_error(location + ": spawn needs time,row,zombie,wave");
            }
            SpawnEvent event{
                parseFloat(fields[0], location), parseInt(fields[1], location),
                lookup(zombies, fields[2], location), parseInt(fields[3], location),
            };
            if (event.timeSeconds < previousSpawnTime || event.row < 0 ||
                event.row >= BoardGeometry::rows || event.wave < 1) {
                throw std::runtime_error(location + ": invalid spawn order, row, or wave");
            }
            previousSpawnTime = event.timeSeconds;
            result.schedule.push_back(event);
        } else {
            throw std::runtime_error(location + ": unknown key '" + key + "'");
        }
    }

    if (!hasId || !hasMode || !hasName || !hasDescription || !hasInitialSun ||
        !hasWaveCount || !hasPlants) {
        throw std::runtime_error(path.string() + ": missing required level field");
    }
    if (result.id != expectedId || result.initialSun < 0 || result.waveCount < 0) {
        throw std::runtime_error(path.string() + ": invalid id, sun, or wave count");
    }
    if (result.mode == GameMode::Endless) {
        if (result.waveCount != 0 || !result.schedule.empty()) {
            throw std::runtime_error(path.string() + ": endless mode must use dynamic waves");
        }
    } else if (result.waveCount < 1 || result.schedule.empty() ||
               std::any_of(result.schedule.begin(), result.schedule.end(),
                   [&](const SpawnEvent& event) { return event.wave > result.waveCount; })) {
        throw std::runtime_error(path.string() + ": fixed mode needs a valid schedule");
    }
    return result;
}

}  // namespace

std::array<LevelDefinition, levelCount> loadLevelDefinitions(
    const std::filesystem::path& directory
) {
    return {{
        loadLevelFile(directory / "tutorial.level", LevelId::Tutorial),
        loadLevelFile(directory / "garden.level", LevelId::Garden),
        loadLevelFile(directory / "night_challenge.level", LevelId::NightChallenge),
        loadLevelFile(directory / "endless_defense.level", LevelId::EndlessDefense),
    }};
}

}  // namespace object_project
