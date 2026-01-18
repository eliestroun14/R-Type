// ScoreSystem.cpp

#include <game/systems/ScoreSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <cstdio>

void ScoreSystem::onUpdate(float)
{
    auto& scores = _engine.getComponents<Score>();
    auto& texts  = _engine.getComponents<Text>();
    auto& events = _engine.scoreEvents();

    const size_t hud = static_cast<size_t>(_hud);

    if (hud >= scores.size() || !scores[hud]) {
        events.clear();
        return;
    }

    auto& s = scores[hud].value().score; // uint32_t

    for (const auto& ev : events) {
        const int32_t delta = ev.amount;

        if (delta >= 0) {
            s += static_cast<uint32_t>(delta);
        } else {
            const uint32_t dec = static_cast<uint32_t>(-delta);
            s = (s > dec) ? (s - dec) : 0u;
        }
    }

    if (hud < texts.size() && texts[hud]) {
        std::snprintf(texts[hud].value().str, sizeof(texts[hud].value().str),
                      "%u", s);
    }

    events.clear();
}

