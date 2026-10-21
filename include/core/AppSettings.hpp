#pragma once

#include <array>
#include <cstddef>

namespace object_project {

struct WindowResolution {
    unsigned int width{};
    unsigned int height{};
};

class AppSettings {
public:
    static constexpr std::array<WindowResolution, 3> resolutions{{
        {960, 600},
        {1280, 800},
        {1600, 1000},
    }};

    int musicVolume() const { return musicVolume_; }
    int effectsVolume() const { return effectsVolume_; }
    bool fullscreen() const { return fullscreen_; }
    std::size_t resolutionIndex() const { return resolutionIndex_; }
    WindowResolution resolution() const { return resolutions[resolutionIndex_]; }

    void adjustMusicVolume(int delta);
    void adjustEffectsVolume(int delta);
    void toggleFullscreen() { fullscreen_ = !fullscreen_; }
    void cycleResolution(int direction);

private:
    int musicVolume_{45};
    int effectsVolume_{70};
    bool fullscreen_{};
    std::size_t resolutionIndex_{};
};

}  // namespace object_project
