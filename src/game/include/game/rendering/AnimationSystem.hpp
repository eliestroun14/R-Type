#pragma once

#include <vector>
#include <cstdint>

namespace client {
namespace rendering {

struct AnimationFrame {
    uint32_t textureId;
    float duration;
};

class Animation {
public:
    void addFrame(uint32_t textureId, float duration);
    uint32_t getCurrentFrame(float time) const;
    bool isLooping() const { return m_loop; }
    void setLooping(bool loop) { m_loop = loop; }

private:
    std::vector<AnimationFrame> m_frames;
    bool m_loop = true;
};

class AnimationSystem {
public:
    void update(float deltaTime);
    void addAnimation(uint32_t entityId, Animation animation);
    void removeAnimation(uint32_t entityId);

private:
    std::unordered_map<uint32_t, std::pair<Animation, float>> m_animations;
};

} // namespace rendering
} // namespace client
