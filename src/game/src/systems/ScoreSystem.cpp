/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ScoreSystem
*/

#include <game/systems/ScoreSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/ecs/entity/Entity.hpp>

struct Score {
    uint32_t value = 0;
};

void ScoreSystem::onStartRunning()
{
    for (Entity e : _engine.getEntitiesWith<Score>()) {
        _engine.getComponentEntity<Score>(e).value().value = 0;
    }
}

void ScoreSystem::onUpdate(float)
{
    auto& events = _engine.scoreEvents();
    if (events.empty())
        return;

    for (const auto& ev : events) {
        if (!_engine.isAlive(ev.scorer))
            continue;

        auto& optScore = _engine.getComponentEntity<Score>(ev.scorer);
        if (!optScore.has_value())
            continue;

        Score& score = optScore.value();
        score.value += ev.amount;
    }

    events.clear();
}


