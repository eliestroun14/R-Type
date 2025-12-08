/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** System
*/

#include "System.hpp"
#include <algorithm>

void System::addEntity(Entity e)
{
    auto id = static_cast<std::size_t>(e);
    for (auto const& ent : _entities) {
        if (static_cast<std::size_t>(ent) == id)
            return;
    }

    _entities.push_back(e);

    if (!_running) {
        _running = true;
        onStartRunning();
    }
}

void System::removeEntity(Entity const& e)
{
    auto it = std::find(_entities.begin(), _entities.end(), e);

    if (it != _entities.end()) {
        *it = _entities.back();
        _entities.pop_back();
    }

    if (_entities.empty() && _running) {
        _running = false;
        onStopRunning();
    }
}