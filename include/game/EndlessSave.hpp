#pragma once

#include "game/GameSession.hpp"

#include <filesystem>

namespace object_project {

struct EndlessSaveData {
    int highestWave{1};
    bool hasSnapshot{};
    GameSessionState snapshot;
};

EndlessSaveData loadEndlessSave();
bool writeEndlessSave(const EndlessSaveData& data);
EndlessSaveData loadEndlessSave(const std::filesystem::path& path);
bool writeEndlessSave(const EndlessSaveData& data, const std::filesystem::path& path);

}  // namespace object_project
