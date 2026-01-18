#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <common/protocol/Packet.hpp>

namespace protocol {

    // Using PacketHeader from Packet.hpp
    using PacketHeader = common::protocol::PacketHeader;

    // Packet Type
    enum class PacketTypes : uint8_t {
        //CONNECTION          = 0x01-0x0F
        TYPE_CLIENT_CONNECT         = 0x01,
        TYPE_SERVER_ACCEPT          = 0x02,
        TYPE_SERVER_REJECT          = 0x03,
        TYPE_CLIENT_DISCONNECT      = 0x04,
        TYPE_HEARTBEAT              = 0x05,

        //INPUT               = 0x10-0x1F 
        TYPE_PLAYER_INPUT = 0x10,

        //WORLD_STATE         = 0x20-0x3F
        TYPE_ENTITY_SPAWN           = 0x21,
        TYPE_ENTITY_DESTROY         = 0x22,

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
        TYPE_TRANSFORM_SNAPSHOT     = 0x24,
        TYPE_VELOCITY_SNAPSHOT      = 0x25,
        TYPE_HEALTH_SNAPSHOT        = 0x26,
        TYPE_WEAPON_SNAPSHOT        = 0x27,
        TYPE_AI_SNAPSHOT            = 0x28,
        TYPE_ANIMATION_SNAPSHOT     = 0x29,
        TYPE_COMPONENT_ADD          = 0x2A,
        TYPE_COMPONENT_REMOVE       = 0x2B,
        TYPE_TRANSFORM_SNAPSHOT_DELTA = 0x2C,
        TYPE_HEALTH_SNAPSHOT_DELTA  = 0x2D,
        TYPE_ENTITY_FULL_STATE      = 0x2E,

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
        FLAG_RELIABLE               = 0x01,
        FLAG_COMPRESSED             = 0x02,
        FLAG_ENCRYPTED              = 0x04,
        FLAG_FRAGMENTED             = 0x08,
        FLAG_PRIORITY               = 0x10,
        FLAG_RESERVED_5             = 0x20,
        FLAG_RESERVED_6             = 0x40,
        FLAG_RESERVED_7             = 0x80
    };

    // Entity Types
    enum class EntityTypes : uint8_t {
        ENTITY_TYPE_PLAYER             = 0x01,
        ENTITY_TYPE_ENEMY              = 0x02,
        ENTITY_TYPE_ENEMY_BOSS         = 0x03,
        ENTITY_TYPE_PROJECTILE_PLAYER  = 0x04,
        ENTITY_TYPE_PROJECTILE_ENEMY   = 0x05,
        ENTITY_TYPE_POWERUP            = 0x06,
        ENTITY_TYPE_OBSTACLE           = 0x07,
        ENTITY_TYPE_BG_ELEMENT         = 0x08
    };

    // Game Control Structures
    struct LevelComplete {
        PacketHeader    header;
        uint8_t         completed_level;
        uint8_t         next_level;
        uint32_t        bonus_score;
        uint16_t        completion_time;
    };

    struct LevelStart {
        PacketHeader    header;
        uint8_t         level_id;
        char            level_name[32];
        uint16_t        estimated_duration;
    };

    enum class AudioEffectType : uint8_t {
        SFX_SHOOT_BASIC            = 0x00,
        SFX_SHOOT_CHARGED          = 0x01,
        SFX_SHOOT_LASER            = 0x02,
        SFX_EXPLOSION_SMALL_1      = 0x03,
        SFX_EXPLOSION_SMALL_2      = 0x04,
        SFX_EXPLOSION_LARGE_1      = 0x05,
        SFX_EXPLOSION_LARGE_2      = 0x06,
        SFX_POWERUP_COLLECT_1      = 0x07,
        SFX_POWERUP_COLLECT_2      = 0x08,
        SFX_PLAYER_HIT             = 0x09,
        SFX_PLAYER_DEATH_1         = 0x0A,
        SFX_PLAYER_DEATH_2         = 0x0B,
        SFX_PLAYER_DEATH_3         = 0x0C,
        SFX_FORCE_ATTACH           = 0x0D,
        SFX_FORCE_DETACH           = 0x0E,
        SFX_BOSS_ROAR              = 0x0F,
        SFX_MENU_SELECT            = 0x10,
        SFX_MENU_ALERT             = 0x11
    };

} // namespace protocol

#endif // PROTOCOL_HPP_
