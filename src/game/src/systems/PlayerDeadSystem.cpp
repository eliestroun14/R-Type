/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerDeadSystem
*/

#include <game/systems/PlayerDeadSystem.hpp>

#define MAX_DEATH_TIME 10000

void PlayerDeadSystem::onUpdate(float dt)
{
    auto& deadPlayers = this->_engine.getComponents<DeadPlayer>();

    for (size_t e : _entities) {
        if (!deadPlayers[e])
            continue;

        auto& deathState = deadPlayers[e];

        Entity deadPlayer = this->_engine.getEntityFromId(e);

        if (deathState->initialized == false) {
            // remove components to diseable the player
            if (this->_engine.hasComponent<HitBox>(deadPlayer))
                this->_engine.removeComponent<HitBox>(deadPlayer);

            if (this->_engine.hasComponent<InputComponent>(deadPlayer))
                this->_engine.removeComponent<InputComponent>(deadPlayer);

            // change the player sprite with the big explosion sprite
            if (this->_engine.hasComponent<Sprite>(deadPlayer)) {
                auto& sprite = this->_engine.getComponentEntity<Sprite>(deadPlayer);
                sprite->assetId = BIG_EXPLOSION;
                sprite->rect.left = 0;
                sprite->rect.width = BIG_EXPLOSION_SPRITE_WIDTH;
                sprite->rect.height = BIG_EXPLOSION_SPRITE_HEIGHT;
            }

            // change the player animtion to anim correcly the explosion
            if (this->_engine.hasComponent<Animation>(deadPlayer)) {
                auto& animation = this->_engine.getComponentEntity<Animation>(deadPlayer);
                animation->frameWidth = BIG_EXPLOSION_ANIMATION_WIDTH;
                animation->frameHeight = BIG_EXPLOSION_ANIMATION_HEIGHT;
                animation->currentFrame = BIG_EXPLOSION_ANIMATION_CURRENT;
                animation->elapsedTime = BIG_EXPLOSION_ANIMATION_ELAPSED_TIME;
                animation->frameDuration = BIG_EXPLOSION_ANIMATION_DURATION;
                animation->startFrame = BIG_EXPLOSION_ANIMATION_START;
                animation->endFrame = BIG_EXPLOSION_ANIMATION_END;
                animation->loop = BIG_EXPLOSION_ANIMATION_LOOPING;
            }

            deathState->initialized = true;
        }

        deathState->timer += dt;

        if (deathState->timer >= MAX_DEATH_TIME) {
            //TODO: final destroy if player die too many times

            //TODO: respawn the player, create a method to respawn the player, reset the correct sprite and add the components removed under
        }
    }
}
