#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <common/protocol/Packet.hpp>

namespace protocol {

    // ============================================================================
    // NETWORK PROTOCOL STRUCTURES - No padding for exact size control
    // ============================================================================
    #pragma pack(push, 1)

    // Using PacketHeader from Packet.hpp
    using PacketHeader = common::protocol::PacketHeader;

    //Packet Type
    enum class PacketTypes : uint8_t {

        //CONNECTION          = 0x01-0x0F
        TYPE_CLIENT_CONNECT         = 0x01,
        TYPE_SERVER_ACCEPT          = 0x02,
        TYPE_SERVER_REJECT          = 0x03,
        TYPE_CLIENT_DISCONNECT      = 0x04,
        TYPE_HEARTBEAT              = 0x05,

        //INPUT               = 0x10-0x1F 
        TYPE_PLAYER_INPUT = 0x10,

        //WORLD_STATE         = 0x20-0x3F (Reserved for future use or legacy)
        // TYPE_WORLD_SNAPSHOT      = 0x20,  // DEPRECATED: Use ECS Component Snapshots instead (0x24-0x2F)
        TYPE_ENTITY_SPAWN           = 0x21,
        TYPE_ENTITY_DESTROY         = 0x22,
        // TYPE_ENTITY_UPDATE       = 0x23,  // DEPRECATED: Use Component-specific snapshots instead

        //GAME_EVENTS                 = 0x40-0x5F 
        TYPE_PLAYER_HIT             = 0x40,
        TYPE_PLAYER_DEATH           = 0x41,
        TYPE_SCORE_UPDATE           = 0x42,
        TYPE_POWER_PICKUP           = 0x43,
        TYPE_WEAPON_FIRE            = 0x44,

        //GAME_CONTROL                = 0x60-0x6F 
        TYPE_GAME_START             = 0x60,
        TYPE_GAME_END               = 0x61,
        TYPE_LEVEL_COMPLETE         = 0x62,
        TYPE_LEVEL_START            = 0x63,

        //PROTOCOL_CONTROL            = 0x70-0x7F
        TYPE_ACK                    = 0x70,
        TYPE_PING                   = 0x71,
        TYPE_PONG                   = 0x72,

        //ECS_COMPONENTS              = 0x24-0x2F
        TYPE_TRANSFORM_SNAPSHOT     = 0x24,             // Snapshot des Transform components
        TYPE_VELOCITY_SNAPSHOT      = 0x25,             // Snapshot des Velocity components
        TYPE_HEALTH_SNAPSHOT        = 0x26,             // Snapshot des Health components
        TYPE_WEAPON_SNAPSHOT        = 0x27,             // Snapshot des Weapon components
        TYPE_AI_SNAPSHOT            = 0x28,             // Snapshot des AI components
        TYPE_ANIMATION_SNAPSHOT     = 0x29,             // Snapshot des Animation components
        TYPE_COMPONENT_ADD          = 0x2A,             // Ajouter un composant (reliable)
        TYPE_COMPONENT_REMOVE       = 0x2B,             // Retirer un composant (reliable)
        TYPE_TRANSFORM_SNAPSHOT_DELTA = 0x2C,           // Delta Transform snapshot
        TYPE_HEALTH_SNAPSHOT_DELTA  = 0x2D,             // Delta Health snapshot
        TYPE_ENTITY_FULL_STATE      = 0x2E,             // État complet d'une entité (spawn)

        //EFFECTS                     = 0x50-0x5F
        TYPE_VISUAL_EFFECT          = 0x50,
        TYPE_AUDIO_EFFECT           = 0x51,
        TYPE_PARTICLE_SPAWN         = 0x52,

        //GAME_SYSTEMS                = 0x64-0x6F
        TYPE_FORCE_STATE            = 0x64,
        TYPE_AI_STATE               = 0x65
    };



    // Packet Flags
    enum class PacketFlags : uint8_t {
        FLAG_RELIABLE               = 0x01,             // Requires acknowledge
        FLAG_COMPRESSED             = 0x02,             // Payload is compressed (and needs to be decompressed)
        FLAG_ENCRYPTED              = 0x04,             // Payload is encrypted
        FLAG_FRAGMENTED             = 0x08,             // Part of fragmented msg
        FLAG_PRIORITY               = 0x10,             // High prio processing
        FLAG_RESERVED_5             = 0x20,             // Reserved for future
        FLAG_RESERVED_6             = 0x40,             // Reserved for future
        FLAG_RESERVED_7             = 0x80              // Reserved for future
    };



    // Client -> Server
    struct ClientConnect {

        protocol::PacketHeader header;                  // type = 0x01
        uint8_t                protocol_version;        // Version = 1
        char                   player_name[32];         // Player name
        uint32_t               client_id;               // Unique client identifier
    };
    // total size: 49 bytes


    // Server -> Client
    struct ServerAccept {

        protocol::PacketHeader header;                  // type = 0x02
        uint32_t               assigned_player_id;      // Unique player identifier assigned by server
        uint8_t                max_players;             // Maximum players allowed on server
        uint32_t               game_instance_id;        // Unique game instance identifier
        uint16_t               server_tickrate;         // Server tickrate in Hz
    };
    // total size: 23 bytes

    // Server -> Client
    struct ServerReject {
        protocol::PacketHeader header;                 // type = 0x03
        uint8_t                reject_code;            // Reason code for rejection
        char                   reason_message[64];     // Human-readable reason message
    };
    // total size: 77 bytes

    enum class RejectCodes : uint8_t {
        REJECT_SERVER_FULL                   = 0x00,   // Server is full
        REJECT_INCOMPATIBLE_PROTOCOL_VERSION = 0x01,   // Protocol version mismatch
        REJECT_INVALID_PLAYER_NAME           = 0x02,   // Invalid player name
        REJECT_BANNED_CLIENT                 = 0x03,   // Banned client
        REJECT_GENERIC_ERROR                 = 0xFF    // Generic error
        // Add here if we need
    };

    static constexpr uint8_t REJECT_CODE_MIN = static_cast<uint8_t>(RejectCodes::REJECT_SERVER_FULL);
    static constexpr uint8_t REJECT_CODE_MAX = static_cast<uint8_t>(RejectCodes::REJECT_BANNED_CLIENT);
    static constexpr uint8_t REJECT_CODE_GENERIC_ERROR = static_cast<uint8_t>(RejectCodes::REJECT_GENERIC_ERROR);

    // Client -> Server or Server -> Client
    struct ClientDisconnect {
        protocol::PacketHeader header;                 // type = 0x04 + FLAG_RELIABLE
        uint32_t               client_id;              // Unique client identifier
        uint8_t                reason;                 // Reason for disconnection
    };
    // total size: 17 bytes

    enum class DisconnectReasons : uint8_t {
        REASON_NORMAL_DISCONNECT            = 0x00,    // Client initiated disconnect
        REASON_TIMEOUT                      = 0x01,    // Server initiated disconnect
        REASON_KICKED_BY_SERVER             = 0x02,    // Kicked by server admin
        REASON_CLIENT_ERROR                 = 0x03,    // Client error
        REASON_GENERIC_ERROR                = 0xFF     // Generic error
        // Add here if we need
    };

    static constexpr uint8_t DISCONNECT_REASON_MIN = static_cast<uint8_t>(DisconnectReasons::REASON_NORMAL_DISCONNECT);
    static constexpr uint8_t DISCONNECT_REASON_MAX = static_cast<uint8_t>(DisconnectReasons::REASON_CLIENT_ERROR);
    static constexpr uint8_t DISCONNECT_REASON_GENERIC_ERROR = static_cast<uint8_t>(DisconnectReasons::REASON_GENERIC_ERROR);

    // Client -> Server or Server -> Client
    struct HeartBeat {
        protocol::PacketHeader header;                 // type = 0x05
        uint32_t               player_id;              // Unique player identifier
    };
    // total size: 16 bytes

    // Client -> Server
    struct PlayerInput {
        protocol::PacketHeader header;                 // type = 0x10
        uint32_t               player_id;              // Unique player identifier
        uint16_t               input_state;            // Bitfield of input states
        uint16_t               aim_direction_x;        // Optional Aim direction X (normalized from float to uint16_t to gain speed)
        uint16_t               aim_direction_y;        // Optional Aim direction Y (normalized from float to uint16_t to gain speed)
    };
    // total size: 24 bytes

    enum class InputFlags : uint16_t {
        // Bit 0 - Bit 15
        INPUT_MOVE_UP            = 0x0001,
        INPUT_MOVE_DOWN          = 0x0002,
        INPUT_MOVE_LEFT          = 0x0004,
        INPUT_MOVE_RIGHT         = 0x0008,
        INPUT_FIRE_PRIMARY       = 0x0010,
        INPUT_FIRE_SECONDARY     = 0x0020,
        INPUT_ACTION_SPECIAL     = 0x0040,
        INPUT_RESERVED_7         = 0x0080,
        INPUT_RESERVED_8         = 0x0100,
        // Bits 9-15 reserved for future bonus actions
    };

    static constexpr uint16_t INPUT_FLAGS_MASK = 0x01FF;  // Bits 0-8 valid, bits 9-15 reserved

    // ============================================================================
    // ENTITY TYPES (Used in ECS component system)
    // ============================================================================

    enum class EntityTypes : uint8_t {
        ENTITY_TYPE_PLAYER             = 0x01,
        ENTITY_TYPE_ENEMY              = 0x02,
        ENTITY_TYPE_ENEMY_BOSS         = 0x03,
        ENTITY_TYPE_PROJECTILE_PLAYER  = 0x04,
        ENTITY_TYPE_PROJECTILE_ENEMY   = 0x05,
        ENTITY_TYPE_POWERUP            = 0x06,
        ENTITY_TYPE_OBSTACLE           = 0x07,
        ENTITY_TYPE_BG_ELEMENT         = 0x08
        // Add here if we need
    };

    static constexpr uint8_t ENTITY_TYPE_MIN = static_cast<uint8_t>(EntityTypes::ENTITY_TYPE_PLAYER);
    static constexpr uint8_t ENTITY_TYPE_MAX = static_cast<uint8_t>(EntityTypes::ENTITY_TYPE_BG_ELEMENT);

    // Server -> Client
    struct EntitySpawn {
        protocol::PacketHeader header;                 // type = 0x21 + FLAG_RELIABLE
        uint32_t               entity_id;              // Unique entity identifier
        uint8_t                entity_type;            // Entity type
        uint16_t               position_x;             // X position
        uint16_t               position_y;             // Y position
        uint8_t                mob_variant;            // Sub-type for mobs ex : melee, shooting one etc
        uint8_t                initial_health;         // Initial health
        uint16_t               initial_velocity_x;     // Initial X velocity
        uint16_t               initial_velocity_y;     // Initial Y velocity
        uint8_t                is_playable;            // 0 = no Playable component, 1 = has Playable (local player)
    };
    // total size: 28 bytes

    // Server -> Client
    struct EntityDestroy {
        protocol::PacketHeader header;                 // type = 0x22 + FLAG_RELIABLE
        uint32_t               entity_id;              // Unique entity identifier
        uint8_t                destroy_reason;         // Reason for destruction
        uint16_t               final_position_x;      // Final X position (for animations)
        uint16_t               final_position_y;      // Final Y position (for animations)
    };
    // total size: 21 bytes

    enum class EntityDestroyReasons : uint8_t {
        DESTROY_KILLED_BY_PLAYER      = 0x00,
        DESTROY_KILLED_BY_ENEMY       = 0x01,
        DESTROY_OUT_OF_BOUNDS         = 0x02,
        DESTROY_TIMEOUT_DESPAWN       = 0x03,
        DESTROY_LEVEL_TRANSITION      = 0x04
        // Add here if we need
    };

    static constexpr uint8_t ENTITY_DESTROY_REASON_MIN = static_cast<uint8_t>(EntityDestroyReasons::DESTROY_KILLED_BY_PLAYER);
    static constexpr uint8_t ENTITY_DESTROY_REASON_MAX = static_cast<uint8_t>(EntityDestroyReasons::DESTROY_LEVEL_TRANSITION);

    // Server -> Client
    struct EntityUpdate {
        PacketHeader    header;                         // type = 0 x23
        uint32_t        entity_id;                      // Entity to update
        uint8_t         update_flags;                   // Which fields are updated
        uint16_t        pos_x;                          // Updated X position ( if flag set )
        uint16_t        pos_y;                          // Updated Y position ( if flag set )
        uint8_t         health;                         // Updated health ( if flag set )
        uint8_t         shield;                         // Updated shield ( if flag set )
        uint8_t         state_flags;                    // Updated state ( if flag set )
        uint16_t        velocity_x;                     // Updated X velocity ( if flag set )
        uint16_t        velocity_y;                     // Updated Y velocity ( if flag set )
    };
    // Total size : 28 bytes

    enum class EntityUpdateFlags : uint8_t {
        // Bits 0-4
        UPDATE_POS            = 0x01,
        UPDATE_HEALTH         = 0x04,
        UPDATE_SHIELD         = 0x08,
        UPDATE_STATE_FLAGS    = 0x10,
        UPDATE_VELOCITY       = 0x20
        // Bits 5-7 reserved for future use
    };

    static constexpr uint8_t ENTITY_UPDATE_FLAGS_MASK = 0x3F;  // Bits 0-5 valid, bits 6-7 reserved

    // Server -> Client
    struct PlayerHit {
        PacketHeader   header;                          // type = 0x40, FLAG_RELIABLE
        uint32_t       player_id;                       // Player who was hit
        uint32_t       attacker_id;                     // Entity that caused damage
        uint8_t        damage;                          // Damage amount
        uint8_t        remaining_health;                // Health after damage
        uint8_t        remaining_shield;                // Shield after damage
        int16_t        hit_pos_x;                       // Hit location X
        int16_t        hit_pos_y;                       // Hit location Y
    };
    // Total size: 27 bytes

    // Server -> Client
    struct PlayerDeath {
        PacketHeader    header;                         // type = 0x41, FLAG_RELIABLE
        uint32_t        player_id;                      // Player who died
        uint32_t        killer_id;                      // Entity that killed player
        uint32_t        score_before_death;             // Player's score
        int16_t         death_pos_x;                    // Death location X
        int16_t         death_pos_y;                    // Death location Y
    };
    // Total size: 28 bytes

    // Server -> Client
    struct ScoreUpdate {
        PacketHeader    header;                         // type = 0x42
        uint32_t        player_id;                      // Player whose score changed
        uint32_t        new_score;                      // Updated total score
        int16_t         score_delta;                    // Change in score (can be negative)
        uint8_t         reason;                         // Reason for score change
    };
    // Total size: 23 bytes

    enum class ScoreChangeReasons : uint8_t {
        SCORE_ENEMY_KILL         = 0x00,
        SCORE_BOSS_KILL          = 0x01,
        SCORE_POWERUP_COLLECTED  = 0x02,
        SCORE_LEVEL_COMPLETED    = 0x03,
        SCORE_BONUS_SURVIVAL     = 0x04,
        SCORE_TOOK_DAMAGE        = 0x05
        // Add here if we need
    };

    // Server -> Client
    struct PowerupPickup {
        PacketHeader    header;                         // type = 0x43, FLAG_RELIABLE
        uint32_t        player_id;                      // Player who picked up powerup
        uint32_t        powerup_id;                     // Powerup entity ID
        uint8_t         powerup_type;                   // Type of powerup
        uint8_t         duration;                       // Effect duration (seconds, 0=permanent)
    };
    // Total size: 22 bytes

    enum class PowerupTypes : uint8_t {
        POWERUP_SPEED_BOOST        = 0x00,
        POWERUP_WEAPON_UPGRADE       = 0x01,
        POWERUP_FORCE                = 0x02,    // R-Type signature
        POWERUP_SHIELD               = 0x03,
        POWERUP_EXTRA_LIFE           = 0x04,
        POWERUP_INVINCIBILITY        = 0x05,
        POWERUP_HEAL                 = 0x06
        // Add here if we need...
    };

    // Server -> Client
    struct WeaponFire {
        PacketHeader    header;                 // type = 0x44
        uint32_t        shooter_id;             // Entity that fired
        uint32_t        projectile_id;          // New projectile entity ID
        int16_t         origin_x;               // Fire origin X
        int16_t         origin_y;               // Fire origin Y
        int16_t         direction_x;            // Direction vector X (normalized*1000)
        int16_t         direction_y;            // Direction vector Y (normalized*1000)
        uint8_t         weapon_type;            // Weapon type fired
    };
    // Total size: 29 bytes

    enum class WeaponTypes : uint8_t {
        WEAPON_TYPE_BASIC          = 0x00,
        WEAPON_TYPE_CHARGED        = 0x01,
        WEAPON_TYPE_SPREAD         = 0x02,
        WEAPON_TYPE_LASER          = 0x03,
        WEAPON_TYPE_MISSILE        = 0x04,
        WEAPON_TYPE_FORCE_SHOT     = 0x05
        // Add here if we need...
    };

    // Server -> Client
    struct GameStart {
        PacketHeader    header;                 // type = 0x60, FLAG_RELIABLE
        uint32_t        game_instance_id;       // Game instance identifier
        uint8_t         player_count;           // Number of players
        uint32_t        player_ids[4];          // Player IDs (up to 4)
        uint8_t         level_id;               // Starting level
        uint8_t         difficulty;             // Difficulty setting
    };
    // Total size: 35 bytes

    enum class DifficultyLevels : uint8_t {
        DIFFICULTY_EASY        = 0x00,
        DIFFICULTY_NORMAL      = 0x01,
        DIFFICULTY_HARD        = 0x02,
        DIFFICULTY_INSANE      = 0x03
    };

    // Server -> Client
    struct GameEnd {
        PacketHeader    header;                 // type = 0x61, FLAG_RELIABLE
        uint8_t         end_reason;             // Reason game ended
        uint32_t        final_scores[4];        // Final scores for all players
        uint8_t         winner_id;              // Winning player (if applicable)
        uint32_t        play_time;              // Total game time (seconds)
    };
    // Total size: 34 bytes

    enum class GameEndReasons : uint8_t {
        GAME_END_VICTORY            = 0x00,     // All levels completed
        GAME_END_DEFEAT             = 0x01,     // All players dead
        GAME_END_TIMEOUT            = 0x02,     // Time limit reached
        GAME_END_PLAYER_LEFT        = 0x03,     // Player voluntarily quit
        GAME_END_SERVER_SHUTDOWN    = 0x04      // Server shutdown
    };

    // Server -> Client
    struct LevelComplete {
        PacketHeader    header;                 // type = 0x62, FLAG_RELIABLE
        uint8_t         completed_level;        // Level that was completed
        uint8_t         next_level;             // Next level to load (0xFF=game end)
        uint32_t        bonus_score;            // Completion bonus
        uint16_t        completion_time;        // Time taken (seconds)
    };
    // Total size: 20 bytes

    // Server -> Client
    struct LevelStart {
        PacketHeader    header;                 // type = 0x63, FLAG_RELIABLE
        uint8_t         level_id;               // Level identifier
        char            level_name[32];         // Level name
        uint16_t        estimated_duration;     // Estimated time (seconds)
    };
    // Total size: 47 bytes

    struct Acknowledgment {
        PacketHeader    header;                 // type = 0x70
        uint32_t        acked_sequence;         // Sequence number being ACKed
        uint32_t        received_timestamp;     // When packet was received
        uint32_t        client_id;              // Unique client identifier
    };
    // Total size 20 + client_id size = 24 bytes

    struct Ping {
        PacketHeader    header;                 // type = 0x71
        uint32_t        client_timestamp;       // Client's current timestamp
        uint32_t        client_id;              // Unique client identifier
    };
    // Total size: 16 bytes

    struct Pong {
        PacketHeader    header;                // type = 0x72
        uint32_t        client_timestamp;      // Original client timestamp from PING
        uint32_t        server_timestamp;      // Server's timestamp when received
    };
    // Total size: 20 bytes

    // ============================================================================
    // ECS COMPONENT SYSTEM
    // ============================================================================

    // Component Type Identifiers
    enum class ComponentType : uint8_t {
        COMPONENT_TRANSFORM       = 0x01,       // Position, rotation, scale
        COMPONENT_VELOCITY        = 0x02,       // Linear velocity
        COMPONENT_HEALTH          = 0x03,       // Health/shield
        COMPONENT_WEAPON          = 0x04,       // Weapon state
        COMPONENT_AI              = 0x05,       // AI behavior
        COMPONENT_FORCE           = 0x06,       // R-Type Force attachment
        COMPONENT_HITBOX          = 0x07,       // Collision bounds
        COMPONENT_SPRITE          = 0x08,       // Visual representation
        COMPONENT_ANIMATION       = 0x09,       // Animation state
        COMPONENT_POWERUP         = 0x0A,       // Powerup effect
        COMPONENT_SCORE           = 0x0B,       // Score tracking
        COMPONENT_INPUT           = 0x0C,       // Input state (for players)
        COMPONENT_PHYSICS         = 0x0D,       // Physics properties
        COMPONENT_LIFETIME        = 0x0E,       // Auto-destroy after time
        COMPONENT_PARENT          = 0x0F,       // Parent-child relationship
        // Reserved 0x10-0xFF for future components
    };

    // Individual Component Data Structures
    struct ComponentTransform {
        int16_t   pos_x;
        int16_t   pos_y;
        uint16_t  rotation;                     // 0-65535 maps to 0-360 degrees
        uint16_t  scale;                        // Fixed point: 1000 = 1.0x scale
    };
    // Size: 8 bytes

    struct ComponentVelocity {
        int16_t   vel_x;
        int16_t   vel_y;
        int16_t   acceleration_x;               // Optional acceleration
        int16_t   acceleration_y;
    };
    // Size: 8 bytes

    struct ComponentHealth {
        uint8_t   current_health;
        uint8_t   max_health;
        uint8_t   current_shield;
        uint8_t   max_shield;
    };
    // Size: 4 bytes

    struct ComponentWeapon {
        uint8_t   weapon_type;
        uint8_t   ammo_count;
        uint16_t  cooldown_remaining;           // Milliseconds
        uint8_t   power_level;                  // 1-5 for R-Type
    };
    // Size: 5 bytes

    struct ComponentAI {
        uint8_t   ai_state;                     // Current AI state
        uint8_t   behavior_type;                // AI behavior pattern
        uint32_t  target_entity_id;             // Current target (0 = no target)
        uint16_t  state_timer;                  // Time in current state (ms)
    };
    // Size: 8 bytes

    struct ComponentForce {
        uint32_t  parent_ship_id;               // Ship this Force is attached to
        uint8_t   attachment_point;             // 0=front, 1=back, 2=detached
        uint8_t   force_level;                  // Power level
        uint8_t   charge_state;                 // Charge beam state
    };
    // Size: 7 bytes

    struct ComponentCollision {
        uint8_t   collision_layer;              // Physics layer
        uint8_t   collision_mask;               // What layers it collides with
        uint16_t  radius;                       // Collision radius (for circle)
        int16_t   bounds_width;                 // For AABB
        int16_t   bounds_height;
    };
    // Size: 8 bytes

    struct ComponentSprite {
        uint16_t  sprite_id;          // Sprite asset ID
        uint8_t   color_tint_r;       // Color modulation
        uint8_t   color_tint_g;
        uint8_t   color_tint_b;
        uint8_t   alpha;              // Transparency
        uint8_t   layer;              // Rendering layer
    };
    // Size: 7 bytes

    struct ComponentAnimation {
        uint16_t  animation_id;       // Current animation
        uint16_t  frame_index;        // Current frame
        uint16_t  frame_duration;     // MS per frame
        uint8_t   loop_mode;          // 0=once, 1=loop, 2=pingpong
    };
    // Size: 7 bytes

    struct ComponentPowerup {
        uint8_t   powerup_type;
        uint16_t  duration_remaining; // Milliseconds (0=permanent)
        uint8_t   stack_count;        // For stackable powerups
    };
    // Size: 4 bytes

    struct ComponentLifetime {
        uint16_t  remaining_ms;       // Time until auto-destroy
    };
    // Size: 2 bytes

    struct ComponentParent {
        uint32_t  parent_entity_id;   // Parent entity (0 = no parent)
        int16_t   offset_x;           // Relative position to parent
        int16_t   offset_y;
    };
    // Size: 8 bytes

    // ============================================================================
    // ECS COMPONENT SNAPSHOTS (Pure ECS Architecture) /!\ A LIRE /!\
    // ============================================================================
    
    /**
     * ARCHITECTURE : SNAPSHOTS PAR TYPE DE COMPOSANT
     * 
     * Cette approche est mieux pour une architecture ECS car :
     * 1. Chaque type de composant a son propre packet
     * 2. Le serveur itère sur un seul type de composant à la fois (cache-friendly)
     * 3. Permet des fréquences d'update différentes par composant
     * 4. Compression optimale (données homogènes)
     * 5. Aligné avec l'architecture ECS pure (un système = un type de composant)
     * 
     * Exemple d'utilistion :
     * - TransformSnapshot : 60 Hz (mouvement fluide)
     * - HealthSnapshot : 20 Hz (moins critique)
     * - SpriteSnapshot : 10 Hz (changements rares)
     */

    // ============================================================================
    // COMPONENT SNAPSHOTS (0x24-0x2F)
    // ============================================================================

    /**
     * NOTE D'IMPLÉMENTATION :
     * 
     * Ces snapshots sont reçus au format sérializé (structure ci-dessous).
     * Lors de la désérialisation par la socket au moment de recevoir le packet, les données du tableau 
     * variable sont converties en std::vector<std::pair<uint32_t, ComponentXXX>>
     * pour faciliter le traitement côté application.
     * 
     * Exemple pour TransformSnapshot :
     * - Format réseau : header + world_tick + entity_count + [entity_id, ComponentTransform][]
     * - Format après désérialisation : vector<pair<entity_id, ComponentTransform>>
     */

    // Server -> Client: Snapshot of Transform components
    // Envoyé fréquemment (30-60 Hz) pour le mouvement
    struct TransformSnapshot {
        PacketHeader    header;                 // type = 0x24
        uint32_t        world_tick;             // Server tick number
        uint16_t        entity_count;           // Number of entities with Transform
        // Followed by entity_count entries of:
        // [uint32_t entity_id][ComponentTransform data (8 bytes)]
        // Total per entity: 12 bytes
    };
    // Base size: 18 bytes + (entity_count × 12) bytes
    // Exemple: 20 entités = 18 + 240 = 258 bytes

    // Server -> Client: Snapshot of Velocity components
    // Envoyé moyennement (20-30 Hz)
    struct VelocitySnapshot {
        PacketHeader    header;                 // type = 0x25
        uint32_t        world_tick;
        uint16_t        entity_count;
        // Followed by: [uint32_t entity_id][ComponentVelocity data (8 bytes)]
        // Total per entity: 12 bytes
    };
    // Base size: 18 bytes + (entity_count × 12) bytes

    // Server -> Client: Snapshot of Health components
    // Envoyé moins souvent (10-20 Hz) car moins critique pour l'affichage
    struct HealthSnapshot {
        PacketHeader    header;                 // type = 0x26
        uint32_t        world_tick;
        uint16_t        entity_count;
        // Followed by: [uint32_t entity_id][ComponentHealth data (4 bytes)]
        // Total per entity: 8 bytes
    };
    // Base size: 18 bytes + (entity_count × 8) bytes
    // Plus compact ! 20 entités = 18 + 160 = 178 bytes

    // Server -> Client: Snapshot of Weapon components
    // Envoyé rarement (5-10 Hz) sauf changement
    struct WeaponSnapshot {
        PacketHeader    header;                 // type = 0x27
        uint32_t        world_tick;
        uint16_t        entity_count;
        // Followed by: [uint32_t entity_id][ComponentWeapon data (5 bytes)]
        // Total per entity: 9 bytes
    };
    // Base size: 18 bytes + (entity_count × 9) bytes

    // Server -> Client: Snapshot of AI components
    // Envoyé rarement (2-5 Hz) car l'IA est prédictible
    struct AISnapshot {
        PacketHeader    header;                 // type = 0x28
        uint32_t        world_tick;
        uint16_t        entity_count;
        // Followed by: [uint32_t entity_id][ComponentAI data (8 bytes)]
        // Total per entity: 12 bytes
    };
    // Base size: 18 bytes + (entity_count × 12) bytes

    // Server -> Client: Snapshot of Animation components
    // Envoyé moyennement (10-20 Hz)
    struct AnimationSnapshot {
        PacketHeader    header;                 // type = 0x29
        uint32_t        world_tick;
        uint16_t        entity_count;
        // Followed by: [uint32_t entity_id][ComponentAnimation data (7 bytes)]
        // Total per entity: 11 bytes
    };
    // Base size: 18 bytes + (entity_count × 11) bytes

    // ============================================================================
    // COMPONENT LIFECYCLE (Reliable packets)
    // ============================================================================

    // Server -> Client: Add a component to an entity
    // Utilisé quand une entité gagne un nouveau composant
    // Exemple : Joueur ramasse un powerup -> ajoute ComponentPowerup
    struct ComponentAdd {
        PacketHeader    header;                 // type = 0x2A, FLAG_RELIABLE
        uint32_t        entity_id;              // Target entity
        uint8_t         component_type;         // ComponentType enum
        uint8_t         data_size;              // Size of component data
        // Followed by component data (variable size)
    };
    // Base size: 18 bytes + component_data_size
    // Exemple avec Health: 18 + 4 = 22 bytes

    // Server -> Client: Remove a component from an entity
    // Utilisé quand un composant est retiré
    // Exemple : Powerup expire -> retire ComponentPowerup
    struct ComponentRemove {
        PacketHeader    header;                 // type = 0x2B, FLAG_RELIABLE
        uint32_t        entity_id;              // Target entity
        uint8_t         component_type;         // ComponentType to remove
    };
    // Size: 17 bytes

    // ============================================================================================
    // DELTA SNAPSHOTS (Optimisation avancée) Mentionné dans bcp d'articles pour des vrais jeux
    // ============================================================================================

    /**
     * DELTA SNAPSHOTS : Envoie uniquement les changements depuis le dernier snapshot
     * 
     * Utilisation :
     * - Snapshot FULL : Toutes les entités avec ce composant
     * - Snapshot DELTA : Seulement celles qui ont changé
     * 
     * Le client maintient l'état et applique les deltas
     */

    // Server -> Client: Delta Transform snapshot (only changed entities)


    struct TransformSnapshotDelta {

        PacketHeader    header;                 // type = 0x2C
        uint32_t        world_tick;
        uint32_t        base_tick;              // Tick de référence
        uint16_t        entity_count;           // Seulement les entités modifiées
        // Followed by: [uint32_t entity_id][ComponentTransform data]
    };
    // Économie massive : Si seulement 5/40 entités bougent
    // Full: 18 + 40×12 = 498 bytes
    // Delta: 18 + 5×12 = 78 bytes → 84% d'économie !

    // Server -> Client: Delta Health snapshot
    struct HealthSnapshotDelta {
        PacketHeader    header;                 // type = 0x2D
        uint32_t        world_tick;
        uint32_t        base_tick;
        uint16_t        entity_count;
        // Followed by: [uint32_t entity_id][ComponentHealth data]
    };

    // ============================================================================
    // MULTI-COMPONENT SNAPSHOT (Cas particulier)
    // ============================================================================

    /**
     * Utilisé pour synchroniser plusieurs composants d'une entité spécifique
     * Cas d'usage : Nouveau joueur rejoint (souvent en cas de reconnexion) → envoie son état complet
     */
    struct EntityFullState {
        PacketHeader    header;                 // type = 0x2E, FLAG_RELIABLE
        uint32_t        entity_id;
        uint8_t         entity_type;            // EntityTypes enum
        uint8_t         component_count;        // Number of components
        // Followed by component_count entries of:
        // [uint8_t component_type][uint8_t size][component_data]
    };
    // Usage : Spawn complet d'un joueur avec tous ses composants

    // ============================================================================
    // EFFECTS SYSTEM (Visual and Audio feedback
    // ============================================================================

    enum class VisualEffectType : uint8_t {
        VFX_EXPLOSION_SMALL       = 0x00,
        VFX_EXPLOSION_MEDIUM      = 0x01,
        VFX_EXPLOSION_LARGE       = 0x02,
        VFX_MUZZLE_FLASH          = 0x03,
        VFX_IMPACT_SPARK          = 0x04,
        VFX_POWERUP_GLOW          = 0x05,
        VFX_SHIELD_HIT            = 0x06,
        VFX_WARP_IN               = 0x07,
        VFX_WARP_OUT              = 0x08,
        VFX_CHARGE_BEAM           = 0x09,
        VFX_FORCE_DETACH          = 0x0A,
        VFX_PLAYER_SPAWN          = 0x0B,
        VFX_BOSS_INTRO            = 0x0C
        // Add more as needed
    };

    enum class AudioEffectType : uint8_t {
        // WEAPONS
        SFX_SHOOT_BASIC            = 0x00,
        SFX_SHOOT_CHARGED          = 0x01,
        SFX_SHOOT_LASER            = 0x02,

        // EXPLOSIONS
        SFX_EXPLOSION_SMALL_1      = 0x03,
        SFX_EXPLOSION_SMALL_2      = 0x04,
        SFX_EXPLOSION_LARGE_1      = 0x05,
        SFX_EXPLOSION_LARGE_2      = 0x06,

        // POWERUPS
        SFX_POWERUP_COLLECT_1      = 0x07,
        SFX_POWERUP_COLLECT_2      = 0x08,

        // PLAYER
        SFX_PLAYER_HIT             = 0x09,
        SFX_PLAYER_DEATH_1         = 0x0A,
        SFX_PLAYER_DEATH_2         = 0x0B,
        SFX_PLAYER_DEATH_3         = 0x0C,

        // FORCE
        SFX_FORCE_ATTACH           = 0x0D,
        SFX_FORCE_DETACH           = 0x0E,

        // BOSS
        SFX_BOSS_ROAR              = 0x0F,

        // UI
        SFX_MENU_SELECT            = 0x10,
        SFX_MENU_ALERT             = 0x11,

        // MUSIC
        MAIN_MENU_MUSIC            = 0x12,
        FIRST_LEVEL_MUSIC          = 0x13,
        SECOND_LEVEL_MUSIC         = 0x14,
        THIRD_LEVEL_MUSIC          = 0x15,
        FOURTH_LEVEL_MUSIC         = 0x16,
        VICTORY_MUSIC              = 0x17,
        DEFEAT_MUSIC               = 0x18

        // Add more as needed
    };

    // Server -> Client: Spawn a visual effect
    struct VisualEffect {
        PacketHeader    header;                 // type = 0x50
        uint8_t         effect_type;
        int16_t         pos_x;
        int16_t         pos_y;
        uint16_t        duration_ms;            // Effect duration
        uint8_t         scale;                  // Scale multiplier (100 = 1.0x)
        uint8_t         color_tint_r;           // Optional color modulation
        uint8_t         color_tint_g;
        uint8_t         color_tint_b;
    };
    // Size: 23 bytes

    // Server -> Client: Play an audio effect
    struct AudioEffect {
        PacketHeader    header;                 // type = 0x51
        uint8_t         effect_type;
        int16_t         pos_x;                  // 3D audio positioning
        int16_t         pos_y;
        uint8_t         volume;                 // 0-255
        uint8_t         pitch;                  // Pitch modifier (100 = normal)
    };
    // Size: 19 bytes

    // Server -> Client: Spawn particle system
    struct ParticleSpawn {
        PacketHeader    header;                 // type = 0x52
        uint16_t        particle_system_id;     // Particle system type
        int16_t         pos_x;
        int16_t         pos_y;
        int16_t         velocity_x;             // Initial velocity
        int16_t         velocity_y;
        uint16_t        particle_count;         // Number of particles
        uint16_t        lifetime_ms;            // Particle lifetime
        uint8_t         color_start_r;          // Start color
        uint8_t         color_start_g;
        uint8_t         color_start_b;
        uint8_t         color_end_r;            // End color (for fade)
        uint8_t         color_end_g;
        uint8_t         color_end_b;
    };
    // Size: 32 bytes

    // ============================================================================
    // GAME SYSTEMS (R-Type specific mechanics)
    // ============================================================================

    enum class ForceAttachmentPoint : uint8_t {
        FORCE_DETACHED            = 0x00,
        FORCE_FRONT               = 0x01,
        FORCE_BACK                = 0x02,
        FORCE_ORBITING            = 0x03  // For special modes
    };

    enum class AIBehaviorType : uint8_t {
        AI_IDLE                   = 0x00,
        AI_PATROL                 = 0x01,
        AI_CHASE                  = 0x02,
        AI_FLEE                   = 0x03,
        AI_ATTACK_PATTERN_1       = 0x04,  // Straight line
        AI_ATTACK_PATTERN_2       = 0x05,  // Sine wave
        AI_ATTACK_PATTERN_3       = 0x06,  // Circle
        AI_BOSS_PHASE_1           = 0x07,
        AI_BOSS_PHASE_2           = 0x08,
        AI_BOSS_PHASE_3           = 0x09,
        AI_KAMIKAZE               = 0x0A
        // Add more patterns as needed
    };

    // Server -> Client: Update Force (R-Type signature weapon) state
    struct ForceState {
        PacketHeader    header;                 // type = 0x64
        uint32_t        force_entity_id;        // The Force entity
        uint32_t        parent_ship_id;         // Ship it's attached to (0 = detached)
        uint8_t         attachment_point;       // Where it's attached
        uint8_t         power_level;            // 1-5
        uint8_t         charge_percentage;      // 0-100 for charge beam
        uint8_t         is_firing;              // Boolean flag
    };
    // Size: 24 bytes

    // Server -> Client: Update AI state
    struct AIState {
        PacketHeader    header;                 // type = 0x65
        uint32_t        entity_id;              // AI entity
        uint8_t         current_state;          // Current AI state
        uint8_t         behavior_type;          // Behavior pattern
        uint32_t        target_entity_id;       // Current target (0 = no target)
        int16_t         waypoint_x;             // Next waypoint or target pos
        int16_t         waypoint_y;
        uint16_t        state_timer;            // Time remaining in current state
    };
    // Size: 30 bytes

    // ============================================================================
    // HELPER STRUCTURES
    // ============================================================================

    /**
     * Structure helper pour construire les snapshots
     * Utilisé côté serveur pour itérer efficacement sur les composants
     */
    struct ComponentSnapshotEntry {
        uint32_t  entity_id;
        uint8_t   component_data[32];  // Max component size (ajuster selon besoin)
    };

    /**
     * Configuration des fréquences d'update par composant
     * À ajuster selon les besoins du jeu
     */
    struct ComponentUpdateFrequencies {
        static constexpr uint8_t TRANSFORM_HZ   = 60;  // Critique : mouvement fluide
        static constexpr uint8_t VELOCITY_HZ    = 30;  // Moyen : prédiction possible
        static constexpr uint8_t HEALTH_HZ      = 20;  // Bas : changements rares
        static constexpr uint8_t WEAPON_HZ      = 10;  // Bas : changements rares
        static constexpr uint8_t AI_HZ          = 5;   // Très bas : prédictible
        static constexpr uint8_t ANIMATION_HZ   = 15;  // Moyen : visible mais interpolable
    };

    /**
     * Masque de bits pour tracking des composants "dirty"
     * Utilisé pour les delta snapshots
     */
    struct ComponentDirtyFlags {
        static constexpr uint16_t TRANSFORM     = 0x0001;
        static constexpr uint16_t VELOCITY      = 0x0002;
        static constexpr uint16_t HEALTH        = 0x0004;
        static constexpr uint16_t WEAPON        = 0x0008;
        static constexpr uint16_t AI            = 0x0010;
        static constexpr uint16_t FORCE         = 0x0020;
        static constexpr uint16_t COLLISION     = 0x0040;
        static constexpr uint16_t SPRITE        = 0x0080;
        static constexpr uint16_t ANIMATION     = 0x0100;
        static constexpr uint16_t POWERUP       = 0x0200;
        static constexpr uint16_t SCORE         = 0x0400;
        static constexpr uint16_t INPUT         = 0x0800;
        static constexpr uint16_t PHYSICS       = 0x1000;
        static constexpr uint16_t LIFETIME      = 0x2000;
        static constexpr uint16_t PARENT        = 0x4000;
    };

    #pragma pack(pop)
}

#endif // PROTOCOL_HPP_