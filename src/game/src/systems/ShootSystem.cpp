/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ShootSystem
*/

#include <game/systems/ShootSystem.hpp>
#include <game/coordinator/Coordinator.hpp>
#include <engine/ecs/component/Components.hpp>
#include <cmath>
#include <chrono>

void ShootSystem::onStartRunning()
{
    // System initialization if needed
}

void ShootSystem::onUpdate(float dt)
{
    // Get current time in milliseconds for fire rate calculation
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    uint32_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    auto& weapons = this->_engine.getComponents<Weapon>();
    auto& transforms = this->_engine.getComponents<Transform>();
    auto& inputs = this->_engine.getComponents<InputComponent>();

    // Process each entity with a weapon
    for (size_t e : this->_entities) {
        if (!weapons[e] || !transforms[e])
            continue;

        auto& weapon = weapons[e].value();
        auto& transform = transforms[e].value();
        
        // Check if entity has InputComponent (is a player)
        bool isPlayer = inputs[e].has_value();

        // Check if the entity wants to shoot
        bool shouldShoot = false;

        if (isPlayer) {
            // Player entity shoots when shoot action is held from InputComponent
            auto& input = inputs[e].value();
            shouldShoot = input.activeActions[GameAction::SHOOT];
            if (shouldShoot) {
                std::cout << "[ShootSystem] SHOOT detected for player " << input.playerId << std::endl;
            }
        } else {
            // Non-player entities (enemies) decide to shoot based on AI
            // For now, we'll use a simple pattern: shoot periodically
            // TODO: Integrate with AISystem for smarter shooting patterns
            shouldShoot = (currentTime % 2000) < 100;  // Shoot for 100ms every 2 seconds
        }

        // Verify fire rate and spawn projectile
        if (shouldShoot && canShoot(weapon, currentTime)) {
            // IMPORTANT: Only queue weapon fires on the SERVER
            // Clients send INPUT packets, server processes them and broadcasts WEAPON_FIRE
            if (!_isServer) {
                // Client: The input is already being sent via INPUT packets in buildClientPacketBasedOnStatus
                // Just update the lastShotTime to prevent spamming the input
                weapon.lastShotTime = currentTime;
                if (isPlayer) {
                    std::cout << "[ShootSystem] CLIENT: Shooting detected, waiting for server response..." << std::endl;
                }
                continue;
            }
            
            // SERVER ONLY: Queue the weapon fire event
            auto [dirX, dirY] = calculateDirection(Entity::fromId(e));
            weapon.lastShotTime = currentTime;
            
            // Get the NetworkId component to use as shooter ID
            auto& networkIdOpt = this->_engine.getComponentEntity<NetworkId>(Entity::fromId(e));
            uint32_t shooterId = networkIdOpt.has_value() ? networkIdOpt->id : static_cast<uint32_t>(e);
            
            // Determine weapon type (for now use basic)
            uint8_t weaponType = static_cast<uint8_t>(protocol::WeaponTypes::WEAPON_TYPE_BASIC);
            
            // Queue the weapon fire event to be processed by Coordinator
            _coordinator.queueWeaponFire(shooterId, transform.x, transform.y, dirX, dirY, weaponType);
            
            if (isPlayer) {
                std::cout << "[ShootSystem] Weapon fire queued from player " << shooterId 
                          << " at (" << transform.x << ", " << transform.y << ")" << std::endl;
            }
        }
    }
}

void ShootSystem::spawnProjectile(Entity shooterId, float originX, float originY,
                                  float directionX, float directionY,
                                  bool isFromPlayable, const Weapon& weapon)
{
    // Server-side: Don't spawn projectile entities here - the Coordinator will handle this
    // based on weapon fire packets. This method now just serves to trigger the fire event.
    // The actual projectile spawning happens in Coordinator::handlePacketWeaponFire or
    // Coordinator::spawnProjectile when processing the weapon fire packet.
    
    // Note: Client-side sound playing is handled in Coordinator when receiving weapon fire packet
}

bool ShootSystem::canShoot(const Weapon& weapon, uint32_t currentTime) const
{
    return (currentTime - weapon.lastShotTime) >= weapon.fireRateMs;
}

std::pair<float, float> ShootSystem::calculateDirection(Entity shooterId)
{
    auto& transforms = this->_engine.getComponents<Transform>();
    auto& inputs = this->_engine.getComponents<InputComponent>();

    // Default: shoot to the right
    float dirX = 1.0f;
    float dirY = 0.0f;

    bool isPlayer = inputs[shooterId].has_value();

    if (isPlayer) {
        // For players: always shoot to the right
        dirX = 1.0f;
        dirY = 0.0f;
    } else {
        // For enemies: shoot towards the player (if we can find one)
        // TODO: Implement proper targeting system
        // For now, shoot left (towards player side)
        dirX = -1.0f;
        dirY = 0.0f;
    }

    return {dirX, dirY};
}

Assets ShootSystem::getProjectileAsset(ProjectileType projectileType, bool isFromPlayable)
{
    // Map to existing bullet assets to avoid missing files
    if (projectileType == ProjectileType::MISSILE) {
        return DEFAULT_BULLET; // available in assets/bullet/defaultBullet.png
    } else if (projectileType == ProjectileType::LASER) {
        return CHARCHING_BULLET; // reuse charging bullet visual
    }
    // Fallback to a safe existing asset
    return DEFAULT_BULLET;
}
