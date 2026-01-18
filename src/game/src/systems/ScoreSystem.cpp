// ScoreSystem.cpp

#include <game/systems/ScoreSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <cstdio>
#include <common/logger/Logger.hpp>

void ScoreSystem::onUpdate(float)
{
    auto& scores = _engine.getComponents<Score>();
    auto& texts  = _engine.getComponents<Text>();
    auto& events = _engine.scoreEvents();

    const size_t hud = static_cast<size_t>(_hud);

    LOG_DEBUG_CAT("ScoreSystem",
        "tick: hud={} events={} scoresSize={} textsSize={}",
        hud, events.size(), scores.size(), texts.size());

    if (hud >= scores.size() || !scores[hud]) {
        LOG_WARN_CAT("ScoreSystem", "HUD score component missing: hud={} hasScore={}", hud,
            (hud < scores.size() && scores[hud].has_value()));
        events.clear();
        return;
    }

    uint32_t before = scores[hud].value().score;
    uint32_t added = 0;

    for (const auto& ev : events) {
        added += ev.amount;
        LOG_DEBUG_CAT("ScoreSystem", "apply event: amount={} (runningAdded={})", ev.amount, added);
    }

    scores[hud].value().score = before + added;

    LOG_INFO_CAT("ScoreSystem", "score updated: {} -> {}", before, scores[hud].value().score);

    if (hud < texts.size() && texts[hud]) {
        std::snprintf(texts[hud].value().str, sizeof(texts[hud].value().str),
                      "%u", scores[hud].value().score);
        LOG_DEBUG_CAT("ScoreSystem", "text synced: '%s'", texts[hud].value().str);
    } else {
        LOG_WARN_CAT("ScoreSystem", "HUD text missing: hud={} hasText={}", hud,
            (hud < texts.size() && texts[hud].has_value()));
    }

    events.clear();
}
