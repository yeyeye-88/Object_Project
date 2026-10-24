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
    Spore,
    Hit,
    Wave,
    Ready,
    Explosion,
    PotatoExplosion,
    Mower,
    PoleVault,
    Newspaper,
    Chomp,
    FirePea,
    Win,
    Lose,
    Count,
};

enum class MusicTrack {
    Menu,
    Gameplay,
    Count,
};

class AudioSystem {
public:
    AudioSystem();

    void applySettings(const AppSettings& settings);
    void play(SoundCue cue);
    void playMusic(MusicTrack track);

private:
    static constexpr std::size_t cueCount = static_cast<std::size_t>(SoundCue::Count);
    static constexpr std::size_t musicTrackCount = static_cast<std::size_t>(MusicTrack::Count);

    std::array<sf::SoundBuffer, musicTrackCount> musicBuffers_;
    std::optional<sf::Sound> music_;
    std::optional<MusicTrack> activeMusicTrack_;
    std::array<sf::SoundBuffer, cueCount> buffers_;
    std::array<std::optional<sf::Sound>, cueCount> sounds_;
    float effectsVolume_{70.0F};
    float musicVolume_{70.0F};
};

}  // namespace object_project
