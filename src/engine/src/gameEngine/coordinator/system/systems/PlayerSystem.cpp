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
    auto& animations = this->_coordinator.getComponents<Animation>();

    float scaleFactor = this->_coordinator.getScaleFactor();

    for (size_t e : this->_entities) {
        if (!playables[e] || !velocities[e])
            continue;

        auto& vel = velocities[e].value();

        float baseSpeed = 0.6f;
        float vx = 0.0f;
        float vy = 0.0f;

        // Check RenderManager actions for continuous movement
        if (this->_coordinator.isActionActive(GameAction::MOVE_LEFT))
            vx = -baseSpeed * scaleFactor;
        if (this->_coordinator.isActionActive(GameAction::MOVE_RIGHT))
            vx = baseSpeed * scaleFactor;
        if (this->_coordinator.isActionActive(GameAction::MOVE_UP))
            vy = -baseSpeed * scaleFactor;
        if (this->_coordinator.isActionActive(GameAction::MOVE_DOWN))
            vy = baseSpeed * scaleFactor;

        vel.vx = vx;
        vel.vy = vy;

        // Update animation based on movement direction
        if (animations[e]) {
            auto& anim = animations[e].value();

            // Determine current direction (0=down, 1=up, 2=neutral)
            int currentDirection = 2;  // default to neutral
            if (this->_coordinator.isActionActive(GameAction::MOVE_DOWN)) {
                currentDirection = 0;
            } else if (this->_coordinator.isActionActive(GameAction::MOVE_UP)) {
                currentDirection = 1;
            }

            // Only update animation if direction changed
            if (currentDirection != this->_lastDirection) {
                this->_lastDirection = currentDirection;

                int newStartFrame = 2;
                int newEndFrame = 2;

                if (currentDirection == 0) {  // DOWN
                    newStartFrame = 0;
                    newEndFrame = 1;
                } else if (currentDirection == 1) {  // UP
                    newStartFrame = 3;
                    newEndFrame = 4;
                }
                // else neutral stays 2-2

                anim.startFrame = newStartFrame;
                anim.endFrame = newEndFrame;
                anim.currentFrame = newStartFrame;
                anim.elapsedTime = 0;
            }

            anim.loop = false;
        }
    }
}
