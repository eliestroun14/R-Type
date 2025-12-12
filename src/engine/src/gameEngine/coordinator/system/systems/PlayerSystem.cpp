/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PlayerSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/PlayerSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>

void PlayerSystem::onUpdate(float dt)
{
    auto& playables = this->_coordinator.getComponents<Playable>();
    auto& velocities = this->_coordinator.getComponents<Velocity>();

    for (size_t e : this->_entities) {
        if (!playables[e] || velocities[e])
            continue;

        auto& vel = velocities[e].value();

        float vx;
        float vy;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            vx = -0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            vx = 0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            vy = -0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            vy = 0.1;

        vel.vx = vx;
        vel.vy = vy;
    }
}
