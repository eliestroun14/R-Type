/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** NetworkManager Create Functions Tests
*/

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <engine/gameEngine/coordinator/network/PacketManager.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>

using namespace protocol;

// ============================================================================
// Test Fixtures and Helpers
// ============================================================================

class NetworkManagerCreateTest : public ::testing::Test {
protected:
    /**
     * Helper to create args buffer with flags, sequence, timestamp, and payload
     */
    std::vector<uint8_t> createArgsBuffer(
        uint8_t flags_count,
        const std::vector<uint8_t>& flags,
        uint32_t sequence_number,
        uint32_t timestamp,
        const std::vector<uint8_t>& payload)
    {
        // Calculate total size needed
        size_t total_size = 1 + flags_count + 4 + 4 + payload.size();
        std::vector<uint8_t> args;
        args.reserve(total_size);
        
        // Add flags_count
        args.push_back(flags_count);
        
        // Add flags
        for (uint8_t flag : flags) {
            args.push_back(flag);
        }
        
        // Add sequence_number (little-endian)
        appendVector(args, uint32ToBytes(sequence_number));
        
        // Add timestamp (little-endian)
        appendVector(args, uint32ToBytes(timestamp));
        
        // Add payload
        appendVector(args, payload);
        
        return args;
    }

    /**
     * Helper to create uint32_t bytes
     */
    std::vector<uint8_t> uint32ToBytes(uint32_t value) {
        std::vector<uint8_t> bytes(4);
        std::memcpy(bytes.data(), &value, 4);
        return bytes;
    }

    /**
     * Helper to create uint16_t bytes
     */
    std::vector<uint8_t> uint16ToBytes(uint16_t value) {
        std::vector<uint8_t> bytes(2);
        std::memcpy(bytes.data(), &value, 2);
        return bytes;
    }

    /**
     * Helper to create float bytes
     */
    std::vector<uint8_t> floatToBytes(float value) {
        std::vector<uint8_t> bytes(4);
        std::memcpy(bytes.data(), &value, 4);
        return bytes;
    }

    /**
     * Helper to append vector to another vector
     */
    void appendVector(std::vector<uint8_t> &dest, const std::vector<uint8_t> &src) {
        dest.insert(dest.end(), src.begin(), src.end());
    }
};

// ============================================================================
// CONNECTION TESTS (0x01-0x0F)
// ============================================================================

class CreateClientConnectTest : public NetworkManagerCreateTest {
};

TEST_F(CreateClientConnectTest, ValidClientConnect) {
    std::vector<uint8_t> payload;
    payload.push_back(PROTOCOL_VERSION); // protocol_version
    payload.insert(payload.end(), 32, 0); // player_name (32 bytes)
    std::string name = "TestPlayer";
    std::memcpy(payload.data() + 1, name.c_str(), name.length());
    appendVector(payload, uint32ToBytes(12345)); // client_id

    auto args = createArgsBuffer(1, {0x01}, 100, 200, payload);
    auto result = PacketManager::createClientConnect(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.magic, 0x5254);
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT));
    EXPECT_EQ(result->header.flags, 0x01);
    EXPECT_EQ(result->header.sequence_number, 100);
    EXPECT_EQ(result->header.timestamp, 200);
    EXPECT_EQ(result->data.size(), CLIENT_CONNECT_PAYLOAD_SIZE);
}

TEST_F(CreateClientConnectTest, InvalidArgsSize) {
    auto args = createArgsBuffer(0, {}, 100, 200, std::vector<uint8_t>(5)); // Too small payload
    auto result = PacketManager::createClientConnect(args);

    EXPECT_FALSE(result.has_value());
}

TEST_F(CreateClientConnectTest, MultipleFlags) {
    std::vector<uint8_t> payload(CLIENT_CONNECT_PAYLOAD_SIZE, 0);
    auto args = createArgsBuffer(3, {0x01, 0x02, 0x04}, 100, 200, payload);
    auto result = PacketManager::createClientConnect(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.flags, 0x01 | 0x02 | 0x04); // Combined flags
}

class CreateServerAcceptTest : public NetworkManagerCreateTest {
};

TEST_F(CreateServerAcceptTest, ValidServerAccept) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(42)); // assigned_player_id
    payload.push_back(4); // max_players
    appendVector(payload, uint32ToBytes(999)); // game_instance_id
    appendVector(payload, uint16ToBytes(60)); // server_tickrate

    auto args = createArgsBuffer(1, {0x01}, 101, 201, payload);
    auto result = PacketManager::createServerAccept(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_SERVER_ACCEPT));
    EXPECT_EQ(result->data.size(), SERVER_ACCEPT_PAYLOAD_SIZE);
}

TEST_F(CreateServerAcceptTest, InvalidArgsSize) {
    auto args = createArgsBuffer(0, {}, 101, 201, std::vector<uint8_t>(5)); // Too small
    auto result = PacketManager::createServerAccept(args);

    EXPECT_FALSE(result.has_value());
}

class CreateServerRejectTest : public NetworkManagerCreateTest {
};

TEST_F(CreateServerRejectTest, ValidServerReject) {
    std::vector<uint8_t> payload;
    payload.push_back(1); // reject_code
    std::string reason = "Server full";
    payload.insert(payload.end(), reason.begin(), reason.end());
    payload.insert(payload.end(), 64 - reason.length(), 0); // Pad to 64 bytes

    auto args = createArgsBuffer(1, {0x01}, 102, 202, payload);
    auto result = PacketManager::createServerReject(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_SERVER_REJECT));
    EXPECT_EQ(result->data.size(), SERVER_REJECT_PAYLOAD_SIZE);
}

class CreateClientDisconnectTest : public NetworkManagerCreateTest {
};

TEST_F(CreateClientDisconnectTest, ValidClientDisconnect) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(42)); // player_id
    payload.push_back(0); // reason

    auto args = createArgsBuffer(1, {0x01}, 103, 203, payload);
    auto result = PacketManager::createClientDisconnect(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_DISCONNECT));
}

class CreateHeartBeatTest : public NetworkManagerCreateTest {
};

TEST_F(CreateHeartBeatTest, ValidHeartBeat) {
    std::vector<uint8_t> payload = uint32ToBytes(42); // player_id

    auto args = createArgsBuffer(1, {0x01}, 104, 204, payload);
    auto result = PacketManager::createHeartBeat(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_HEARTBEAT));
}

// ============================================================================
// INPUT TESTS (0x10-0x1F)
// ============================================================================

class CreatePlayerInputTest : public NetworkManagerCreateTest {
};

TEST_F(CreatePlayerInputTest, ValidPlayerInput) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(42)); // player_id
    appendVector(payload, uint16ToBytes(0x0F)); // input_state
    appendVector(payload, uint16ToBytes(100)); // aim_direction_x
    appendVector(payload, uint16ToBytes(200)); // aim_direction_y

    auto args = createArgsBuffer(1, {0x01}, 110, 210, payload);
    auto result = PacketManager::createPlayerInput(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_PLAYER_INPUT));
}

// ============================================================================
// WORLD_STATE TESTS (0x20-0x3F)
// ============================================================================

class CreateEntitySpawnTest : public NetworkManagerCreateTest {
};

TEST_F(CreateEntitySpawnTest, ValidEntitySpawn) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(1); // entity_type
    appendVector(payload, uint16ToBytes(500)); // position_x
    appendVector(payload, uint16ToBytes(400)); // position_y
    payload.push_back(0); // mob_variant
    payload.push_back(100); // initial_health
    appendVector(payload, uint16ToBytes(10)); // initial_velocity_x
    appendVector(payload, uint16ToBytes(20)); // initial_velocity_y

    auto args = createArgsBuffer(1, {0x01}, 120, 220, payload);
    auto result = PacketManager::createEntitySpawn(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_ENTITY_SPAWN));
}

class CreateEntityDestroyTest : public NetworkManagerCreateTest {
};

TEST_F(CreateEntityDestroyTest, ValidEntityDestroy) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(0); // destroy_reason
    appendVector(payload, uint16ToBytes(500)); // final_position_x
    appendVector(payload, uint16ToBytes(400)); // final_position_y

    auto args = createArgsBuffer(1, {0x01}, 121, 221, payload);
    auto result = PacketManager::createEntityDestroy(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_ENTITY_DESTROY));
}

// ============================================================================
// ECS SNAPSHOT TESTS (0x30-0x3F)
// ============================================================================

class CreateTransformSnapshotTest : public NetworkManagerCreateTest {
};

TEST_F(CreateTransformSnapshotTest, ValidTransformSnapshot) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // world_tick
    payload.push_back(1); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    appendVector(payload, uint16ToBytes(500)); // x
    appendVector(payload, uint16ToBytes(400)); // y

    auto args = createArgsBuffer(1, {0x01}, 130, 230, payload);
    auto result = PacketManager::createTransformSnapshot(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_TRANSFORM_SNAPSHOT));
}

class CreateVelocitySnapshotTest : public NetworkManagerCreateTest {
};

TEST_F(CreateVelocitySnapshotTest, ValidVelocitySnapshot) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // world_tick
    payload.push_back(1); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    appendVector(payload, uint16ToBytes(10)); // vx
    appendVector(payload, uint16ToBytes(20)); // vy

    auto args = createArgsBuffer(1, {0x01}, 131, 231, payload);
    auto result = PacketManager::createVelocitySnapshot(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_VELOCITY_SNAPSHOT));
}

class CreateHealthSnapshotTest : public NetworkManagerCreateTest {
};

TEST_F(CreateHealthSnapshotTest, ValidHealthSnapshot) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // world_tick
    payload.push_back(1); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(100); // health

    auto args = createArgsBuffer(1, {0x01}, 132, 232, payload);
    auto result = PacketManager::createHealthSnapshot(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_HEALTH_SNAPSHOT));
}

// ============================================================================
// GAME_EVENTS TESTS (0x40-0x5F)
// ============================================================================

class CreatePlayerHitTest : public NetworkManagerCreateTest {
};

TEST_F(CreatePlayerHitTest, ValidPlayerHit) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // player_id
    appendVector(payload, uint32ToBytes(2)); // attacker_id
    payload.push_back(25); // damage
    payload.push_back(50); // remaining_health
    payload.push_back(0); // remaining_shield
    appendVector(payload, uint16ToBytes(500)); // hit_pos_x
    appendVector(payload, uint16ToBytes(400)); // hit_pos_y

    auto args = createArgsBuffer(1, {0x01}, 140, 240, payload);
    auto result = PacketManager::createPlayerHit(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_PLAYER_HIT));
}

class CreatePlayerDeathTest : public NetworkManagerCreateTest {
};

TEST_F(CreatePlayerDeathTest, ValidPlayerDeath) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // player_id
    appendVector(payload, uint32ToBytes(2)); // killer_id
    appendVector(payload, uint32ToBytes(1000)); // score_before_death
    appendVector(payload, uint16ToBytes(500)); // death_pos_x
    appendVector(payload, uint16ToBytes(400)); // death_pos_y

    auto args = createArgsBuffer(1, {0x01}, 141, 241, payload);
    auto result = PacketManager::createPlayerDeath(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_PLAYER_DEATH));
}

class CreateScoreUpdateTest : public NetworkManagerCreateTest {
};

TEST_F(CreateScoreUpdateTest, ValidScoreUpdate) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // player_id
    appendVector(payload, uint32ToBytes(1000)); // new_score
    appendVector(payload, uint16ToBytes(100)); // score_delta
    payload.push_back(0); // reason

    auto args = createArgsBuffer(1, {0x01}, 142, 242, payload);
    auto result = PacketManager::createScoreUpdate(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_SCORE_UPDATE));
}

class CreatePowerupPickupTest : public NetworkManagerCreateTest {
};

TEST_F(CreatePowerupPickupTest, ValidPowerupPickup) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // player_id
    appendVector(payload, uint32ToBytes(100)); // powerup_id
    payload.push_back(1); // powerup_type
    payload.push_back(0); // duration

    auto args = createArgsBuffer(1, {0x01}, 143, 243, payload);
    auto result = PacketManager::createPowerupPickup(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_POWER_PICKUP));
}

class CreateWeaponFireTest : public NetworkManagerCreateTest {
};

TEST_F(CreateWeaponFireTest, ValidWeaponFire) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // shooter_id
    appendVector(payload, uint32ToBytes(100)); // projectile_id
    appendVector(payload, uint16ToBytes(500)); // origin_x
    appendVector(payload, uint16ToBytes(400)); // origin_y
    appendVector(payload, uint16ToBytes(100)); // direction_x
    appendVector(payload, uint16ToBytes(0)); // direction_y
    payload.push_back(0); // weapon_type

    auto args = createArgsBuffer(1, {0x01}, 144, 244, payload);
    auto result = PacketManager::createWeaponFire(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_WEAPON_FIRE));
}

class CreateVisualEffectTest : public NetworkManagerCreateTest {
};

TEST_F(CreateVisualEffectTest, ValidVisualEffect) {
    std::vector<uint8_t> payload;
    payload.push_back(1); // effect_type
    appendVector(payload, uint16ToBytes(500)); // pos_x
    appendVector(payload, uint16ToBytes(400)); // pos_y
    appendVector(payload, uint16ToBytes(1000)); // duration_ms
    payload.push_back(255); // scale
    payload.push_back(255); // color_tint_r
    payload.push_back(0); // color_tint_g
    payload.push_back(0); // color_tint_b

    auto args = createArgsBuffer(1, {0x01}, 145, 245, payload);
    auto result = PacketManager::createVisualEffect(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_VISUAL_EFFECT));
}

class CreateAudioEffectTest : public NetworkManagerCreateTest {
};

TEST_F(CreateAudioEffectTest, ValidAudioEffect) {
    std::vector<uint8_t> payload;
    payload.push_back(0); // effect_type
    appendVector(payload, uint16ToBytes(500)); // pos_x
    appendVector(payload, uint16ToBytes(400)); // pos_y
    payload.push_back(128); // volume
    payload.push_back(100); // pitch

    auto args = createArgsBuffer(1, {0x01}, 146, 246, payload);
    auto result = PacketManager::createAudioEffect(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_AUDIO_EFFECT));
}

class CreateParticleSpawnTest : public NetworkManagerCreateTest {
};

TEST_F(CreateParticleSpawnTest, ValidParticleSpawn) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint16ToBytes(1)); // particle_system_id
    appendVector(payload, uint16ToBytes(500)); // pos_x
    appendVector(payload, uint16ToBytes(400)); // pos_y
    appendVector(payload, uint16ToBytes(10)); // velocity_x
    appendVector(payload, uint16ToBytes(20)); // velocity_y
    appendVector(payload, uint16ToBytes(100)); // particle_count
    appendVector(payload, uint16ToBytes(5000)); // lifetime_ms
    payload.push_back(255); // color_start_r
    payload.push_back(128); // color_start_g
    payload.push_back(0); // color_start_b
    payload.push_back(128); // color_end_r
    payload.push_back(64); // color_end_g
    payload.push_back(0); // color_end_b

    auto args = createArgsBuffer(1, {0x01}, 147, 247, payload);
    auto result = PacketManager::createParticleSpawn(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_PARTICLE_SPAWN));
}

// ============================================================================
// GAME_CONTROL TESTS (0x60-0x6F)
// ============================================================================

class CreateGameStartTest : public NetworkManagerCreateTest {
};

TEST_F(CreateGameStartTest, ValidGameStart) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // game_instance_id
    payload.push_back(4); // player_count
    appendVector(payload, uint32ToBytes(10)); // player_ids[0]
    appendVector(payload, uint32ToBytes(20)); // player_ids[1]
    appendVector(payload, uint32ToBytes(30)); // player_ids[2]
    appendVector(payload, uint32ToBytes(40)); // player_ids[3]
    payload.push_back(1); // level_id
    payload.push_back(1); // difficulty

    auto args = createArgsBuffer(1, {0x01}, 160, 260, payload);
    auto result = PacketManager::createGameStart(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_GAME_START));
}

class CreateGameEndTest : public NetworkManagerCreateTest {
};

TEST_F(CreateGameEndTest, ValidGameEnd) {
    std::vector<uint8_t> payload;
    payload.push_back(0); // end_reason
    appendVector(payload, uint32ToBytes(1000)); // final_scores[0]
    appendVector(payload, uint32ToBytes(500)); // final_scores[1]
    appendVector(payload, uint32ToBytes(200)); // final_scores[2]
    appendVector(payload, uint32ToBytes(0)); // final_scores[3]
    payload.push_back(1); // winner_id
    appendVector(payload, uint32ToBytes(300)); // play_time

    auto args = createArgsBuffer(1, {0x01}, 161, 261, payload);
    auto result = PacketManager::createGameEnd(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_GAME_END));
}

class CreateLevelCompleteTest : public NetworkManagerCreateTest {
};

TEST_F(CreateLevelCompleteTest, ValidLevelComplete) {
    std::vector<uint8_t> payload;
    payload.push_back(1); // completed_level
    payload.push_back(2); // next_level
    appendVector(payload, uint32ToBytes(1000)); // bonus_score
    appendVector(payload, uint16ToBytes(120)); // completion_time

    auto args = createArgsBuffer(1, {0x01}, 162, 262, payload);
    auto result = PacketManager::createLevelComplete(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_LEVEL_COMPLETE));
}

class CreateLevelStartTest : public NetworkManagerCreateTest {
};

TEST_F(CreateLevelStartTest, ValidLevelStart) {
    std::vector<uint8_t> payload;
    payload.push_back(1); // level_id
    payload.insert(payload.end(), 32, 0); // level_name (32 bytes)
    std::string name = "Level1";
    std::memcpy(payload.data() + 1, name.c_str(), name.length());
    appendVector(payload, uint16ToBytes(120)); // estimated_duration

    auto args = createArgsBuffer(1, {0x01}, 163, 263, payload);
    auto result = PacketManager::createLevelStart(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_LEVEL_START));
}

class CreateForceStateTest : public NetworkManagerCreateTest {
};

TEST_F(CreateForceStateTest, ValidForceState) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // force_entity_id
    appendVector(payload, uint32ToBytes(10)); // parent_ship_id
    payload.push_back(0); // attachment_point
    payload.push_back(5); // power_level
    payload.push_back(75); // charge_percentage
    payload.push_back(1); // is_firing

    auto args = createArgsBuffer(1, {0x01}, 164, 264, payload);
    auto result = PacketManager::createForceState(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_FORCE_STATE));
}

class CreateAIStateTest : public NetworkManagerCreateTest {
};

TEST_F(CreateAIStateTest, ValidAIState) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // entity_id
    payload.push_back(0); // current_state
    payload.push_back(1); // behavior_type
    appendVector(payload, uint32ToBytes(2)); // target_entity_id
    appendVector(payload, uint16ToBytes(100)); // waypoint_x
    appendVector(payload, uint16ToBytes(200)); // waypoint_y
    appendVector(payload, uint16ToBytes(5000)); // state_timer

    auto args = createArgsBuffer(1, {0x01}, 165, 265, payload);
    auto result = PacketManager::createAIState(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_AI_STATE));
}

// ============================================================================
// PROTOCOL_CONTROL TESTS (0x70-0x7F)
// ============================================================================

class CreateAcknowledgmentTest : public NetworkManagerCreateTest {
};

TEST_F(CreateAcknowledgmentTest, ValidAcknowledgment) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(99)); // acked_sequence
    appendVector(payload, uint32ToBytes(1000)); // received_timestamp

    auto args = createArgsBuffer(1, {0x01}, 170, 270, payload);
    auto result = PacketManager::createAcknowledgment(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_ACK));
}

class CreatePingTest : public NetworkManagerCreateTest {
};

TEST_F(CreatePingTest, ValidPing) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1234567890)); // client_timestamp

    auto args = createArgsBuffer(1, {0x01}, 171, 271, payload);
    auto result = PacketManager::createPing(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_PING));
}

class CreatePongTest : public NetworkManagerCreateTest {
};

TEST_F(CreatePongTest, ValidPong) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(171)); // client_timestamp
    appendVector(payload, uint32ToBytes(1234567890)); // server_timestamp

    auto args = createArgsBuffer(1, {0x01}, 172, 272, payload);
    auto result = PacketManager::createPong(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_PONG));
}

// ============================================================================
// Additional Component Tests
// ============================================================================

class CreateComponentRemoveTest : public NetworkManagerCreateTest {
};

TEST_F(CreateComponentRemoveTest, ValidComponentRemove) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // entity_id
    payload.push_back(0); // component_type

    auto args = createArgsBuffer(1, {0x01}, 180, 280, payload);
    auto result = PacketManager::createComponentRemove(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_COMPONENT_REMOVE));
}

// ============================================================================
// Additional Snapshot and Component Tests
// ============================================================================

class CreateWeaponSnapshotTest : public NetworkManagerCreateTest {
};

TEST_F(CreateWeaponSnapshotTest, ValidWeaponSnapshot) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // world_tick
    appendVector(payload, uint16ToBytes(1)); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(1); // weapon_type
    payload.push_back(75); // ammo
    appendVector(payload, uint16ToBytes(5000)); // fire_rate

    auto args = createArgsBuffer(1, {0x01}, 150, 250, payload);
    auto result = PacketManager::createWeaponSnapshot(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_WEAPON_SNAPSHOT));
}

class CreateAISnapshotTest : public NetworkManagerCreateTest {
};

TEST_F(CreateAISnapshotTest, ValidAISnapshot) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // world_tick
    appendVector(payload, uint16ToBytes(1)); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(0); // ai_state
    payload.push_back(1); // behavior_type
    appendVector(payload, uint32ToBytes(200)); // target_id
    appendVector(payload, uint16ToBytes(100)); // waypoint_x
    appendVector(payload, uint16ToBytes(200)); // waypoint_y

    auto args = createArgsBuffer(1, {0x01}, 151, 251, payload);
    auto result = PacketManager::createAISnapshot(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_AI_SNAPSHOT));
}

class CreateAnimationSnapshotTest : public NetworkManagerCreateTest {
};

TEST_F(CreateAnimationSnapshotTest, ValidAnimationSnapshot) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // world_tick
    appendVector(payload, uint16ToBytes(1)); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(5); // animation_id
    appendVector(payload, uint16ToBytes(0)); // frame
    appendVector(payload, uint16ToBytes(100)); // animation_speed

    auto args = createArgsBuffer(1, {0x01}, 152, 252, payload);
    auto result = PacketManager::createAnimationSnapshot(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_ANIMATION_SNAPSHOT));
}

class CreateComponentAddTest : public NetworkManagerCreateTest {
};

TEST_F(CreateComponentAddTest, ValidComponentAdd) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(1)); // entity_id
    payload.push_back(0); // component_type
    payload.push_back(4); // data_size
    appendVector(payload, uint16ToBytes(100)); // sample component data

    auto args = createArgsBuffer(1, {0x01}, 181, 281, payload);
    auto result = PacketManager::createComponentAdd(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_COMPONENT_ADD));
}

class CreateTransformSnapshotDeltaTest : public NetworkManagerCreateTest {
};

TEST_F(CreateTransformSnapshotDeltaTest, ValidTransformSnapshotDelta) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(10)); // world_tick
    appendVector(payload, uint32ToBytes(5)); // base_tick
    appendVector(payload, uint16ToBytes(1)); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    appendVector(payload, uint16ToBytes(500)); // x
    appendVector(payload, uint16ToBytes(400)); // y

    auto args = createArgsBuffer(1, {0x01}, 153, 253, payload);
    auto result = PacketManager::createTransformSnapshotDelta(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA));
}

class CreateHealthSnapshotDeltaTest : public NetworkManagerCreateTest {
};

TEST_F(CreateHealthSnapshotDeltaTest, ValidHealthSnapshotDelta) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(10)); // world_tick
    appendVector(payload, uint32ToBytes(5)); // base_tick
    appendVector(payload, uint16ToBytes(1)); // entity_count
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(100); // health

    auto args = createArgsBuffer(1, {0x01}, 154, 254, payload);
    auto result = PacketManager::createHealthSnapshotDelta(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA));
}

class CreateEntityFullStateTest : public NetworkManagerCreateTest {
};

TEST_F(CreateEntityFullStateTest, ValidEntityFullState) {
    std::vector<uint8_t> payload;
    appendVector(payload, uint32ToBytes(100)); // entity_id
    payload.push_back(1); // entity_type
    payload.push_back(1); // component_count
    payload.push_back(0); // component_type
    payload.push_back(4); // component_data_size
    appendVector(payload, uint16ToBytes(100)); // sample component data

    auto args = createArgsBuffer(1, {0x01}, 155, 255, payload);
    auto result = PacketManager::createEntityFullState(args);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_ENTITY_FULL_STATE));
}

// ============================================================================
// Error Handling Tests
// ============================================================================

class CreateErrorHandlingTest : public NetworkManagerCreateTest {
};

TEST_F(CreateErrorHandlingTest, InsufficientFlagsData) {
    // flags_count = 3 but only provide 1 flag byte
    auto args = createArgsBuffer(3, {0x01}, 100, 200, std::vector<uint8_t>(37));
    auto result = PacketManager::createClientConnect(args);

    EXPECT_FALSE(result.has_value());
}

TEST_F(CreateErrorHandlingTest, ZeroFlagsCount) {
    std::vector<uint8_t> payload(CLIENT_CONNECT_PAYLOAD_SIZE, 0);
    auto args = createArgsBuffer(0, {}, 100, 200, payload);
    auto result = PacketManager::createClientConnect(args);

    ASSERT_TRUE(result.has_value()); // Should work with no flags
}

TEST_F(CreateErrorHandlingTest, MaxFlagsCount) {
    std::vector<uint8_t> flags(255, 0x01); // Max possible flags
    std::vector<uint8_t> payload(CLIENT_CONNECT_PAYLOAD_SIZE, 0);
    auto args = createArgsBuffer(255, flags, 100, 200, payload);
    auto result = PacketManager::createClientConnect(args);

    // With 255 flags, total size = 1 + 255 + 4 + 4 + 37 = 301 bytes
    // This should actually succeed since we have all the data
    EXPECT_TRUE(result.has_value());
}

TEST_F(CreateErrorHandlingTest, OptionalReturnType) {
    auto args = createArgsBuffer(1, {0x01}, 100, 200, std::vector<uint8_t>(5)); // Too small
    auto result = PacketManager::createClientConnect(args);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result, std::nullopt);
}

TEST_F(CreateErrorHandlingTest, ValidOptionalReturnType) {
    std::vector<uint8_t> payload(CLIENT_CONNECT_PAYLOAD_SIZE, 0);
    auto args = createArgsBuffer(1, {0x01}, 100, 200, payload);
    auto result = PacketManager::createClientConnect(args);

    EXPECT_TRUE(result.has_value());
    EXPECT_NE(result, std::nullopt);
}
