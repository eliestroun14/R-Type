/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** AccessibilitySystem
*/

#ifndef ACCESSIBILITYSYSTEM_HPP_
#define ACCESSIBILITYSYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

class AccessibilitySystem : public System {
public:
    AccessibilitySystem(gameEngine::GameEngine& engine)
        : _engine(engine)
    {}

    // virtual ~AccessibilitySystem() = default;

    void onCreate() override {}

    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};

#endif /* !ACCESSIBILITYSYSTEM_HPP_ */
