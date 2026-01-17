/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** BackgroundSystem
*/

#include <game/systems/BackgroundSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/logger/Logger.hpp>


void BackgroundSystem::onUpdate(float dt)
{
    LOG_DEBUG_CAT("BackgroundSystem","BackgroundSystem::onUpdate - dt: {} ", dt);
    auto& transforms = this->_engine.getComponents<Transform>();
    auto& sprites = this->_engine.getComponents<Sprite>();
    auto& backgrounds = this->_engine.getComponents<ScrollingBackground>();

    float scaleFactor = this->_engine.getScaleFactor();

    for (size_t e : _entities) {
        if (!transforms[e] || !sprites[e] || !backgrounds[e])
            continue;

        auto& transform = transforms[e].value();
        auto& sprite = sprites[e].value();
        auto& bg = backgrounds[e].value();

        float scrollAmout = bg.scrollSpeed * scaleFactor * dt;

        if (bg.horizontal) {
            bg.currentOffset -= scrollAmout;

            if (bg.repeat == true) {
                float bgWidth = sprite.rect.width * transform.scale * scaleFactor;

                // reset offset when a texture completely go by the screen
                if (bg.currentOffset <= -bgWidth)
                    bg.currentOffset += bgWidth;
            }

            transform.x = bg.currentOffset;

        } else {
            bg.currentOffset -= scrollAmout;

            if (bg.repeat == true) {
                float bgHeight = sprite.rect.height * transform.scale * scaleFactor;

                if (bg.currentOffset <= -bgHeight)
                    bg.currentOffset += bgHeight;
            }

            transform.y = bg.currentOffset;
        }

    }
}