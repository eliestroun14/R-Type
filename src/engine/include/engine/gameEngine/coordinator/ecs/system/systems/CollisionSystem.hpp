/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** CollisionSystem
*/

#ifndef COLLISIONSYSTEM_HPP_
#define COLLISIONSYSTEM_HPP_

#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

struct Transform;
struct Sprite;

class CollisionSystem : public System{
    public:
        CollisionSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}
        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
        
        bool checkAABBCollision(const Sprite& s1, const Sprite& s2);
};

#endif /* !COLLISIONSYSTEM_HPP_ */
