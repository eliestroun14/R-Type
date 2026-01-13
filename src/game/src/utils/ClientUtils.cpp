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

void createButton(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale, sf::IntRect rectSprite,
    Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
    std::function<void ()> onClick)
{
    Entity button = engine.createEntity("Button_" + label);
    engine.addComponent<Transform>(button, Transform(pos.x, pos.y, 0, scale));

    engine.addComponent<Sprite>(button, Sprite(noneAssetId, ZIndex::IS_UI_HUD, rectSprite));

    ButtonTextures textures = { noneAssetId, hoverAssetId, clickedAssetId};
    engine.addComponent<ButtonComponent>(button, ButtonComponent(textures, onClick));

    Entity textEntity = engine.createEntity("ButtonText_" + label);
    engine.addComponent<Transform>(textEntity, Transform(pos.x, pos.y, 0, scale));
    engine.addComponent<Text>(textEntity, Text(label, FontAssets::DEFAULT_FONT, textColor, textSize, ZIndex::IS_UI_HUD));
}


void createText(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos,
    float rotation, float scale)
{
    Entity text = engine.createEntity("Text_" + label);
    engine.addComponent<Transform>(text, Transform(pos.x, pos.y, rotation, scale));

    engine.addComponent<Text>(text, Text(label, FontAssets::DEFAULT_FONT, textColor, textSize, ZIndex::IS_UI_HUD));
}


void createImage(gameEngine::GameEngine& engine, Assets assetId,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rect, ZIndex zIndex = ZIndex::IS_UI_HUD)
{
    Entity image = engine.createEntity("Entity");

    engine.addComponent<Transform>(image, Transform(pos.x, pos.y, rotation, scale));
    engine.addComponent<Sprite>(image, Sprite(assetId, zIndex, rect));
}


void createCheckbox(gameEngine::GameEngine& engine, bool initialState,
    Assets uncheckedAsset, Assets checkedAsset, sf::Vector2f pos,
    sf::IntRect rectSprite, float rotation, float scale,
    std::function<void(bool)> onToggle)
{
    //FIXME: check if the method is correct
    Entity uncheckEntity = engine.createEntity("UncheckEntity");

    engine.addComponent(uncheckEntity, Transform(pos.x, pos.y, rotation, scale));
    engine.addComponent(uncheckEntity, Sprite(uncheckedAsset, ZIndex::IS_UI_HUD, rectSprite));

    Entity checkEntity = engine.createEntity("CheckEntity");

    engine.addComponent(checkEntity, Transform(pos.x, pos.y, rotation, scale));
    engine.addComponent(checkEntity, Sprite(checkedAsset, ZIndex::IS_UI_HUD, rectSprite));
}
