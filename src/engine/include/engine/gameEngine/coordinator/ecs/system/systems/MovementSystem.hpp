#pragma once

#include <engine/gameEngine/coordinator/ecs/system/System.hpp>
#include <engine/gameEngine/coordinator/ecs/entity/EntityManager.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/Coordinator.hpp>

class MovementSystem : public System {
public:
    MovementSystem(Coordinator& coord)
        : _coord(coord)
    {}

    void onCreate() override {}

    void onUpdate(float dt) override;

private:
    Coordinator& _coord;
};
