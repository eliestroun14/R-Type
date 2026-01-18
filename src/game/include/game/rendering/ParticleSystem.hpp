#pragma once

#include <vector>

namespace client {
namespace rendering {

struct Particle {
    float x, y;
    float velocityX, velocityY;
    float life;
    uint32_t color;
};

class ParticleSystem {
public:
    void update(float deltaTime);
    void render();

    void emit(float x, float y, int count);
    void clear();

private:
    std::vector<Particle> m_particles;
};

} // namespace rendering
} // namespace client
