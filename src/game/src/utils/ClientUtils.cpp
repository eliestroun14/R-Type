/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientUtils
*/

#include <game/utils/ClientUtils.hpp>
#include <random>
#include <iostream>
#include <chrono>

uint32_t generateClientId()
{
    std::random_device rd;  // Random seed generator
    std::mt19937 gen(rd());  // Mersenne Twister random engine
    std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);  // Uniform distribution

    return dis(gen);
}

uint32_t getCurrentTimeMs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return milliseconds;
}

// void createButton(gameEngine::GameEngine& engine, std::string label,
//     unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale, sf::IntRect rectSprite,
//     Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
//     std::function<void ()> onClick)
// {
//     Entity button = engine.createEntity("Button_" + label);
//     engine.addComponent<Transform>(button, Transform(pos.x, pos.y, 0, scale));

//     engine.addComponent<Sprite>(button, Sprite(noneAssetId, ZIndex::IS_UI_HUD, rectSprite));

//     ButtonTextures textures = { noneAssetId, hoverAssetId, clickedAssetId};
//     engine.addComponent<ButtonComponent>(button, ButtonComponent(textures, onClick));

//     Entity textEntity = engine.createEntity("ButtonText_" + label);
//     engine.addComponent<Transform>(textEntity, Transform(pos.x, pos.y, 0, scale));
//     engine.addComponent<Text>(textEntity, Text(label, textColor, textSize, ZIndex::IS_UI_HUD));
// }

std::vector<Entity> createButton(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale, sf::IntRect rectSprite,
    Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
    std::function<void ()> onClick)
{
    std::vector<Entity> createdEntities;

    // button
    Entity button = engine.createEntity("Button_" + label);
    engine.addComponent<Transform>(button, Transform(pos.x, pos.y, 0, scale));
    engine.addComponent<Sprite>(button, Sprite(noneAssetId, ZIndex::IS_UI_HUD, rectSprite));
    ButtonTextures textures = { noneAssetId, hoverAssetId, clickedAssetId};
    engine.addComponent<ButtonComponent>(button, ButtonComponent(textures, onClick));

    createdEntities.push_back(button);

    // text
    float centerX = pos.x + (rectSprite.width * scale) / 2.0f;
    float centerY = pos.y + (rectSprite.height * scale) / 2.0f;

    Entity textEntity = engine.createEntity("ButtonText_" + label);
    engine.addComponent<Transform>(textEntity, Transform(centerX, centerY, 0, scale));
    engine.addComponent<Text>(textEntity, Text(label.c_str(), textColor, textSize, ZIndex::IS_UI_HUD));

    createdEntities.push_back(textEntity);

    return createdEntities;
}


Entity createText(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos,
    float rotation, float scale)
{
    Entity text = engine.createEntity("Text_" + label);
    engine.addComponent<Transform>(text, Transform(pos.x, pos.y, rotation, scale));

    engine.addComponent<Text>(text, Text(label.c_str(), textColor, textSize, ZIndex::IS_UI_HUD));

    return text;
}


Entity createImage(gameEngine::GameEngine& engine, Assets assetId,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rect, ZIndex zIndex)
{
    Entity image = engine.createEntity("Entity");

    engine.addComponent<Transform>(image, Transform(pos.x, pos.y, rotation, scale));
    engine.addComponent<Sprite>(image, Sprite(assetId, zIndex, rect));

    return image;
}


Entity createAnimatedImage(gameEngine::GameEngine& engine, Assets assetId, Animation animation,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rect, ZIndex zIndex)
{
    Entity image = engine.createEntity("Entity");

    engine.addComponent<Transform>(image, Transform(pos.x, pos.y, rotation, scale));
    engine.addComponent<Sprite>(image, Sprite(assetId, zIndex, rect));
    engine.addComponent<Animation>(image, Animation(animation));

    return image;
}
