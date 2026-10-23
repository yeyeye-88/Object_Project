#pragma once

#include "core/AppSettings.hpp"

#include <SFML/Audio.hpp>

#include <array>
#include <optional>

namespace object_project {

enum class SoundCue {
    Click,
    CollectSun,
    Plant,
    Attack,
    Hit,
    Wave,
    Ready,
    Explosion,
    Mower,
    PoleVault,
    Newspaper,
    Chomp,
    FirePea,
    Win,
    Lose,
    Count,
};

class AudioSystem {
public:
    AudioSystem();

    void applySettings(const AppSettings& settings);
    void play(SoundCue cue);

private:
    static constexpr std::size_t cueCount = static_cast<std::size_t>(SoundCue::Count);

    sf::SoundBuffer musicBuffer_;
    std::optional<sf::Sound> music_;
    std::array<sf::SoundBuffer, cueCount> buffers_;
    std::array<std::optional<sf::Sound>, cueCount> sounds_;
    float effectsVolume_{70.0F};
};

}  // namespace object_project
