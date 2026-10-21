#include "audio/AudioSystem.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace object_project {

namespace {

constexpr unsigned int sampleRate = 22050;
constexpr float pi = 3.14159265358979323846F;

std::int16_t sampleFor(const float phase, const float envelope, const float amplitude = 0.22F) {
    return static_cast<std::int16_t>(
        std::sin(phase) * envelope * amplitude * 32767.0F
    );
}

}  // namespace

AudioSystem::AudioSystem() {
    const std::array<float, cueCount> frequencies{{620.0F, 880.0F, 420.0F, 540.0F, 170.0F, 260.0F}};
    const std::array<float, cueCount> lengths{{0.07F, 0.18F, 0.12F, 0.07F, 0.09F, 0.42F}};
    for (std::size_t index = 0; index < cueCount; ++index) {
        buffers_[index] = makeTone(
            frequencies[index],
            lengths[index],
            index == static_cast<std::size_t>(SoundCue::Hit)
        );
        sounds_[index].emplace(buffers_[index]);
    }

    musicBuffer_ = makeMusic();
    music_.emplace(musicBuffer_);
    music_->setLooping(true);
    music_->play();
}

void AudioSystem::applySettings(const AppSettings& settings) {
    effectsVolume_ = static_cast<float>(settings.effectsVolume());
    if (music_) {
        music_->setVolume(static_cast<float>(settings.musicVolume()));
    }
}

void AudioSystem::play(const SoundCue cue) {
    const std::size_t index = static_cast<std::size_t>(cue);
    if (index >= sounds_.size() || !sounds_[index]) {
        return;
    }
    sounds_[index]->setVolume(effectsVolume_);
    sounds_[index]->play();
}

sf::SoundBuffer AudioSystem::makeTone(
    const float frequency,
    const float seconds,
    const bool descending
) const {
    const std::size_t count = static_cast<std::size_t>(sampleRate * seconds);
    std::vector<std::int16_t> samples(count);
    float phase = 0.0F;
    for (std::size_t index = 0; index < count; ++index) {
        const float progress = static_cast<float>(index) / static_cast<float>(count);
        const float currentFrequency = descending
            ? frequency * (1.0F - progress * 0.45F)
            : frequency;
        phase += 2.0F * pi * currentFrequency / static_cast<float>(sampleRate);
        const float envelope = std::min(progress * 12.0F, 1.0F) * (1.0F - progress);
        samples[index] = sampleFor(phase, envelope);
    }
    return sf::SoundBuffer(
        samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono}
    );
}

sf::SoundBuffer AudioSystem::makeMusic() const {
    constexpr std::array<float, 8> notes{{261.63F, 329.63F, 392.00F, 329.63F,
                                          293.66F, 349.23F, 440.00F, 349.23F}};
    constexpr float noteSeconds = 0.42F;
    const std::size_t samplesPerNote = static_cast<std::size_t>(sampleRate * noteSeconds);
    std::vector<std::int16_t> samples(samplesPerNote * notes.size());
    for (std::size_t note = 0; note < notes.size(); ++note) {
        for (std::size_t offset = 0; offset < samplesPerNote; ++offset) {
            const float progress = static_cast<float>(offset) /
                                   static_cast<float>(samplesPerNote);
            const float phase = 2.0F * pi * notes[note] *
                                static_cast<float>(offset) /
                                static_cast<float>(sampleRate);
            const float envelope = std::min(progress * 8.0F, 1.0F) *
                                   std::min((1.0F - progress) * 5.0F, 1.0F);
            samples[note * samplesPerNote + offset] = sampleFor(phase, envelope, 0.08F);
        }
    }
    return sf::SoundBuffer(
        samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono}
    );
}

}  // namespace object_project
