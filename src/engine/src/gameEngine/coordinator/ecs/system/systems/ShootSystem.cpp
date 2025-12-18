/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ShootSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/ShootSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
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
            auto [dirX, dirY] = calculateDirection(Entity::fromId(e));
            weapon.lastShotTime = currentTime;
            spawnProjectile(Entity::fromId(e), transform.x, transform.y, dirX, dirY, isPlayer, weapon);
            if (isPlayer) {
                std::cout << "[ShootSystem] Projectile spawned from player at ("
                          << transform.x << ", " << transform.y << ")" << std::endl;
            }
        }
    }
}

void ShootSystem::spawnProjectile(Entity shooterId, float originX, float originY,
                                  float directionX, float directionY,
                                  bool isFromPlayable, const Weapon& weapon)
{
    // Create projectile entity
    std::string projectileName = isFromPlayable ? "PlayerProjectile" : "EnemyProjectile";
    Entity projectile = this->_engine.createEntity(projectileName);

    // Calculate projectile velocity (units per millisecond)
    float projectileSpeed = 1.5f;  // tuned for visible travel with dt in ms
    float length = std::sqrt(directionX * directionX + directionY * directionY);
    if (length > 0.0f) {
        directionX /= length;
        directionY /= length;
    }

    float velocityX = directionX * projectileSpeed;
    float velocityY = directionY * projectileSpeed;

    // Get appropriate sprite for projectile
    Assets projectileAsset = getProjectileAsset(weapon.projectileType, isFromPlayable);

    // Add components to projectile
    this->_engine.addComponent<Transform>(projectile, Transform(originX, originY, 0.0f, 2.0f));
    this->_engine.addComponent<Velocity>(projectile, Velocity(velocityX, velocityY));
    this->_engine.addComponent<Sprite>(projectile, Sprite(projectileAsset, ZIndex::IS_GAME, sf::IntRect(0, 0, 16, 8)));
    this->_engine.addComponent<HitBox>(projectile, HitBox());

    // Add Projectile component to track ownership and target type
    this->_engine.addComponent<Projectile>(projectile, Projectile(shooterId, isFromPlayable, weapon.damage));
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
