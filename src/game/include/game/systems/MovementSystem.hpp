#pragma once

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

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
