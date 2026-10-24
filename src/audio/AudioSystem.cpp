#include "audio/AudioSystem.hpp"

#include <array>
#include <string>
#include <string_view>

namespace object_project {

AudioSystem::AudioSystem() {
    static constexpr std::array<std::string_view, cueCount> soundPaths{{
        "assets/pvz/audio/buttonclick.ogg",
        "assets/pvz/audio/points.ogg",
        "assets/pvz/audio/plant1.ogg",
        "assets/pvz/audio/shoop.ogg",
        "assets/pvz/audio/puff.ogg",
        "assets/pvz/audio/splat1.ogg",
        "assets/pvz/audio/hugewave.ogg",
        "assets/pvz/audio/readysetplant.ogg",
        "assets/pvz/audio/cherrybomb.ogg",
        "assets/pvz/audio/potato_mine.ogg",
        "assets/pvz/audio/lawnmower.ogg",
        "assets/pvz/audio/polevault.ogg",
        "assets/pvz/audio/newspaper_rarrgh2.ogg",
        "assets/pvz/audio/chomp.ogg",
        "assets/pvz/audio/firepea.ogg",
        "assets/pvz/audio/winmusic.ogg",
        "assets/pvz/audio/losemusic.ogg",
    }};
    for (std::size_t index = 0; index < cueCount; ++index) {
        if (buffers_[index].loadFromFile(std::string(soundPaths[index]))) {
            sounds_[index].emplace(buffers_[index]);
        }
    }

    static constexpr std::array<std::string_view, musicTrackCount> musicPaths{{
        "assets/pvz/audio/Mountains.ogg",
        "assets/pvz/audio/UraniwaNi.ogg",
    }};
    for (std::size_t index = 0; index < musicTrackCount; ++index) {
        if (!musicBuffers_[index].loadFromFile(std::string(musicPaths[index]))) {
            continue;
        }
    }
    playMusic(MusicTrack::Menu);
}

void AudioSystem::applySettings(const AppSettings& settings) {
    effectsVolume_ = static_cast<float>(settings.effectsVolume());
    musicVolume_ = static_cast<float>(settings.musicVolume());
    if (music_) {
        music_->setVolume(musicVolume_);
    }
}

void AudioSystem::playMusic(const MusicTrack track) {
    if (activeMusicTrack_ == track && music_ && music_->getStatus() == sf::Sound::Status::Playing) {
        return;
    }
    const std::size_t index = static_cast<std::size_t>(track);
    if (index >= musicBuffers_.size() || musicBuffers_[index].getSampleCount() == 0) {
        return;
    }
    music_.emplace(musicBuffers_[index]);
    music_->setLooping(true);
    music_->setVolume(musicVolume_);
    music_->play();
    activeMusicTrack_ = track;
}

void AudioSystem::play(const SoundCue cue) {
    const std::size_t index = static_cast<std::size_t>(cue);
    if (index >= sounds_.size() || !sounds_[index]) {
        return;
    }
    sounds_[index]->setVolume(effectsVolume_);
    sounds_[index]->play();
}

}  // namespace object_project
