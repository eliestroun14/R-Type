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
#include <cstring>
#include <engine/render/RenderManager.hpp>

// #include <engine/render/RenderManager.hpp>

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
    char str[128];
    unsigned int size;
    sf::Color color;
    ZIndex zIndex;

    Text(const char *s, sf::Color c = sf::Color::White, unsigned int sz = 30, ZIndex z = ZIndex::IS_UI_HUD)
        : color(c), size(sz), zIndex(z)
    {
        // init tab to 0
        std::memset(this->str, 0, sizeof(this->str));
        // copy the text and be sure that we do not go over 127 char
        if (s)
            std::strncpy(this->str, s, sizeof(this->str) - 1);
    }
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
    uint64_t lastShotTime; // in milliseconds (uint64_t to avoid overflow with epoch timestamps)
    int damage;
    ProjectileType projectileType;
    Weapon(uint32_t fireRate, uint64_t lastShot, int damages, ProjectileType type)
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
 * @brief Tag component for timer UI elements that need dynamic updates.
 * 
 * TimerUI entities display countdown timers that update each frame.
 */
struct TimerUI {};



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
    float clientPosX = 0.0f;                     ///< Client's reported position X (for shoot sync)
    float clientPosY = 0.0f;                     ///< Client's reported position Y (for shoot sync)

    InputComponent(uint32_t id) : playerId(id), activeActions() {}
};

/**
 * @enum EnemyType
 * @brief Defines the type of enemy entity.
 */
enum EnemyType {
    BASIC = 0,
    FAST = 1,
    TANK = 2,
    BOSS = 3,
};

/**
 * @brief Tag component. Marks the entity as an enemy.
 *
 */
struct Enemy {
    EnemyType type = EnemyType::BASIC;
};

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
    float levelDuration;          // Total duration in seconds (0 = infinite/until all waves complete)
    std::string backgroundAsset;  // Path/name of background image asset
    std::string soundTheme;       // Path/name of background music asset
    int currentWaveIndex;
    float elapsedTime;
    bool completed;
    bool started;                 // Whether the level has started

    Level() : levelDuration(0.f), currentWaveIndex(0), elapsedTime(0.f), completed(false), started(false) {}
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
 * @struct AudioEffect
 * @brief Component for one-shot audio effects triggered at specific game events.
 *
 * Unlike AudioSource (which is attached to entities), AudioEffect represents
 * temporary sound effects that play once and disappear. Typically used with
 * Lifetime component for automatic cleanup.
 *
 * **Typical Use Cases:**
 * - Explosion sounds
 * - Impact/hit sounds
 * - Menu button clicks
 * - Power-up collection sounds
 * - Damage feedback sounds
 *
 * **Workflow:**
 * 1. Coordinator::playAudioEffect() creates a temporary entity
 * 2. Adds Transform (position), AudioEffect, and AudioSource components
 * 3. AudioSystem plays the sound via 3D audio or UI audio
 * 4. Lifetime component destroys the entity after a timeout
 *
 * @see AudioSource for persistent entity-attached audio
 * @see Coordinator::playAudioEffect() for implementation
 */
struct AudioEffect {
    /// @brief Audio effect type from protocol (determines which sound to play)
    protocol::AudioEffectType type;
    
    /// @brief Volume multiplier (0.0 = silent, 1.0 = full volume)
    float volume;
    
    /// @brief Pitch modifier (1.0 = normal pitch, <1.0 = lower, >1.0 = higher)
    float pitch;
    
    /// @brief Internal flag: tracks if this effect is currently playing
    bool isPlaying;

    /**
     * @brief Constructor for AudioEffect
     * @param t Audio effect type to play
     * @param v Volume level (0.0-1.0)
     * @param p Pitch modifier (1.0 = normal)
     */
    AudioEffect(protocol::AudioEffectType t, float v, float p)
        : type(t), volume(v), pitch(p), isPlaying(false) {}
};




/**
 * @struct AudioSource
 * @brief Component for managing audio playback attached to game entities.
 *
 * This component enables entities to play sounds with 3D positional audio support and
 * automatic replay capabilities. Sounds can be one-shot or repeating based on the `loop` flag.
 *
 * The AudioSystem processes all entities with this component and handles playback logic.
 *
 * **Playback Behavior:**
 * - When `loop = false` (default): Sound plays once and never repeats
 * - When `loop = true`: Sound replays automatically after `soundDuration` seconds elapse
 *
 * **3D Audio:**
 * - If `isUI = false`: Sound uses 3D positioning with attenuation based on distance
 * - If `isUI = true`: Sound plays at full volume (UI sound, no spatial processing)
 *
 * @see AudioSystem for implementation details
 * @see AudioEffect for one-shot effects
 */
struct AudioSource {
    /// @brief Audio asset to play (from AudioAssets enum)
    AudioAssets assetId;
    
    /// @brief Loop behavior: false = play once, true = replay after soundDuration
    bool loop;
    
    /// @brief Distance threshold (in units) before attenuation begins for 3D audio
    /// 
    /// **What it does:**
    /// If you're closer than this distance to the sound source, you hear it at FULL volume.
    /// Once you're farther than this, the volume starts to decrease based on distance.
    ///
    /// **Visual Example:**
    /// ```
    /// minDistance = 100 units
    ///
    /// Distance from sound:
    ///   0-100 units    → Full Volume (1.0)
    ///   100-200 units  → Volume decreasing (attenuation kicks in)
    ///   200+ units     → Quieter and quieter
    /// ```
    ///
    /// **Practical Examples:**
    /// - Close sound (explosion nearby): minDistance=50.0f (you hear full volume until 50 units away)
    /// - Far sound (distant gunshot): minDistance=200.0f (stays loud for longer)
    /// - UI sound (menu click): minDistance=0.0f (doesn't matter, set isUI=true instead)
    ///
    /// Default: 100.0f. Only applies when isUI=false
    float minDistance;
    
    /// @brief Attenuation factor for 3D audio (0.0-1.0)
    /// 
    /// **What it does:**
    /// Controls HOW FAST the volume decreases as you get farther from the sound.
    /// Higher values = sound fades faster with distance
    /// Lower values = sound stays loud for longer distance
    ///
    /// **Visual Examples:**
    /// ```
    /// Attenuation=0.1 (slow fade, stays loud longer)
    ///   100m ▓▓▓▓▓▓▓▓▓▓ (full volume)
    ///   200m ▓▓▓▓▓▓▓ (still pretty loud)
    ///   300m ▓▓▓ (quieter)
    ///   400m ▓ (very quiet)
    ///
    /// Attenuation=0.5 (moderate fade)
    ///   100m ▓▓▓▓▓▓▓▓▓▓ (full volume)
    ///   200m ▓▓▓▓ (noticeably quieter)
    ///   300m ▓ (very quiet)
    ///
    /// Attenuation=1.0 (fast fade, disappears quickly)
    ///   100m ▓▓▓▓▓▓▓▓▓▓ (full volume)
    ///   150m ▓ (almost gone)
    ///   200m ░ (barely audible)
    /// ```
    ///
    /// **Practical Examples:**
    /// - Close explosion (should fade quickly): attenuation=0.5f-1.0f
    /// - Distant ambient sound (should stay loud): attenuation=0.1f-0.2f
    /// - Gunshot (medium range): attenuation=0.3f-0.5f
    ///
    /// Default: 0.5f. Only applies when isUI=false
    float attenuation;
    
    /// @brief If true, this is a UI sound (no 3D positioning)
    /// 
    /// **What it controls:**
    /// - isUI=false (default): **3D Audio** - Volume depends on distance to player
    ///   * Uses minDistance and attenuation
    ///   * Plays from entity position using Transform component
    ///   * Example: explosion at (x,y) → volume decreases as player moves away
    ///
    /// - isUI=true: **UI Audio** - Always plays at full volume, no distance calculation
    ///   * Ignores minDistance and attenuation parameters
    ///   * Perfect for menu sounds, button clicks, notifications
    ///   * Always heard clearly regardless of camera position
    ///
    /// **When to use each:**
    /// ```
    /// // 3D sound (explosion blast)
    /// AudioSource(AudioAssets::SFX_EXPLOSION, false, 150.0f, 0.5f, false, 1.0f)
    ///                                                               ↑ false = 3D
    ///
    /// // UI sound (menu button click)
    /// AudioSource(AudioAssets::SFX_MENU_SELECT, false, 0.0f, 0.0f, true, 0.1f)
    ///                                                                   ↑ true = UI
    /// ```
    ///
    /// Default: false (3D audio enabled)
    bool isUI;
    
    /// @brief Internal flag: tracks if sound has played in current cycle
    /// Reset by AudioSystem when elapsedTimeSincePlay >= soundDuration (for replay)
    /// or when sound is first added (for one-shot)
    bool hasBeenPlayed;
    
    /// @brief Internal timer: elapsed time since sound last played (in seconds)
    /// Incremented by AudioSystem each frame. Used to determine replay timing.
    float elapsedTimeSincePlay;
    
    /// @brief Duration of the audio file (in seconds)
    /// Used to determine when a looping sound should replay
    float soundDuration;

    /**
     * @brief Constructor for AudioSource component
     *
     * @param asset The audio asset to play
     * @param looping If false (default), play once. If true, replay after soundDuration
     * @param minDist Distance threshold for attenuation (default: 100.0f)
     * @param atten Attenuation factor 0.0-1.0 (default: 0.5f)
     * @param ui If true, no 3D positioning (default: false)
     * @param duration Duration of audio file in seconds (default: 1.0f)
     *
     * **Parameter Guide:**
     *
     * **minDist** - "When does sound volume start decreasing?"
     * - 50.0f = Volume is full until you're 50 units away, then fades
     * - 100.0f = Volume is full until you're 100 units away, then fades
     * - 200.0f = Sound stays loud for longer distance
     *
     * **atten** - "How fast does sound fade when you move away?"
     * - 0.1f = Slow fade (distant ambient sound)
     * - 0.5f = Medium fade (normal effects)
     * - 1.0f = Fast fade (close, explosive sounds)
     *
     * **ui** - "Is this a UI sound or game world sound?"
     * - false = Game world sound (uses 3D positioning, fades with distance)
     * - true = UI sound (always full volume, no distance calculation)
     *
     * **looping** - "Should sound repeat?"
     * - false = Play once and stop (typical for gunshots, impacts)
     * - true = Replay after duration seconds (for ambient loops)
     *
     * @example
     * @code
     * // Projectile shot sound (loud nearby, fades with distance)
     * AudioSource(AudioAssets::SFX_SHOOT_BASIC, 
     *             false,      // loop: one-shot
     *             100.0f,     // minDist: full volume within 100 units
     *             0.5f,       // atten: moderate fade
     *             false,      // isUI: 3D positioned sound
     *             0.3f)       // duration: 0.3 seconds
     *
     * // Menu button click (always heard, no 3D positioning)
     * AudioSource(AudioAssets::SFX_MENU_SELECT,
     *             false,      // loop: one-shot
     *             0.0f,       // minDist: doesn't matter (UI sound)
     *             0.0f,       // atten: doesn't matter (UI sound)
     *             true,       // isUI: UI sound (ignore distance)
     *             0.5f)       // duration: 0.5 seconds
     *
     * // Ambient enemy hum (plays from enemy position, replays every 2 seconds)
     * AudioSource(AudioAssets::SFX_AMBIENT_HUM,
     *             true,       // loop: repeating
     *             200.0f,     // minDist: full volume within 200 units
     *             0.2f,       // atten: slow fade (ambient)
     *             false,      // isUI: 3D positioned sound
     *             2.0f)       // duration: 2 seconds between replays
     * @endcode
     */
    AudioSource(AudioAssets asset, bool looping = false,
                float minDist = 100.0f, float atten = 0.5f, bool ui = false, 
                float duration = 1.0f)
        : assetId(asset), loop(looping), minDistance(minDist),
          attenuation(atten), isUI(ui), hasBeenPlayed(false),
          elapsedTimeSincePlay(0.0f), soundDuration(duration)
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


// ############################################################################
// ############################ ACCESSIBILITY #################################
// ############################################################################

/**
 * @brief GameConfig component.
 *
 * Only ONE entity has this component assigned when Game is created.
 * Let know the game from the entity if music/sound are enabled or for keybinds.
 * Used by: AccessibilitySystem, RebindSystem.
 */
struct GameConfig {
    FontAssets activeFont;
    bool musicEnabled;
    bool soundEnabled;
    // you can add accessibility settings, maybe the keybinds ?
    std::map<sf::Keyboard::Key, GameAction> _keybinds;

    GameConfig(FontAssets activeF, bool music, bool sound)
        : activeFont(activeF), musicEnabled(music), soundEnabled(sound) {
            // Movement
            _keybinds[sf::Keyboard::Up] = GameAction::MOVE_UP;
            _keybinds[sf::Keyboard::Down] = GameAction::MOVE_DOWN;
            _keybinds[sf::Keyboard::Left] = GameAction::MOVE_LEFT;
            _keybinds[sf::Keyboard::Right] = GameAction::MOVE_RIGHT;

            // ACTIONS
            _keybinds[sf::Keyboard::S] = GameAction::SHOOT;
            _keybinds[sf::Keyboard::D] = GameAction::SWITCH_WEAPON;
            _keybinds[sf::Keyboard::Space] = GameAction::USE_POWERUP;
            _keybinds[sf::Keyboard::F] = GameAction::SPECIAL;

            // OTHER
            _keybinds[sf::Keyboard::P] = GameAction::OPTIONS;
            _keybinds[sf::Keyboard::Escape] = GameAction::EXIT;
        }
};

struct Rebind {
    GameAction action;
    bool isWaiting = false;
    Entity associatedText;

    Rebind(GameAction a, Entity textEnt)
        : action(a), isWaiting(false), associatedText(textEnt) {}
 
};
// ############################################################################
// ################################# TEAMS  ###################################
// ############################################################################

/**
 * @enum TeamType
 * @brief Defines the teams/factions an entity can belong to.
 *
 * An entity can belong to multiple teams simultaneously.
 * For example: a wall can be an OBSTACLE, so it doesn't take damage from enemies or other walls.
 */
enum class TeamType : uint8_t {
    PLAYER      = 1 << 0,  // 0x01 - Playable character
    ENEMY       = 1 << 1,  // 0x02 - AI enemy
    OBSTACLE    = 1 << 2,  // 0x04 - Wall, static object (doesn't take damage)
    POWERUP     = 1 << 3,  // 0x08 - Collectible item
    NEUTRAL     = 1 << 4,  // 0x10 - Neutral object (doesn't interact much)
    BOSS        = 1 << 5   // 0x20 - Boss enemy
};

/**
 * @brief Stores team membership for collision and damage rules.
 *
 * Uses bitflags to support multiple teams per entity.
 * 
 * **Examples:**
 * - Player: TeamType::PLAYER
 * - Wall: TeamType::OBSTACLE
 * - Boss: TeamType::BOSS | TeamType::ENEMY (both boss AND enemy)
 * - Collectible: TeamType::POWERUP
 * 
 * **Collision Rules (in CollisionSystem):**
 * - PLAYER projectiles don't hit PLAYER or POWERUP
 * - ENEMY projectiles don't hit ENEMY or OBSTACLE
 * - OBSTACLE never takes damage
 * - POWERUP is collected by PLAYER
 * 
 * Used by: CollisionSystem.
 */
struct Team
{
    uint8_t teamMask;  ///< Bitmask of teams this entity belongs to

    Team(uint8_t mask = static_cast<uint8_t>(TeamType::NEUTRAL))
        : teamMask(mask) {}

    Team(TeamType type)
        : teamMask(static_cast<uint8_t>(type)) {}

    /**
     * @brief Check if entity belongs to a specific team
     */
    bool hasTeam(TeamType type) const {
        return (teamMask & static_cast<uint8_t>(type)) != 0;
    }

    /**
     * @brief Add entity to a team (doesn't remove from other teams)
     */
    void addTeam(TeamType type) {
        teamMask |= static_cast<uint8_t>(type);
    }

    /**
     * @brief Remove entity from a specific team
     */
    void removeTeam(TeamType type) {
        teamMask &= ~static_cast<uint8_t>(type);
    }

    /**
     * @brief Check if this entity should collide with another based on teams
     * 
     * @param thisTeam This entity's Team component
     * @param otherTeam Other entity's Team component
     * @param thisIsProjectile Whether this entity is a projectile
     * @return true if collision should be processed
     */
    static bool canCollide(const Team& thisTeam, const Team& otherTeam, bool thisIsProjectile = false)
    {
        // Obstacles never take damage
        if (otherTeam.hasTeam(TeamType::OBSTACLE))
            return false;

        if (!thisIsProjectile)
            return true;  // Non-projectile collisions go through normal logic

        // Projectile collision rules
        bool shooterIsPlayer = thisTeam.hasTeam(TeamType::PLAYER);
        bool targetIsPlayer = otherTeam.hasTeam(TeamType::PLAYER);
        bool targetIsEnemy = otherTeam.hasTeam(TeamType::ENEMY);
        bool targetIsBoss = otherTeam.hasTeam(TeamType::BOSS);

        if (shooterIsPlayer) {
            // Player projectiles only hit ENEMY or BOSS (not PLAYER)
            return targetIsEnemy || targetIsBoss;
        } else {
            // Enemy projectiles only hit PLAYER (not other ENEMY)
            return targetIsPlayer;
        }
    }
};

#endif /* !COMPONENTS_HPP_ */
