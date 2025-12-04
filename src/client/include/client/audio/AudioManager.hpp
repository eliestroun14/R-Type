#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace client {
namespace audio {

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    void shutdown();

    bool loadSound(const std::string& name, const std::string& filepath);
    bool loadMusic(const std::string& name, const std::string& filepath);

    void playSound(const std::string& name, float volume = 1.0f);
    void playMusic(const std::string& name, float volume = 1.0f, bool loop = true);
    void stopMusic();

    void setMasterVolume(float volume);
    void setSoundVolume(float volume);
    void setMusicVolume(float volume);

private:
    bool m_initialized;
    float m_masterVolume = 1.0f;
    float m_soundVolume = 1.0f;
    float m_musicVolume = 1.0f;
};

} // namespace audio
} // namespace client
