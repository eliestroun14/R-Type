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
#include <map>
#include <common/constants/render/Assets.hpp>
#include <SFML/Graphics.hpp>
#include <utility>
#include <engine/ecs/entity/Entity.hpp>
#include <vector>
#include <common/protocol/Protocol.hpp>
#include <functional>

// Forward declaration for GameAction enum
enum class GameAction;

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
};


// ############################################################################
// ################################ RENDER  ###################################
// ############################################################################

enum ZIndex {
    IS_BACKGROUND,
    IS_GAME,
    IS_UI_HUD
};

/**
 * @brief Visual representation of an entity using an asset ID.
 *
 * Used by: RenderSystem, CollisionSystem
 */
struct Sprite
{
    Assets assetId;
    ZIndex zIndex; // 0 = Background, 1 = Game, 2 = UI/HUD
    sf::Rect<int> rect;
    sf::FloatRect globalBounds; // for collisions

    Sprite(Assets id, ZIndex z, sf::Rect<int> r)
        : assetId(id), zIndex(z), rect(r), globalBounds() {}

    Sprite(Assets id, ZIndex z) : assetId(id), zIndex(z), rect(), globalBounds() {}
};


/**
 * @brief Handles sprite sheet animation data.
 *
 * Used by: AnimationSystem.
 */
struct Animation
{
    int frameWidth;   // frame width (ex: 32px)
    int frameHeight;  // frame height (ex: 32px)

    int currentFrame;    // current frame (ex: 3)
    float elapsedTime;   // elapsed time since last frame
    float frameDuration; // speed (ex: 0.1s)

    int startFrame;      // frame start (ex: 3 for UP animation for the player)
    int endFrame;        // frame end (ex: 4 for UP animation for the player)
    bool loop;
    Animation(int w, int h, int current, float elapsTime, float duration,
        int start, int end, bool isLooping)
        : frameWidth(w), frameHeight(h), currentFrame(current), elapsedTime(elapsTime),
        frameDuration(duration), startFrame(start), endFrame(end), loop(isLooping) {}
};


/**
 * @brief Wrapper for text data display.
 *
 * Used by: RenderSystem (UI).
 */
struct Text
{
    std::string str;
    FontAssets fontId;
    unsigned int size;
    sf::Color color;
    ZIndex zIndex;

    Text(std::string s, FontAssets f = DEFAULT_FONT, sf::Color c = sf::Color::White, unsigned int sz = 30, ZIndex z = ZIndex::IS_UI_HUD)
        : str(s), color(c), fontId(f), size(sz), zIndex(z) {}
};

/**
 * @brief Contain for the game background.
 *
 * Used by: BackgroundSystem (UI).
 */
struct ScrollingBackground
{
    float scrollSpeed;
    float currentOffset;
    bool horizontal;
    bool repeat;

    ScrollingBackground(float speed, bool isHorizontal, bool shouldRepeat)
        : scrollSpeed(speed), currentOffset(0.0f), horizontal(isHorizontal), repeat(shouldRepeat) {}
};

/**
 * @brief Contain for the visual effects.
 *
 * Used by: RenderSystem
 */
struct VisualEffect
{
    protocol::VisualEffectType type;
    float scale;
    float duration;
    float color_r;
    float color_g;
    float color_b;
    VisualEffect(protocol::VisualEffectType t, float s, float d, float r, float g, float b)
        : type(t), scale(s), duration(d), color_r(r), color_g(g), color_b(b) {}
};


struct Lifetime {
    float remainingTime;

    Lifetime(float time) : remainingTime(time) {}
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
    SPEED_BOOST,
    WEAPON_UPGRADE,
    FORCE,
    SHIELD,
    EXTRA_LIFE,
    INVINCIBILITY,
    HEAL,
    UNKOWN,
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
    UNKNOWN
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
 * Used to distinguish the local player from remote players.
 * The local player is the one controlled by the keyboard/input on THIS client.
 */
struct Playable {};


/**
 * @brief Tag component. Marks the entity as the local player controlled character.
 *
 * MainMenuScene entities are visible only in main menu.
 */
struct MainMenuScene {};


/**
 * @brief Tag component. Marks the entity as the local player controlled character.
 *
 * OptionMenuScene entities are visible only in option menu.
 */
struct OptionMenuScene {};


/**
 * @brief Tag component. Marks the entity as game scene entity.
 *
 * GameScene entities are visible only in game.
 */
struct GameScene {};



/**
 * @brief Stores input state for an entity (player).
 *
 * Each player entity has its own InputComponent to track active actions.
 * This allows multiple players to have different inputs simultaneously.
 * Used by: PlayerSystem, ShootSystem, InputSystem.
 */
struct InputComponent
{
    uint32_t playerId;                           ///< ID of the player this input belongs to
    std::map<GameAction, bool> activeActions;    ///< Current active actions for this player

    InputComponent(uint32_t id) : playerId(id), activeActions() {}
};

/**
 * @brief Tag component. Marks the entity as an enemy.
 *
 */
struct Enemy {};

/*
 * @brief Represents a projectile fired by an entity.
 *
 * Stores information about who shot it and what it can hit.
 * Used by: ShootSystem, CollisionSystem.
 */
struct Projectile
{
    Entity shooterId;        ///< ID of the entity that fired this projectile
    bool isFromPlayable;     ///< True if shot by a playable entity
    int damage;              ///< Damage dealt on hit

    Projectile(Entity shooter, bool fromPlayable, int dmg)
        : shooterId(shooter), isFromPlayable(fromPlayable), damage(dmg) {}
};


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
    float internalTime = 0.f;
    AI(AiBehaviour behaviour, float detection, float aggro)
        : aiBehaviour(behaviour), detectionRange(detection), aggroRange(aggro) {}
};


// ############################################################################
// ################################# LEVEL ####################################
// ############################################################################

enum EnemyType {
    BASIC,
    FAST,
    TANK,
    BOSS,
};

struct EnemySpawn
{
    EnemyType type;
    float spawnX;
    float spawnY;
    float delayAfterPrevious; // delay after the previous spawn in seconds
};

struct Wave
{
    std::vector<EnemySpawn> enemies;
    float startTime; // when this wave starts (in second from the start of the level)
};

struct Level
{
    std::vector<Wave> waves;
    // float levelDuration;
    int currentWaveIndex;
    float elapsedTime;
    bool completed;

    Level() : currentWaveIndex(0), elapsedTime(0.f), completed(false) {}
};

// ############################################################################
// ################################# PLAYER ###################################
// ############################################################################

/**
 * @brief Marks the entity player as dead.
 *
 * Used to hide the sprite and the control/interaction of the player.
 * See the PlayerDeadSystem !
 */
struct DeadPlayer {
    float timer = 0.0f;
    bool initialized = false; // some actions need to be set once, set to try when they are set
    uint32_t killerId; // optionnal if we want to show a message : "Killed by X"
};


/**
 * @brief Stock the player score (each player have its own score).
 *
 * Used to know the score of the player.
 */
struct Score {
    uint32_t score = 0;
};


// ############################################################################
// ################################# AUDIO ####################################
// ############################################################################

/**
 * @brief Contain for the audio effects.
 *
 * Used by: AudioSystem
 */
struct AudioEffect {
    protocol::AudioEffectType type;
    float volume;        // Multiplicateur de volume (0.0 - 1.0)
    float pitch;         // Modificateur de pitch (1.0 = normal)
    bool isPlaying;

    AudioEffect(protocol::AudioEffectType t, float v, float p)
        : type(t), volume(v), pitch(p), isPlaying(false) {}
};




/**
 * @brief Contain for the audio effects.
 *
 * Used by: AudioSystem
 */
struct AudioSource {
    AudioAssets assetId;
    bool loop;
    float minDistance;      // Distance avant atténuation
    float attenuation;      // Facteur d'atténuation (plus petit = moins d'atténuation)
    bool isUI;              // Si true, pas de positionnement 3D
    // sf::Sound* sound;       // Pointeur vers le sf::Sound actif

    AudioSource(AudioAssets asset, bool looping = false,
                float minDist = 100.0f, float atten = 0.5f, bool ui = false)
        : assetId(asset), loop(looping), minDistance(minDist),
          attenuation(atten), isUI(ui)
        //   sound(nullptr)
          {}

    ~AudioSource() {
        // Le AudioSystem s'occupera du nettoyage
    }
};


// ############################################################################
// ################################ BUTTON  ###################################
// ############################################################################

struct ButtonTextures {
    Assets none;
    Assets hover;
    Assets clicked;
};

enum ButtonState {
    NONE,
    HOVER,
    CLICKED,
};

struct ButtonComponent {
    ButtonTextures textures;
    ButtonState state = ButtonState::NONE;
    std::function<void()> onClick;

    ButtonComponent(ButtonTextures t, std::function<void()> click)
        : textures(t), onClick(click) {}
};

// ############################################################################
// ########################### R-TYPE SPECIFIC ################################
// ############################################################################

/**
 * @enum ForceAttachmentPoint
 * @brief Defines where the Force is attached to the player ship.
 */
enum class ForceAttachmentPoint : uint8_t {
    FORCE_DETACHED   = 0x00,
    FORCE_FRONT      = 0x01,
    FORCE_BACK       = 0x02,
    FORCE_ORBITING   = 0x03  // For special modes
};

/**
 * @brief R-Type Force component - the signature weapon.
 *
 * Represents the Force attachment to a player ship.
 * Used by: ShootSystem, ForceSystem, RenderSystem.
 */
struct Force
{
    uint32_t parentShipId;                   ///< Ship this Force is attached to (0 = detached)
    ForceAttachmentPoint attachmentPoint;    ///< Where it's attached
    uint8_t powerLevel;                      ///< Power level (1-5)
    uint8_t chargePercentage;                ///< Charge beam state (0-100)
    bool isFiring;                           ///< Whether the Force is currently firing

    Force(uint32_t parent, ForceAttachmentPoint attach, uint8_t power, uint8_t charge, bool firing)
        : parentShipId(parent), attachmentPoint(attach), powerLevel(power),
          chargePercentage(charge), isFiring(firing) {}
};

#endif /* !COMPONENTS_HPP_ */
