/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** MovementSystem
*/

#include <game/systems/MovementSystem.hpp>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

void MovementSystem::onUpdate(float dt)
{
    try {
        auto& positions = _engine.getComponents<Transform>();
        auto& velocities = _engine.getComponents<Velocity>();

        for (size_t e : _entities) {
            if (!positions[e].has_value() || !velocities[e].has_value()) {
                continue;
            }

            auto& pos = positions[e].value();
            auto& vel = velocities[e].value();

            // Apply velocity to position
            // Velocity is in pixels/second, dt is in seconds
            // Position delta = velocity * delta_time
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    } catch (const Error& e) {
        LOG_ERROR_CAT("MovementSystem", "Error in MovementSystem::onUpdate: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR_CAT("MovementSystem", "Unexpected error in MovementSystem::onUpdate: {}", e.what());
        throw Error(ErrorType::GameplayError, "MovementSystem update failed: " + std::string(e.what()));
    }
}

