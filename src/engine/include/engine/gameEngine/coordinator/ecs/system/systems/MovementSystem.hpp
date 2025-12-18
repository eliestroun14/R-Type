#pragma once

#include <engine/gameEngine/coordinator/ecs/system/System.hpp>
#include <engine/gameEngine/coordinator/ecs/entity/EntityManager.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/GameEngine.hpp>

class MovementSystem : public System {
public:
    MovementSystem(gameEngine::GameEngine& engine)
        : _engine(engine)
    {}

    void onCreate() override {}

    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};
