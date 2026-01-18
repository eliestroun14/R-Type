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

constexpr float SCREEN_WIDTH = 1920.0f;
constexpr float SCREEN_HEIGHT = 1080.0f;
constexpr float PLAYER_MARGIN = 0.0f;

void PlayerSystem::onUpdate(float dt)
{
    try {
        auto& velocities = this->_engine.getComponents<Velocity>();
        auto& animations = this->_engine.getComponents<Animation>();
        auto& inputs = this->_engine.getComponents<InputComponent>();
        auto& transforms = this->_engine.getComponents<Transform>();
        auto& sprites = this->_engine.getComponents<Sprite>();

        LOG_DEBUG_CAT("PlayerSystem", "onUpdate: processing {} entities", this->_entities.size());

        for (size_t e : this->_entities) {
            if (!inputs[e].has_value()) {
                continue;
            }

            auto& input = inputs[e].value();

            LOG_DEBUG_CAT("PlayerSystem", "onUpdate: entity={} playerId={}", e, input.playerId);

            // IMPORTANT: Velocity is ONLY updated on the SERVER!
            // On the client, velocity/position come from TRANSFORM_SNAPSHOT packets
            // The InputComponent on clients is used ONLY for animations
            if (velocities[e].has_value()) {
                auto& vel = velocities[e].value();

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

                // Update velocity components (only on server)
                vel.vx = vx;
                vel.vy = vy;
            }

            if (transforms[e].has_value()) {
                auto& transform = transforms[e].value();

                float spriteWidth = 0.0f;
                float spriteHeight = 0.0f;

                if (sprites[e].has_value()) {
                    auto& sprite = sprites[e].value();
                    spriteWidth = sprite.rect.width;
                    spriteHeight = sprite.rect.height;
                }

                float minX = PLAYER_MARGIN;
                float maxX = SCREEN_WIDTH - spriteWidth - PLAYER_MARGIN;
                float minY = PLAYER_MARGIN;
                float maxY = SCREEN_HEIGHT - spriteHeight - PLAYER_MARGIN;

                if (transform.x < minX) {
                    transform.x = minX;
                    if (velocities[e].has_value()) {
                        velocities[e].value().vx = 0.0f;
                    }
                } else if (transform.x > maxX) {
                    transform.x = maxX;
                    if (velocities[e].has_value()) {
                        velocities[e].value().vx = 0.0f;
                    }
                }

                if (transform.y < minY) {
                    transform.y = minY;
                    if (velocities[e].has_value()) {
                        velocities[e].value().vy = 0.0f;
                    }
                } else if (transform.y > maxY) {
                    transform.y = maxY;
                    if (velocities[e].has_value()) {
                        velocities[e].value().vy = 0.0f;
                    }
                }

                LOG_DEBUG_CAT("PlayerSystem", "Player position clamped: x={}, y={}", transform.x, transform.y);
            }


            // Update animation based on movement direction (on both server and client)
            if (animations[e].has_value()) {
                auto& anim = animations[e].value();

                // Determine current direction (0=down, 1=up, 2=neutral)
                int currentDirection = 2;  // default to neutral
                if (input.activeActions[GameAction::MOVE_DOWN]) {
                    currentDirection = 0;
                } else if (input.activeActions[GameAction::MOVE_UP]) {
                    currentDirection = 1;
                }

                // Update animation if direction changed
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
                    anim.loop = false;
                    LOG_DEBUG_CAT("PlayerSystem", "onUpdate: entity={} animation updated - direction={} frames[{}-{}]", e, currentDirection, newStartFrame, newEndFrame);
                }
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
