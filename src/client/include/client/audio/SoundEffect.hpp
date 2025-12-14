#pragma once

#include <string>

namespace client {
namespace audio {

class SoundEffect {
public:
    SoundEffect(const std::string& filepath);
    ~SoundEffect();

    void play(float volume = 1.0f);
    void stop();

    bool isPlaying() const;

private:
    bool m_loaded;
};

} // namespace audio
} // namespace client
