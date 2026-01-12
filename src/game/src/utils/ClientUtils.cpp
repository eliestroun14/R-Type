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
#include <engine/GameEngine.hpp>

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
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale,
    Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
    std::function<void ()> onClick)
{
    Entity button = engine.createEntity("Button_" + label);
    engine.addComponent<Transform>(button, Transform(pos.x, pos.y, 0, scale));

    engine.addComponent<Sprite>(button, Sprite(noneAssetId, ZIndex::IS_UI_HUD));

    ButtonTextures textures = { noneAssetId, hoverAssetId, clickedAssetId};
    engine.addComponent<ButtonComponent>(button, ButtonComponent(textures, onClick));

    Entity textEntity = engine.createEntity("ButtonText_" + label);
    engine.addComponent<Transform>(textEntity, Transform(pos.x, pos.y, 0, scale));
    engine.addComponent<Text>(textEntity, Text(label, FontAssets::DEFAULT_FONT, textColor, textSize, ZIndex::IS_UI_HUD));
}

