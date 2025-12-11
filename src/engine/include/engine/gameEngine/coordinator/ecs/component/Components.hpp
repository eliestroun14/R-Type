/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Components
*/

#ifndef COMPONENTS_HPP_
#define COMPONENTS_HPP_

/**
 * @file Components.hpp
 * @brief Definition of all ECS Components used in the R-Type game.
 *
 * Components are pure data structures (POD) attached to entities.
 * They are processed by Systems to implement game logic.
 */

#include <iostream>
#include <../../../../../../common/include/common/constants/render/Assets.hpp>
#include <SFML/Graphics.hpp>
#include <utility>


// ############################################################################
// ################################## CORE  ###################################
// ############################################################################


/**
 * @brief Represents the spatial state of an entity in the world.
 *
 * Used by: PhysicsSystem, RenderSystem
 */
struct Transform
{
    float x;
    float y;
    float rotation;
    float scale;

    Transform(float xx, float yy, float rot, float sca)
        : x(xx), y(yy), rotation(rot), scale(sca) {}
};


/**
 * @brief Defines the movement vector of an entity.
 *
 * Used by: PhysicsSystem.
 */
struct Velocity
{
    float vx; ///< Velocity vector on X axis.
    float vy; ///< Velocity vector on Y axis.

    Velocity(float vxx, float vyy) : vx(vxx), vy(vyy) {}
};


// TODO: ask to the team if they think this component is a good idea

/**
 * @brief Unique identifier for network synchronization.
 *
 * This component links a local Entity (client-side) to a Server Entity ID.
 * It is essential for packet reconciliation and authoritative updates.
 */
struct NetworkId {
    uint32_t id;
    bool isLocal;

    NetworkId(uint32_t netId, bool local = false) : id(netId), isLocal(local) {}
};


// ############################################################################
// ################################ PHYSICS ###################################
// ############################################################################

/**
 * @brief Defines a rectangular collision area.
 *
 * Used by: CollisionSystem.
 */
struct HitBox
{
    float width, height;
    bool isTrigger;
    HitBox(float w, float h, bool triggered)
        : width(w), height(h), isTrigger(triggered) {}
};


// ############################################################################
// ################################ RENDER  ###################################
// ############################################################################

/**
 * @brief Visual representation of an entity using an asset ID.
 *
 * Used by: RenderSystem.
 */
struct Sprite
{
    Assets assetId;
    int zIndex; // 0 = Background, 1 = Game, 2 = UI/HUD
    sf::Rect<int> rect;
    Sprite(Assets id, int z, sf::Rect<int> r)
        : assetId(id), zIndex(z), rect(r) {}
    Sprite(Assets id, int z) : assetId(id), zIndex(z) {}
};


/**
 * @brief Handles sprite sheet animation data.
 *
 * Used by: AnimationSystem.
 */
struct Animation
{
    size_t numFrames;
    size_t frameSpeed;
    size_t currentFrame;
    uint32_t lastFrameTime;
    Animation(size_t nbFrames, size_t speed, size_t current, uint32_t lastTime)
        : numFrames(nbFrames), frameSpeed(speed), currentFrame(current), lastFrameTime(lastTime) {}
};


/**
 * @brief Wrapper for text data display.
 *
 * Used by: RenderSystem (UI).
 */
struct Text
{
    std::string data;
    Text(std::string text) : data(text) {}
};


// ############################################################################
// ################################# LOGIC  ###################################
// ############################################################################

/**
 * @brief Manages health points and maximum capacity.
 *
 * Used by: DamageSystem, UISystem.
 */
struct Health
{
    int currentHealth;
    int maxHp;
    Health(int current, int max) : currentHealth(current), maxHp(max) {}
};


/**
 * @enum PowerupType
 * @brief Types of available power-ups in the game.
 */
enum PowerupType {
    WEAPON_UPGRADE,
    SHIELD,
    SPEED_BOOST,
    HEAL
};

/**
 * @brief Represents a collectible power-up item.
 *
 * Used by: CollisionSystem (Trigger).
 */
struct Powerup
{
    PowerupType powerupType;
    float duration; // TODO: set constants to different duration for powerups
                    // (QUICK_POWERUP, LONG_POWERUP, PERMANENT_POWERUP -> names can change)
    Powerup(PowerupType type, float d) : powerupType(type), duration(d) {}
};


/**
 * @enum ProjectileType
 * @brief Defines the visual and behavioral type of a projectile.
 */
enum ProjectileType {
    MISSILE,
    LASER,
};

/**
 * @brief Defines shooting capabilities.
 *
 * Used by: WeaponSystem, InputSystem.
 */
struct Weapon
{
    uint32_t fireRateMs;
    uint32_t lastShotTime; // in milliseconds
    int damage;
    ProjectileType projectileType;
    Weapon(uint32_t fireRate, uint32_t lastShot, int damages, ProjectileType type)
        : fireRateMs(fireRate), lastShotTime(lastShot), damage(damages), projectileType(type) {}
};

// ############################################################################
// ################################### TAGS ###################################
// ############################################################################

/**
 * @brief Tag component for clickable UI elements.
 *
 * Used by: InputSystem, UISystem.
 */
struct Clickable
{
    bool isClicked;
    Clickable(bool clicked) : isClicked(clicked) {}
};

/**
 * @brief Tag component. Entities with this tag will be processed by the RenderSystem.
 *
 * Often used in conjunction with Sprite or Text.
 */
struct Drawable {};

/**
 * @brief Tag component. Marks the entity as the local player controlled character.
 *
 * Used by: InputSystem.
 */
struct Playable {};


// ############################################################################
// ################################ BEHAVIOUR #################################
// ############################################################################

/**
 * @enum MovementPatternType
 * @brief Defines algorithmic movement patterns for enemies or projectiles.
 */
enum MovementPatternType {
    LINEAR,
    SINE_WAVE,
    CIRCULAR,
    BEZIER,
    CHASE_PLAYER
};

/**
 * @brief Defines complex movement logic beyond simple velocity.
 *
 * Used by: MovementSystem (Server-side mainly).
 */
struct MovementPattern
{
    MovementPatternType patternType;
    float amplitude; // for SINE_WAVE
    float frequency;
    std::vector<std::pair<float, float>> positions; // for complex patterns
    float timeElapsed;
    MovementPattern(MovementPatternType type, float ampl, float freq,
        std::vector<std::pair<float, float>> allPos, float timeElaps)
        : patternType(type), amplitude(ampl), frequency(freq),
          positions(allPos), timeElapsed(timeElaps) {}
};


/**
 * @enum AiBehaviour
 * @brief High-level state machine states for AI.
 */
enum AiBehaviour {
    KAMIKAZE,
    SHOOTER_TACTIC,
    ZIGZAG,
    FORMATION
};

/**
 * @brief Controls the AI decision making.
 *
 * Used by: AISystem.
 */
struct AI
{
    AiBehaviour aiBehaviour;
    float detectionRange;
    float aggroRange;
    AI(AiBehaviour behaviour, float detection, float aggro)
        : aiBehaviour(behaviour), detectionRange(detection), aggroRange(aggro) {}
};

#endif /* !COMPONENTS_HPP_ */
