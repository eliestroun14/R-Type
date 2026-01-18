/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** DestroySystem
*/

#ifndef DESTROYSYSTEM_HPP_
#define DESTROYSYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

class DestroySystem : public System {
public:
    DestroySystem(gameEngine::GameEngine& engine)
        : _engine(engine)
    {}

    void onCreate() override {}

    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};

#endif /* !DESTROYSYSTEM_HPP_ */
