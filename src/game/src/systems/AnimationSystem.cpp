/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AnimationSystem
*/

#include <game/systems/AnimationSystem.hpp>
#include <engine/ecs/component/Components.hpp>

void AnimationSystem::onUpdate(float dt)
{
    auto& animations = this->_engine.getComponents<Animation>();
    auto& sprites = this->_engine.getComponents<Sprite>();

    static bool firstRun = true;
    if (firstRun && this->_entities.size() > 0) {
        LOG_INFO_CAT("AnimationSystem", "Processing {} entities", this->_entities.size());
        firstRun = false;
    }

    for (size_t e : this->_entities) {
        if (!animations[e] || !sprites[e])
            continue;

        auto& anim = animations[e].value();
        auto& sprite = sprites[e].value();

        anim.elapsedTime += dt;

        if (anim.elapsedTime > anim.frameDuration) {
            anim.currentFrame++;
            anim.elapsedTime = 0;
            // LOG_DEBUG_CAT("AnimationSystem", "Entity {} frame advanced to {}", e, anim.currentFrame);
        }

        if (anim.currentFrame > anim.endFrame) {
            if (anim.loop == true)
                anim.currentFrame = anim.startFrame;
            else
                anim.currentFrame = anim.endFrame;
        }

        int frameWidth = anim.frameWidth;
        int frameHeight = anim.frameHeight;

        int newLeft = anim.currentFrame * frameWidth;
        sprite.rect.left = newLeft;
        sprite.rect.width = frameWidth;
        sprite.rect.height = frameHeight;
        
        // LOG_DEBUG_CAT("AnimationSystem", "Entity {} sprite rect=({},{},{}x{}) frame={}/{}",
        //     e, sprite.rect.left, sprite.rect.top, sprite.rect.width, sprite.rect.height,
        //     anim.currentFrame, anim.endFrame);
    }
}