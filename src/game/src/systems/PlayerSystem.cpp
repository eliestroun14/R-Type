/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerSystem
*/

#include <game/systems/PlayerSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/constants/defines.hpp>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

void PlayerSystem::onUpdate(float dt)
{
    try {
        auto& velocities = this->_engine.getComponents<Velocity>();
        auto& animations = this->_engine.getComponents<Animation>();
        auto& inputs = this->_engine.getComponents<InputComponent>();

        for (size_t e : this->_entities) {
            if (!velocities[e].has_value() || !inputs[e].has_value()) {
                continue;
            }

            auto& vel = velocities[e].value();
            auto& input = inputs[e].value();

            // Calculate velocity based on input state
            float vx = 0.0f;
            float vy = 0.0f;

            // Check player's InputComponent actions for continuous movement
            if (input.activeActions[GameAction::MOVE_LEFT])
                vx = -PLAYER_BASE_SPEED;
            if (input.activeActions[GameAction::MOVE_RIGHT])
                vx = PLAYER_BASE_SPEED;
            if (input.activeActions[GameAction::MOVE_UP])
                vy = -PLAYER_BASE_SPEED;
            if (input.activeActions[GameAction::MOVE_DOWN])
                vy = PLAYER_BASE_SPEED;

            // Update velocity components
            vel.vx = vx;
            vel.vy = vy;

            // Update animation based on movement direction
            if (animations[e].has_value()) {
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
    } catch (const Error& e) {
        LOG_ERROR_CAT("PlayerSystem", "Error in PlayerSystem::onUpdate: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR_CAT("PlayerSystem", "Unexpected error in PlayerSystem::onUpdate: {}", e.what());
        throw Error(ErrorType::GameplayError, "PlayerSystem update failed: " + std::string(e.what()));
    }
}
