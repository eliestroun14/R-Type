/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** CollisionSystem
*/

#ifndef COLLISIONSYSTEM_HPP_
#define COLLISIONSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>

struct Transform;
struct Sprite;
struct Team;

/**
 * @class CollisionSystem
 * @brief Handles collision detection and team-based damage resolution.
 *
 * This system:
 * - Detects AABB collisions between entities with HitBox and Sprite
 * - Applies damage based on Team components
 * - Destroys projectiles on impact
 * - Respects team collision rules (e.g., player bullets don't hit players)
 * - Prevents obstacles from taking damage
 *
 * Team Rules:
 * - Player projectiles only damage ENEMY or BOSS
 * - Enemy projectiles only damage PLAYER
 * - OBSTACLE entities never take damage
 * - Non-projectile collisions are processed based on team compatibility
 */
class CollisionSystem : public System{
    public:
        CollisionSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}
        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;

        bool checkAABBCollision(const Sprite& s1, const Sprite& s2);
        void updateGlobalBounds(Sprite& sprite, const Transform& transform);
};

#endif /* !COLLISIONSYSTEM_HPP_ */
