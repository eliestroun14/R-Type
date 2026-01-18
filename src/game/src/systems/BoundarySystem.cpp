/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** BoundarySystem
*/

#include <game/systems/BoundarySystem.hpp>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>
#include <common/constants/defines.hpp>

void BoundarySystem::onUpdate(float dt)
{
    try {
        auto& positions = _engine.getComponents<Transform>();
        auto& sprites = _engine.getComponents<Sprite>();

        // Get screen dimensions from defines
        const float SCREEN_WIDTH = static_cast<float>(WINDOW_WIDTH);
        const float SCREEN_HEIGHT = static_cast<float>(WINDOW_HEIGHT) - 80.0f;

        for (size_t e : _entities) {
            if (!positions[e].has_value()) {
                continue;
            }

            // Only apply screen boundaries to playable entities (players)
            if (!_engine.hasComponent<Playable>(Entity::fromId(e))) {
                continue;
            }

            auto& pos = positions[e].value();

            // Get sprite width and height for boundary checking
            float spriteWidth = 0.0f;
            float spriteHeight = 0.0f;
            if (sprites[e].has_value()) {
                spriteWidth = static_cast<float>(sprites[e].value().rect.width);
                spriteHeight = static_cast<float>(sprites[e].value().rect.height);
            }

            // Clamp position to screen boundaries
            // Left boundary
            if (pos.x < 0.0f) {
                pos.x = 0.0f;
            }
            // Right boundary
            if (spriteWidth > 0.0f && pos.x + spriteWidth > SCREEN_WIDTH) {
                pos.x = SCREEN_WIDTH - spriteWidth;
            }
            // Top boundary
            if (pos.y < 0.0f) {
                pos.y = 0.0f;
            }
            // Bottom boundary
            if (spriteHeight > 0.0f && pos.y + spriteHeight > SCREEN_HEIGHT) {
                pos.y = SCREEN_HEIGHT - spriteHeight;
            }
        }
    } catch (const Error& e) {
        LOG_ERROR_CAT("BoundarySystem", "Error in BoundarySystem::onUpdate: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR_CAT("BoundarySystem", "Unexpected error in BoundarySystem::onUpdate: {}", e.what());
        throw Error(ErrorType::GameplayError, "BoundarySystem update failed: " + std::string(e.what()));
    }
}
