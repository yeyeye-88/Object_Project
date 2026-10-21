#include "core/AppSettings.hpp"

#include <iostream>

int main() {
    object_project::AppSettings settings;
    bool passed = true;

    settings.adjustMusicVolume(1000);
    passed &= settings.musicVolume() == 100;
    settings.adjustMusicVolume(-1000);
    passed &= settings.musicVolume() == 0;

    settings.adjustEffectsVolume(-1000);
    passed &= settings.effectsVolume() == 0;
    settings.adjustEffectsVolume(40);
    passed &= settings.effectsVolume() == 40;

    settings.cycleResolution(-1);
    passed &= settings.resolutionIndex() == object_project::AppSettings::resolutions.size() - 1;
    settings.cycleResolution(1);
    passed &= settings.resolutionIndex() == 0;

    settings.toggleFullscreen();
    passed &= settings.fullscreen();

    std::cout << (passed ? "[PASS] app settings\n" : "[FAIL] app settings\n");
    return passed ? 0 : 1;
}
