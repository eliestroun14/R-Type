/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** LevelTimerSystem
*/

#include <game/systems/LevelTimerSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <sstream>
#include <iomanip>
#include <cstring>

void LevelTimerSystem::onUpdate(float dt)
{
    // Get all Level entities to find elapsed time and duration
    auto& levels = this->_engine.getComponents<Level>();
    auto& texts = this->_engine.getComponents<Text>();

    // Find the active level
    Level* activeLevel = nullptr;

    for (size_t i = 0; i < levels.size(); i++) {
        if (levels[i] && levels[i]->started && !levels[i]->completed) {
            activeLevel = &levels[i].value();
            break;
        }
    }

    // If no active level, skip timer update
    if (!activeLevel) {
        return;
    }

    // Calculate remaining time
    float remainingTime = activeLevel->levelDuration - activeLevel->elapsedTime;
    if (remainingTime < 0.0f) {
        remainingTime = 0.0f;
    }
    
    LOG_DEBUG_CAT("LevelTimerSystem", "Active level found: duration={}, elapsed={}, remaining={}",
        activeLevel->levelDuration, activeLevel->elapsedTime, remainingTime);

    // Update all timer UI entities
    for (size_t entity : this->_entities) {
        if (!texts[entity]) {
            continue;
        }

        auto &text = texts[entity].value();

        // Format time as MM:SS
        int minutes = static_cast<int>(remainingTime) / 60;
        int seconds = static_cast<int>(remainingTime) % 60;

        std::ostringstream oss;
        oss << "Time: " << std::setfill('0') << std::setw(2) << minutes
            << ":" << std::setfill('0') << std::setw(2) << seconds;

        std::string timeStr = oss.str();
        std::strncpy(text.str, timeStr.c_str(), sizeof(text.str) - 1);
        text.str[sizeof(text.str) - 1] = '\0';

        // Change color to red when less than 10 seconds remaining
        if (remainingTime < 10.0f) {
            text.color = sf::Color::Red;
        } else if (remainingTime < 30.0f) {
            text.color = sf::Color(255, 165, 0); // Orange
        } else {
            text.color = sf::Color::Yellow;
        }
    }
}
