/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** CollisionSystem
*/

#ifndef COLLISIONSYSTEM_HPP_
#define COLLISIONSYSTEM_HPP_

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

struct Transform;
struct Sprite;

class CollisionSystem : public System{
    public:
        CollisionSystem(Coordinator& coordinator) : _coordinator(coordinator) {}

        void onCreate() override {}
        void onUpdate(float dt) override;

    private:
        Coordinator& _coordinator;
        
        bool checkAABBCollision(const Transform& t1, const Sprite& s1,
            const Transform& t2, const Sprite& s2);
};

#endif /* !COLLISIONSYSTEM_HPP_ */
