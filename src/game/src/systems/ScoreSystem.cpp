/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ScoreSystem
*/

#include <game/systems/ScoreSystem.hpp>
#include <engine/ecs/component/Components.hpp>

void ScoreSystem::onStartRunning()
{
}

void ScoreSystem::onUpdate(float)
{
    auto& scores = this->_engine.getComponents<Score>();
    auto& texts  = this->_engine.getComponents<Text>();
    auto& events = this->_engine.scoreEvents();

    for (const auto& ev : events) {
        if (!this->_engine.isAlive(ev.scorer))
            continue;

        size_t idx = static_cast<size_t>(ev.scorer);
        if (idx >= scores.size() || !scores[idx])
            continue;

        scores[idx].value().score += ev.amount;

        if (idx < texts.size() && texts[idx]) {
            texts[idx].value().str = std::to_string(scores[idx].value().score);
        }
    }

    events.clear();
}
