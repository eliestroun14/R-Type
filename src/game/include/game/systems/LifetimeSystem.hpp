/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** LifetimeSystem
*/

#ifndef LIFETIMESYSTEM_HPP_
#define LIFETIMESYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

class LifetimeSystem : public System {
public:
    LifetimeSystem(gameEngine::GameEngine& engine)
        : _engine(engine)
    {}

    void onCreate() override {}

    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};

#endif /* !LIFETIMESYSTEM_HPP_ */
