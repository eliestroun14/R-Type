/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderSystem
*/

#include <game/systems/RenderSystem.hpp>
#include <common/constants/render/Assets.hpp>
#include <engine/ecs/component/Components.hpp>
#include <algorithm>
#include <common/constants/defines.hpp>

void RenderSystem::onUpdate(float)
{
    sf::RenderWindow& window = this->_engine.getWindow();

    auto& transforms  = this->_engine.getComponents<Transform>();
    auto& sprites     = this->_engine.getComponents<Sprite>();
    auto& texts       = this->_engine.getComponents<Text>();
    auto& backgrounds = this->_engine.getComponents<ScrollingBackground>();

    // scale factor
    sf::Vector2u windowSize = window.getSize();
    float scaleX = windowSize.x / WINDOW_WIDTH;
    float scaleY = windowSize.y / WINDOW_HEIGHT;
    float scale = std::min(scaleX, scaleY);

    // Build a render list from registries (NOT from _entities)
    this->_sortedEntities.clear();
    const size_t maxSize = std::max({transforms.size(), sprites.size(), texts.size()});
    this->_sortedEntities.reserve(maxSize);

    for (size_t e = 0; e < maxSize; ++e) {
        const bool hasSprite = (e < sprites.size() && sprites[e].has_value());
        const bool hasText   = (e < texts.size()   && texts[e].has_value());
        const bool hasTrans  = (e < transforms.size() && transforms[e].has_value());

        if (!hasTrans)
            continue;
        if (!hasSprite && !hasText)
            continue;

        this->_sortedEntities.push_back(e);
    }

    // sort by z-index
    std::sort(this->_sortedEntities.begin(), this->_sortedEntities.end(),
        [&sprites, &texts](size_t a, size_t b) {
            int za = (a < sprites.size() && sprites[a]) ? (int)sprites[a]->zIndex
                   : (a < texts.size()  && texts[a])   ? (int)texts[a]->zIndex
                   : 0;

            int zb = (b < sprites.size() && sprites[b]) ? (int)sprites[b]->zIndex
                   : (b < texts.size()  && texts[b])   ? (int)texts[b]->zIndex
                   : 0;

            return za < zb;
        }
    );

    // draw
    for (size_t entity : this->_sortedEntities) {
        auto& trans = transforms[entity].value();

        // SPRITE
        if (entity < sprites.size() && sprites[entity]) {
            auto& sprite = sprites[entity].value();
            std::shared_ptr<sf::Texture> texture = this->_engine.getTexture(sprite.assetId);

            if (texture) {
                bool repeat = (entity < backgrounds.size() && backgrounds[entity].has_value() && backgrounds[entity]->repeat);

                sf::Sprite sfSprite;
                sfSprite.setTexture(*texture);
                sfSprite.setPosition(trans.x, trans.y);
                sfSprite.setScale(trans.scale * scale, trans.scale * scale);
                sfSprite.setRotation(trans.rotation);

                if (repeat) {
                    texture->setRepeated(true);
                    sf::IntRect extendedRect(0, 0, sprite.rect.width * 3, sprite.rect.height);
                    sfSprite.setTextureRect(extendedRect);
                } else {
                    if (sprite.rect.width > 0)
                        sfSprite.setTextureRect(sprite.rect);
                }

                sprite.globalBounds = sfSprite.getGlobalBounds();
                window.draw(sfSprite);
            }
        }

        // TEXT
        if (entity < texts.size() && texts[entity]) {
            auto& text = texts[entity].value();
            std::shared_ptr<sf::Font> font = this->_engine.getFont(text.fontId);

            if (font) {
                sf::Text sfText;
                sfText.setFont(*font);
                sfText.setString(text.str);
                sfText.setCharacterSize(text.size);
                sfText.setFillColor(text.color);

                sf::FloatRect textBounds = sfText.getGlobalBounds();
                sfText.setOrigin(textBounds.left + textBounds.width / 2.0f,
                                 textBounds.top + textBounds.height / 2.0f);

                sfText.setPosition(trans.x, trans.y);
                sfText.setScale(trans.scale * scale, trans.scale * scale);

                window.draw(sfText);
            }
        }
    }
}
