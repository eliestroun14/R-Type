/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** AccessibilitySystem
*/

#include <game/systems/AccessibilitySystem.hpp>

void AccessibilitySystem::onUpdate(float dt)
{
    auto& configs = this->_engine.getComponents<GameConfig>();
    FontAssets targetFont = FontAssets::DEFAULT_FONT;

    for (auto& config : configs) {
        if (!config.has_value())
            continue;

        targetFont = config->activeFont;
        break;
    }


    auto& texts = this->_engine.getComponents<Text>();

    for (auto& text : texts) {

        if (!text.has_value())
            continue;

        text->fontId = targetFont;
    }
}
