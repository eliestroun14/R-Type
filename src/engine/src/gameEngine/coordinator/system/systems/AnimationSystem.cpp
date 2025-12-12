/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AnimationSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/AnimationSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>

void AnimationSystem::onUpdate(float dt)
{
    auto& animations = this->_coordinator.getComponents<Animation>();
    auto& sprites = this->_coordinator.getComponents<Sprite>();

    for (size_t e : this->_entities) {
        if (!animations[e] || !sprites[e])
            continue;

        auto& anim = animations[e].value();
        auto& sprite = sprites[e].value();

        anim.elapsedTime += dt;

        if (anim.elapsedTime > anim.frameDuration) {
            anim.currentFrame++;
            anim.elapsedTime = 0;
        }

        if (anim.currentFrame > anim.endFrame) {
            if (anim.loop == true)
                anim.currentFrame = anim.startFrame;
            else
                anim.currentFrame = anim.endFrame;
        }

        int frameWidth = sprite.rect.width;
        int frameHeight = sprite.rect.height;

        int newLeft = anim.currentFrame * frameWidth;
        sprite.rect.left = newLeft;
    }
}