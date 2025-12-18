/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerSystem
*/

#include <game/systems/PlayerSystem.hpp>
#include <engine/ecs/component/Components.hpp>

void PlayerSystem::onUpdate(float dt)
{
    auto& velocities = this->_engine.getComponents<Velocity>();
    auto& animations = this->_engine.getComponents<Animation>();
    auto& inputs = this->_engine.getComponents<InputComponent>();

    float scaleFactor = this->_engine.getScaleFactor();

    for (size_t e : this->_entities) {
        if (!velocities[e] || !inputs[e])
            continue;

        auto& vel = velocities[e].value();
        auto& input = inputs[e].value();

        float baseSpeed = 0.6f;
        float vx = 0.0f;
        float vy = 0.0f;

        // Check player's InputComponent actions for continuous movement
        if (input.activeActions[GameAction::MOVE_LEFT])
            vx = -baseSpeed * scaleFactor;
        if (input.activeActions[GameAction::MOVE_RIGHT])
            vx = baseSpeed * scaleFactor;
        if (input.activeActions[GameAction::MOVE_UP])
            vy = -baseSpeed * scaleFactor;
        if (input.activeActions[GameAction::MOVE_DOWN])
            vy = baseSpeed * scaleFactor;

        vel.vx = vx;
        vel.vy = vy;

        // Update animation based on movement direction
        if (animations[e]) {
            auto& anim = animations[e].value();

            // Determine current direction (0=down, 1=up, 2=neutral)
            int currentDirection = 2;  // default to neutral
            if (input.activeActions[GameAction::MOVE_DOWN]) {
                currentDirection = 0;
            } else if (input.activeActions[GameAction::MOVE_UP]) {
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
