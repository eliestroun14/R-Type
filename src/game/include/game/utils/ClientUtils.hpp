/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientUtils
*/

#ifndef CLIENT_UTILS_HPP
#define CLIENT_UTILS_HPP

#include <cstdint>
#include <engine/GameEngine.hpp>

/**
 * @brief Get the current time in milliseconds since epoch
 * @return uint32_t The current time in milliseconds
 */
uint32_t getCurrentTimeMs();

/**
 * @brief Generate a unique client ID using random number generation
 * @return uint32_t A randomly generated client ID
 */
uint32_t generateClientId();

/**
 * @brief Create a visual button
 * @param engine Thanks to it, the method can create necessary entities for the button
 * @param label The text inside the button and the label for debugging
 * @param textSize The size of the text inside the button
 * @param textColor The color of the texte inside the button
 * @param pos The pos of the button (sf::Vector2f)
 * @param scale The scale of the button
 * @param noneAssetId,hoverAssetId,clickedAssetId The AssetId for the differents states of the button (none, hover and clicked)
 * @param onClick function link to the button
 */
// void createButton(gameEngine::GameEngine& engine, std::string label,
//     unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale, sf::IntRect rectSprite,
//     Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
//     std::function<void ()> onClick);

/**
 * @brief Create a visual button
 * @param engine Thanks to it, the method can create necessary entities for the button
 * @param label The text inside the button and the label for debugging
 * @param textSize The size of the text inside the button
 * @param textColor The color of the texte inside the button
 * @param pos The pos of the button (sf::Vector2f)
 * @param scale The scale of the button
 * @param noneAssetId,hoverAssetId,clickedAssetId The AssetId for the differents states of the button (none, hover and clicked)
 * @param onClick function link to the button
 * @return A vector of entity created, the sprite button and the text
 */
std::vector<Entity> createButton(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale, sf::IntRect rectSprite,
    Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
    std::function<void ()> onClick);

/**
 * @brief Create a visual button
 * @param engine Thanks to it, the method can create necessary entities for the button
 * @param label The text inside the button and the label for debugging
 * @param textSize The size of the text inside the button
 * @param textColor The color of the texte inside the button
 * @param pos The pos of the button (sf::Vector2f)
 * @param rotation The rotation of the Text
 * @param scale The scale of the button
 * @return Entity of the text
 */
Entity createText(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos,
    float rotation, float scale);


/**
 * @brief Create a simple static image
 * @param engine The GameEngine instance
 * @param assetId The texture to use
 * @param pos Position of the image
 * @param float The rotation of the Text
 * @param scale Scale factor
 * @param zIndex Layer (default to 1 or 2 for UI)
 * @return Entity of the image'sprite
 */
Entity createImage(gameEngine::GameEngine& engine, Assets assetId,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rectSprite, ZIndex zIndex);


/**
 * @brief Create a simple static image
 * @param engine The GameEngine instance
 * @param assetId The texture to use
 * @param animation The Animation component used to get spriteSheet data
 * @param pos Position of the image
 * @param float The rotation of the Text
 * @param scale Scale factor
 * @param zIndex Layer (default to 1 or 2 for UI)
 * @return Entity of the image'sprite
 */
Entity createAnimatedImage(gameEngine::GameEngine& engine, Assets assetId, Animation animation,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rectSprite, ZIndex zIndex);

#endif /* !CLIENT_UTILS_HPP */
