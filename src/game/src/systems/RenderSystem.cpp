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

void RenderSystem::onUpdate(float dt)
{
    this->_sortedEntities.clear();

    for (const auto& entity : this->_entities) {
        this->_sortedEntities.push_back(entity);
    }

    auto& sprites = this->_engine.getComponents<Sprite>();
    auto& texts = this->_engine.getComponents<Text>();
    auto& transforms = this->_engine.getComponents<Transform>();

    // Add entities that have Transform + Text but not Sprite (pure text entities like button labels)
    for (size_t i = 0; i < texts.size(); ++i) {
        if (texts[i] && transforms[i] && !sprites[i]) {
            // Check if this entity is not already in _sortedEntities
            if (std::find(this->_sortedEntities.begin(), this->_sortedEntities.end(), i) == this->_sortedEntities.end()) {
                this->_sortedEntities.push_back(i);
            }
        }
    }

    auto& configs = this->_engine.getComponents<GameConfig>();
    for (auto& config : configs) {
        if (config.has_value())
            this->_targetFont = config->activeFont;
    }

    // we need to sort here cuz Z = 0 (background) and Z = 1 (player) and Z = 2 (HUD/UI), to display in the right order
    std::sort(this->_sortedEntities.begin(), this->_sortedEntities.end(),
        [&sprites, &texts](size_t a, size_t b) {
            int za = sprites[a] ? sprites[a]->zIndex : (texts[a] ? texts[a]->zIndex : 0);
            int zb = sprites[b] ? sprites[b]->zIndex : (texts[b] ? texts[b]->zIndex : 0);
            return za < zb;
        }
    );

    sf::RenderWindow& window = this->_engine.getWindow();

    // this part is to render propely in function of the window size
    sf::Vector2u windowSize = window.getSize();
    float scaleX = windowSize.x / WINDOW_WIDTH;
    float scaleY = windowSize.y / WINDOW_HEIGHT;

    float scale = std::min(scaleX, scaleY);

    auto& backgrounds = this->_engine.getComponents<ScrollingBackground>();

    for (const auto& entity : this->_sortedEntities) {
        auto& trans = transforms[entity].value();

        // LOGIC SPRITES
        if (sprites[entity]) {

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

        // LOGIC TEXT
        if (texts[entity]) {
            auto& text = texts[entity].value();
            std::shared_ptr<sf::Font> font = this->_engine.getFont(this->_targetFont);

            if (!font) {
                std::cerr << "[RenderSystem] ERROR: Font is nullptr for text: " << text.str << std::endl;
            } else {
                sf::Text sfText;
                sfText.setFont(*font);
                sfText.setString(text.str);
                sfText.setCharacterSize(text.size);
                sfText.setFillColor(text.color);

                sf::FloatRect textBounds = sfText.getGlobalBounds();
                
                // DEBUG: Log text info when size is 0 or text is not empty
                if (text.size == 0 && text.str[0] != '\0') {
                    std::cerr << "[RenderSystem] WARNING: Text size is 0 for text: '" << text.str << "'" << std::endl;
                }
                
                sfText.setOrigin(textBounds.left + textBounds.width / 2.0f,
                                 textBounds.top + textBounds.height / 2.0f);

                sfText.setPosition(trans.x, trans.y);
                sfText.setScale(trans.scale * scale, trans.scale * scale);

                window.draw(sfText);
            }

        }

    }
}
