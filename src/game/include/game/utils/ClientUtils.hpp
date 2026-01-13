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
void createButton(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale,
    Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
    std::function<void ()> onClick);

/**
 * @brief Create a visual button
 * @param engine Thanks to it, the method can create necessary entities for the button
 * @param label The text inside the button and the label for debugging
 * @param textSize The size of the text inside the button
 * @param textColor The color of the texte inside the button
 * @param pos The pos of the button (sf::Vector2f)
 * @param scale The scale of the button
 */
void createText(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos,
    float rotation, float scale);

#endif /* !CLIENT_UTILS_HPP */
