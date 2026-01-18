/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** NetworkManager Create Functions Tests
*/

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <common/protocol/PacketManager.hpp>
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

TEST_F(CreateClientConnectTest, InvalidArgsSize) {
    auto args = createArgsBuffer(0, {}, 100, 200, std::vector<uint8_t>(5)); // Too small payload
    auto result = PacketManager::createClientConnect(args);

    EXPECT_FALSE(result.has_value());
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

class CreateHeartBeatTest : public NetworkManagerCreateTest {
};

// ============================================================================
// INPUT TESTS (0x10-0x1F)
// ============================================================================

class CreatePlayerInputTest : public NetworkManagerCreateTest {
};

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

class CreatePlayerDeathTest : public NetworkManagerCreateTest {
};

class CreateScoreUpdateTest : public NetworkManagerCreateTest {
};

class CreatePowerupPickupTest : public NetworkManagerCreateTest {
};

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

class CreateAudioEffectTest : public NetworkManagerCreateTest {
};

class CreateParticleSpawnTest : public NetworkManagerCreateTest {
};

// ============================================================================
// GAME_CONTROL TESTS (0x60-0x6F)
// ============================================================================

class CreateGameStartTest : public NetworkManagerCreateTest {
};

class CreateGameEndTest : public NetworkManagerCreateTest {
};

class CreateLevelCompleteTest : public NetworkManagerCreateTest {
};

class CreateLevelStartTest : public NetworkManagerCreateTest {
};

class CreateForceStateTest : public NetworkManagerCreateTest {
};

class CreateAIStateTest : public NetworkManagerCreateTest {
};

// ============================================================================
// PROTOCOL_CONTROL TESTS (0x70-0x7F)
// ============================================================================

class CreateAcknowledgmentTest : public NetworkManagerCreateTest {
};

class CreatePingTest : public NetworkManagerCreateTest {
};



class CreatePongTest : public NetworkManagerCreateTest {
};

// ============================================================================
// Additional Component Tests
// ============================================================================

class CreateComponentRemoveTest : public NetworkManagerCreateTest {
};

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

TEST_F(CreateErrorHandlingTest, OptionalReturnType) {
    auto args = createArgsBuffer(1, {0x01}, 100, 200, std::vector<uint8_t>(5)); // Too small
    auto result = PacketManager::createClientConnect(args);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result, std::nullopt);
}

