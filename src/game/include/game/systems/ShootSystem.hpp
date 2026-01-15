/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ShootSystem
*/

#ifndef SHOOTSYSTEM_HPP_
#define SHOOTSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/constants/render/Assets.hpp>

// Forward declaration
class Coordinator;

/**
 * @class ShootSystem
 * @brief Handles shooting logic for entities with weapons.
 *
 * Responsibilities:
 *  - Processes fire requests from playable and non-playable entities
 *  - Spawns projectiles with proper direction
 *  - Enforces fire rate limits
 *  - Ensures playable entities only hit non-playable entities and vice versa
 *  - Manages projectile lifespan and cleanup
 */
class ShootSystem : public System {
public:
    /**
     * @brief Constructor.
     * @param engine Reference to the GameEngine.
     * @param coordinator Reference to the Coordinator for queuing weapon fire events.
     * @param isServer Whether this system is running on the server (true) or client (false).
     */
    ShootSystem(gameEngine::GameEngine& engine, Coordinator& coordinator, bool isServer = true) 
        : _engine(engine), _coordinator(coordinator), _isServer(isServer) {}

    /**
     * @brief Called when the system starts running.
     */
    void onStartRunning() override;

    /**
     * @brief Called every update cycle.
     * @param dt Delta time in seconds.
     */
    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
    Coordinator& _coordinator;
    bool _isServer;

    /**
     * @brief Spawns a projectile entity.
     * @param shooterId ID of the entity that fired.
     * @param originX Fire origin X coordinate.
     * @param originY Fire origin Y coordinate.
     * @param directionX Direction vector X component.
     * @param directionY Direction vector Y component.
     * @param isFromPlayable Whether the shooter is playable.
     * @param weapon The weapon component with damage info.
     */
    void spawnProjectile(Entity shooterId, float originX, float originY,
                        float directionX, float directionY,
                        bool isFromPlayable, const Weapon& weapon);

    /**
     * @brief Checks if fire rate allows shooting.
     * @param weapon The weapon component.
     * @param currentTime Current game time in milliseconds.
     * @return True if enough time has passed since last shot.
     */
    bool canShoot(const Weapon& weapon, uint32_t currentTime) const;

    /**
     * @brief Calculates direction vector from entity position to target.
     * @param shooterId ID of the shooter.
     * @return Direction vector as (dx, dy) pair.
     */
    std::pair<float, float> calculateDirection(Entity shooterId);

    /**
     * @brief Gets the appropriate projectile asset based on weapon type.
     * @param projectileType The type of projectile.
     * @param isFromPlayable Whether it's from a playable entity.
     * @return Asset ID for the projectile sprite.
     */
    Assets getProjectileAsset(ProjectileType projectileType, bool isFromPlayable);
};

#endif /* !SHOOTSYSTEM_HPP_ */
