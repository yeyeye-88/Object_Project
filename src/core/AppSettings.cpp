#include "core/AppSettings.hpp"

#include <algorithm>

namespace object_project {

void AppSettings::adjustMusicVolume(const int delta) {
    musicVolume_ = std::clamp(musicVolume_ + delta, 0, 100);
}

void AppSettings::adjustEffectsVolume(const int delta) {
    effectsVolume_ = std::clamp(effectsVolume_ + delta, 0, 100);
}

void AppSettings::cycleResolution(const int direction) {
    const int count = static_cast<int>(resolutions.size());
    const int current = static_cast<int>(resolutionIndex_);
    resolutionIndex_ = static_cast<std::size_t>((current + direction + count) % count);
}

}  // namespace object_project
