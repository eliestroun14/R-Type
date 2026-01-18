#pragma once

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

class BoundarySystem : public System {
public:
    BoundarySystem(gameEngine::GameEngine& engine)
        : _engine(engine)
    {}

    void onCreate() override {}

    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};
