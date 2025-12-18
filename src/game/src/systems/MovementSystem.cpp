#include <game/systems/MovementSystem.hpp>

void MovementSystem::onUpdate(float dt)
{
    auto& positions = _engine.getComponents<Transform>();
    auto& velocities = _engine.getComponents<Velocity>();

    float scaleFactor = this->_engine.getScaleFactor();

    for (size_t e : _entities) {
        if (!positions[e].has_value() || !velocities[e].has_value())
            continue;

        auto& pos = positions[e].value();
        auto& vel = velocities[e].value();

        pos.x += vel.vx * scaleFactor * dt;
        pos.y += vel.vy * scaleFactor * dt;
    }
}
