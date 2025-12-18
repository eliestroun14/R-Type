/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/RenderSystem.hpp>
#include <common/constants/render/Assets.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <algorithm>

void RenderSystem::onUpdate(float dt)
{
    this->_sortedEntities.clear();

    for (const auto& entity : this->_entities) {
        this->_sortedEntities.push_back(entity);
    }

    auto& sprites = this->_engine.getComponents<Sprite>();

    // we need to sort here cuz Z = 0 (background) and Z = 1 (player) and Z = 2 (HUD/UI), to display in the right order
    std::sort(this->_sortedEntities.begin(), this->_sortedEntities.end(),
        [&sprites](size_t a, size_t b) {
            // little check au cas ou a sprite doesn't have a zIndex (no reason to append but au cas ou)
            if (!sprites[a] || !sprites[b])
                return a < b;
            return sprites[a]->zIndex < sprites[b]->zIndex;
        }
    );

    sf::RenderWindow& window = this->_engine.getWindow();
    auto& transforms = this->_engine.getComponents<Transform>();

    // this part is to render propely in function of the window size
    sf::Vector2u windowSize = window.getSize();
    float scaleX = windowSize.x / REFERENCE_RESOLUTION.x;
    float scaleY = windowSize.y / REFERENCE_RESOLUTION.y;

    float scale = std::min(scaleX, scaleY);

    auto& backgrounds = this->_engine.getComponents<ScrollingBackground>();

    for (const auto& entity : this->_sortedEntities) {
        auto& trans = transforms[entity].value();
        auto& sprite = sprites[entity].value();

        std::shared_ptr<sf::Texture> texture = this->_engine.getTexture(sprite.assetId);

        if (texture) {

            bool isScrollingBg = backgrounds[entity].has_value();

            if (isScrollingBg && backgrounds[entity]->repeat) {

                texture->setRepeated(true);

                sf::Sprite sfSprite;
                sfSprite.setTexture(*texture);

                sfSprite.setPosition(trans.x, trans.y);
                sfSprite.setScale(trans.scale * scale, trans.scale * scale);
                sfSprite.setRotation(trans.rotation);

                // get how many times we need to draw the background to cover all the screen
                int textureWidth = sprite.rect.width * trans.scale * scale;

                sf::IntRect extendedRect(0,0,
                    sprite.rect.width * 3, // *3 is to be sure we cover all the screen
                    sprite.rect.height);

                sfSprite.setTextureRect(extendedRect);

                sprite.globalBounds = sfSprite.getGlobalBounds();
                window.draw(sfSprite);

            } else {
                sf::Sprite sfSprite;
                sfSprite.setTexture(*texture);

                sfSprite.setPosition(trans.x, trans.y);
                sfSprite.setScale(trans.scale * scale, trans.scale * scale);
                sfSprite.setRotation(trans.rotation);

                if (sprite.rect.width > 0)
                    sfSprite.setTextureRect(sprite.rect);

                sprite.globalBounds = sfSprite.getGlobalBounds();

                window.draw(sfSprite);
            }
        }

    }
}
