#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <cstdint>
#include <string>

namespace protocol {

    // Packet Header
    struct PacketHeader {
        uint16_t    magic = 0x5254;                     // Protocol identifier 'RT' for R-Type
        uint8_t     packet_type;                        // Packet type identifier
        uint8_t     flags;                              // Control flags
        uint32_t    sequence_number;                    // Monotonic sequence nuumber
        uint32_t    timestamp;                          // Milisec since connection
    };
    // total size : 12 bytes

    //Packet Type
    enum class PacketTypes : uint8_t {

        //CONNECTION          = 0x01-0x0F
        TYPE_CLIENT_CONNECT         = 0x01
        TYPE_SERVER_ACCEPT          = 0x02
        TYPE_SERVER_REJECT          = 0x03
        TYPE_CLIENT_DISCONNECT      = 0x04
        TYPE_HEARTBEAT              = 0x05

        //INPUT               = 0x10-0x1F 
        TYPE_PLAYER_INPUT = 0x10

        //WORLD_STATE         = 0x20-0x3F 
        TYPE_WORLD_SNAPSHOT         = 0x20
        TYPE_ENTITY_SPAWN           = 0x21
        TYPE_ENTITY_DESTROY         = 0x22
        TYPE_ENTITY_UPDATE          = 0x23

        //GAME_EVENTS                 = 0x40-0x5F 
        TYPE_PLAYER_HIT             = 0x40
        TYPE_PLAYER_DEATH           = 0x41
        TYPE_SCORE_UPDATE           = 0x42
        TYPE_POWER_PICKUP           = 0x43
        TYPE_WEAPON_FIRE            = 0x44

        //GAME_CONTROL                = 0x60-0x6F 
        TYPE_GAME_START             = 0x60
        TYPE_GAME_END               = 0x61
        TYPE_LEVEL_COMPLETE         = 0x62
        TYPE_LEVEL_START            = 0x63

        //PROTOCOL_CONTROL            = 0x70-0x7F
        TYPE_ACK                    = 0x70
        TYPE_PING                   = 0x71
        TYPE_PONG                   = 0x72
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

    // Client -> Server or Server -> Client
    struct ClientDisconnect {
        protocol::PacketHeader header;                 // type = 0x04 + FLAG_RELIABLE
        uint32_t               player_id;              // Unique player identifier
        uint8_t                reason;                 // Reason for disconnection
    };
    // total size: 8 bytes

    enum class DisconnectReasons : uint8_t {
        REASON_NORMAL_DISCONNECT            = 0x00,    // Client initiated disconnect
        REASON_TIMEOUT                      = 0x01,    // Server initiated disconnect
        REASON_KICKED_BY_SERVER             = 0x02,    // Kicked by server admin
        REASON_CLIENT_ERROR                 = 0x03,    // Client error
        REASON_GENERIC_ERROR                = 0xFF     // Generic error
        // Add here if we need
    };

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

    // Server -> Client
    struct EntityState {
        uint32_t   entity_id;                          // Unique entity identifier
        uint8_t    entity_type;                        // Entity type
        uint16_t   position_x;                         // X position
        uint16_t   position_y;                         // Y position
        uint16_t   velocity_x;                         // X velocity
        uint16_t   velocity_y;                         // Y velocity
        uint8_t   health;                             // Current health
        uint8_t    state_flags;                        // State flags (e.g., alive, active)
    };
    // total size: 16 bytes

    // Server -> Client
    struct WorldSnapshot {
        protocol::PacketHeader header;                // type = 0x20
        uint32_t               world_tick;            // Server world tick number
        uint16_t               entity_count;          // Number of entities in the snapshot
        EntityState            entities[];           // Variable length array
    };
    // total size: 18 + (entity_count * 16) bytes

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
    };
    // total size: 26 bytes

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

    // Server -> Client
    struct EntityUpdate {
        PacketHeader   header;                      // type = 0 x23
        uint32_t       entity_id;                   // Entity to update
        uint8_t        update_flags;                // Which fields are updated
        int16_t        pos_x;                       // Updated X position ( if flag set )
        int16_t        pos_y;                       // Updated Y position ( if flag set )
        uint8_t        health;                      // Updated health ( if flag set )
        uint8_t        shield;                      // Updated shield ( if flag set )
        uint8_t        state_flags;                 // Updated state ( if flag set )
        int16_t        velocity_x;                  // Updated X velocity ( if flag set )
        int16_t        velocity_y;                  // Updated Y velocity ( if flag set )
    };
    // Total size : 26 bytes

    enum class EntityUpdateFlags : uint8_t {
        // Bits 0-4
        UPDATE_POS            = 0x01,
        UPDATE_HEALTH         = 0x04,
        UPDATE_SHIELD         = 0x08,
        UPDATE_STATE_FLAGS    = 0x10,
        UPDATE_VELOCITY       = 0x20
        // Bits 5-7 reserved for future use
    };

    // Server -> Client
    struct PlayerHit {
        PacketHeader   header;                  // type = 0x40, FLAG_RELIABLE
        uint32_t       player_id;               // Player who was hit
        uint32_t       attacker_id;             // Entity that caused damage
        uint8_t        damage;                  // Damage amount
        uint8_t        remaining_health;        // Health after damage
        uint8_t        remaining_shield;        // Shield after damage
        int16_t        hit_pos_x;               // Hit location X
        int16_t        hit_pos_y;               // Hit location Y
    };
    // Total size: 29 bytes

    // Server -> Client
    struct PlayerDeath {
        PacketHeader    header;                 // type = 0x41, FLAG_RELIABLE
        uint32_t        player_id;              // Player who died
        uint32_t        killer_id;              // Entity that killed player
        uint32_t        score_before_death;     // Player's score
        int16_t         death_pos_x;            // Death location X
        int16_t         death_pos_y;            // Death location Y
    };
    // Total size: 30 bytes

    // Server -> Client
    struct ScoreUpdate {
        PacketHeader    header;                 // type = 0x42
        uint32_t        player_id;              // Player whose score changed
        uint32_t        new_score;              // Updated total score
        int16_t         score_delta;            // Change in score (can be negative)
        uint8_t         reason;                 // Reason for score change
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
        PacketHeader    header;                 // type = 0x43, FLAG_RELIABLE
        uint32_t        player_id;              // Player who picked up powerup
        uint32_t        powerup_id;             // Powerup entity ID
        uint8_t         powerup_type;           // Type of powerup
        uint8_t         duration;               // Effect duration (seconds, 0=permanent)
    };
    // Total size: 22 bytes

    enum class PowerupTypes : uint8_t {
        POWERUP_SPEED_BOOST        = 0x00,
        POWERUP_WEAPON_UPGRADE       = 0x01,
        POWERUP_FORCE                = 0x02,    // R-Type signature
        POWERUP_SHIELD               = 0x03,
        POWERUP_EXTRA_LIFE           = 0x04,
        POWERUP_INVINCIBILITY        = 0x05
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
    // Total size: 31 bytes

    enum class WeaponTypes : uint8_t {
        WEAPON_TYPE_BASIC          = 0x00,
        WEAPON_TYPE_Charged        = 0x01,
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
    // Total size: 36 bytes

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
    // Total size: 22 bytes

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
    };
    // Total size: 20 bytes

    struct Ping {
        PacketHeader    header;                 // type = 0x71
        uint32_t        client_timestamp;       // Client's current timestamp
    };
    // Total size: 16 bytes

    struct Pong {
        PacketHeader    header;                // type = 0x72
        uint32_t        client_timestamp;      // Original client timestamp from PING
        uint32_t        server_timestamp;      // Server's timestamp when received
    };
    // Total size: 20 bytes
}