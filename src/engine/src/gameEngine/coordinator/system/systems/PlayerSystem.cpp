/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/PlayerSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>

void PlayerSystem::onUpdate(float dt)
{
    auto& playables = this->_coordinator.getComponents<Playable>();
    auto& velocities = this->_coordinator.getComponents<Velocity>();
    auto& transforms = this->_coordinator.getComponents<Transform>();

    for (size_t e : this->_entities) {
        if (!playables[e] || !velocities[e])
            continue;

        auto& vel = velocities[e].value();
        auto& trans = transforms[e].value();

        float vx = 0.0f;
        float vy = 0.0f;

        // Check RenderManager actions instead of raw keyboard input
        if (this->_coordinator.isActionActive(GameAction::MOVE_LEFT))
            vx = -0.1f;
        if (this->_coordinator.isActionActive(GameAction::MOVE_RIGHT))
            vx = 0.1f;
        if (this->_coordinator.isActionActive(GameAction::MOVE_UP))
            vy = -0.1f;
        if (this->_coordinator.isActionActive(GameAction::MOVE_DOWN))
            vy = 0.1f;

        vel.vx = vx;
        vel.vy = vy;
    }
}
