#include <engine/gameEngine/coordinator/ecs/system/systems/AISystem.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>

#include <cmath>
#include <limits>

static inline float safeInvSqrt(float v)
{
    return 1.f / std::sqrt(v + 0.000001f);
}

void AISystem::onUpdate(float dt)
{
    if (dt <= 0.0f)
        return;

    auto& positions  = _coord.getComponents<Transform>();
    auto& velocities = _coord.getComponents<Velocity>();
    auto& ais        = _coord.getComponents<AI>();
    auto& playables  = _coord.getComponents<Playable>();

    size_t playerId = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < playables.size() && i < positions.size(); ++i) {
        if (playables[i].has_value() && positions[i].has_value()) {
            playerId = i;
            break;
        }
    }
    if (playerId == std::numeric_limits<size_t>::max())
        return;

    const Transform& playerPos = positions[playerId].value();

    constexpr float IDLE_DRIFT_X   = -1.0f;
    constexpr float KAMIKAZE_SPEED = 2.0f;

    for (size_t e : _entities) {
        if (e >= positions.size() || e >= velocities.size() || e >= ais.size())
            continue;

        if (!positions[e].has_value() || !velocities[e].has_value() || !ais[e].has_value())
            continue;

        auto& pos = positions[e].value();
        auto& vel = velocities[e].value();
        auto& ai  = ais[e].value();

        ai.internalTime += dt;

        float dx = playerPos.x - pos.x;
        float dy = playerPos.y - pos.y;
        float inv = safeInvSqrt(dx * dx + dy * dy);

        vel.vx = dx * inv * KAMIKAZE_SPEED;
        vel.vy = dy * inv * KAMIKAZE_SPEED;
    }
}
