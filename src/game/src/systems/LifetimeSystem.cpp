/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** LifetimeSystem
*/

#include <game/systems/LifetimeSystem.hpp>

void LifetimeSystem::onUpdate(float deltaTime) {

    auto& lifetimes = _engine.getComponents<Lifetime>();

    for (size_t e : _entities) {
        if (!lifetimes[e].has_value())
            continue;

        auto& lifetime = lifetimes[e].value();

        lifetime.remainingTime -= deltaTime;

        if (lifetime.remainingTime <= 0.0f) {
            this->_engine.destroyEntity(e);
        }
    }
}

