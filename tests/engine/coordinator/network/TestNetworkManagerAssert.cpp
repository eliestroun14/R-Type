/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** NetworkManager Assert Functions Tests
*/

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include "engine/gameEngine/coordinator/network/NetworkManager.hpp"
#include "common/protocol/Packet.hpp"
#include "common/protocol/Protocol.hpp"

using namespace protocol;

// ============================================================================
// Test Fixtures
// ============================================================================

class NetworkManagerAssertTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize packet with default header
        packet.header.magic = 0x5254;
        packet.header.flags = 0x01; // FLAG_RELIABLE by default
        packet.header.sequence_number = 1;
        packet.header.timestamp = 0;
    }

    common::protocol::Packet packet;
    NetworkManager nm;

    /**
     * Helper function to set data with exact size
     */
    void setPacketData(const std::vector<uint8_t>& data) {
        packet.data = data;
    }

    /**
     * Helper to create a buffer of given size filled with 0
     */
    std::vector<uint8_t> createBuffer(size_t size) {
        return std::vector<uint8_t>(size, 0);
    }

    /**
     * Helper to set uint32_t at offset
     */
    void setUint32At(std::vector<uint8_t>& buffer, size_t offset, uint32_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(uint32_t));
    }

    /**
     * Helper to set uint16_t at offset
     */
    void setUint16At(std::vector<uint8_t>& buffer, size_t offset, uint16_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(uint16_t));
    }

    /**
     * Helper to set uint8_t at offset
     */
    void setUint8At(std::vector<uint8_t>& buffer, size_t offset, uint8_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(uint8_t));
    }

    /**
     * Helper to set string at offset (null-terminated)
     */
    void setStringAt(std::vector<uint8_t>& buffer, size_t offset, const std::string& str, size_t maxLen) {
        std::memcpy(buffer.data() + offset, str.c_str(), std::min(str.length(), maxLen));
    }

    /**
     * Helper to set int16_t at offset
     */
    void setInt16At(std::vector<uint8_t>& buffer, size_t offset, int16_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(int16_t));
    }
};

// ============================================================================
// assertClientConnect Tests (0x01)
// ============================================================================

class AssertClientConnectTest : public NetworkManagerAssertTest {
};

TEST_F(AssertClientConnectTest, ValidClientConnect) {
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1); // protocol_version = 1
    setStringAt(buffer, 1, "TestPlayer", 31); // player_name
    setUint32At(buffer, 33, 42); // client_id = 42
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertClientConnect(packet));
}

TEST_F(AssertClientConnectTest, InvalidPayloadSize) {
    auto buffer = createBuffer(36); // Wrong size
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientConnect(packet));
}

TEST_F(AssertClientConnectTest, InvalidProtocolVersion) {
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 2); // protocol_version = 2 (invalid)
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 42);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientConnect(packet));
}

TEST_F(AssertClientConnectTest, EmptyPlayerName) {
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    // player_name is empty (all zeros)
    setUint32At(buffer, 33, 42);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientConnect(packet));
}

TEST_F(AssertClientConnectTest, ZeroClientId) {
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 0); // client_id = 0 (invalid)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientConnect(packet));
}

TEST_F(AssertClientConnectTest, PlayerNameMaxLength) {
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    std::string longName(31, 'A'); // Max 31 chars
    setStringAt(buffer, 1, longName, 31);
    setUint32At(buffer, 33, 123);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertClientConnect(packet));
}

// ============================================================================
// assertServerAccept Tests (0x02)
// ============================================================================

class AssertServerAcceptTest : public NetworkManagerAssertTest {
};

TEST_F(AssertServerAcceptTest, ValidServerAccept) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100); // assigned_player_id
    setUint8At(buffer, 4, 2); // max_players = 2
    setUint32At(buffer, 5, 999); // game_instance_id
    setUint16At(buffer, 9, 60); // server_tickrate
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertServerAccept(packet));
}

TEST_F(AssertServerAcceptTest, InvalidPayloadSize) {
    auto buffer = createBuffer(10); // Wrong size
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerAccept(packet));
}

TEST_F(AssertServerAcceptTest, ZeroAssignedPlayerId) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 0); // assigned_player_id = 0 (invalid)
    setUint8At(buffer, 4, 2);
    setUint32At(buffer, 5, 999);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerAccept(packet));
}

TEST_F(AssertServerAcceptTest, MaxPlayersZero) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0); // max_players = 0 (invalid)
    setUint32At(buffer, 5, 999);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerAccept(packet));
}

TEST_F(AssertServerAcceptTest, MaxPlayersAboveLimit) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 5); // max_players = 5 (invalid, max is 4)
    setUint32At(buffer, 5, 999);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerAccept(packet));
}

TEST_F(AssertServerAcceptTest, ZeroGameInstanceId) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 2);
    setUint32At(buffer, 5, 0); // game_instance_id = 0 (invalid)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerAccept(packet));
}

TEST_F(AssertServerAcceptTest, ValidMaxPlayersRange) {
    for (uint8_t maxPlayers = 1; maxPlayers <= 4; ++maxPlayers) {
        auto buffer = createBuffer(11);
        setUint32At(buffer, 0, 100);
        setUint8At(buffer, 4, maxPlayers);
        setUint32At(buffer, 5, 999);
        setPacketData(buffer);

        EXPECT_TRUE(NetworkManager::assertServerAccept(packet))
            << "max_players = " << (int)maxPlayers << " should be valid";
    }
}

// ============================================================================
// assertServerReject Tests (0x03)
// ============================================================================

class AssertServerRejectTest : public NetworkManagerAssertTest {
};

TEST_F(AssertServerRejectTest, ValidServerReject) {
    auto buffer = createBuffer(65);
    setUint8At(buffer, 0, protocol::REJECT_CODE_MIN); // Valid reject code
    setStringAt(buffer, 1, "Connection rejected", 63);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertServerReject(packet));
}

TEST_F(AssertServerRejectTest, InvalidPayloadSize) {
    auto buffer = createBuffer(64); // Wrong size
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerReject(packet));
}

TEST_F(AssertServerRejectTest, EmptyReasonMessage) {
    auto buffer = createBuffer(65);
    setUint8At(buffer, 0, protocol::REJECT_CODE_MIN);
    // reason_message is empty (all zeros)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertServerReject(packet));
}

TEST_F(AssertServerRejectTest, GenericErrorCode) {
    auto buffer = createBuffer(65);
    setUint8At(buffer, 0, protocol::REJECT_CODE_GENERIC_ERROR); // Generic error code
    setStringAt(buffer, 1, "Generic error", 63);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertServerReject(packet));
}

TEST_F(AssertServerRejectTest, MaxLengthReason) {
    auto buffer = createBuffer(65);
    setUint8At(buffer, 0, protocol::REJECT_CODE_MIN);
    std::string longReason(63, 'A');
    setStringAt(buffer, 1, longReason, 63);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertServerReject(packet));
}

// ============================================================================
// assertClientDisconnect Tests (0x04)
// ============================================================================

class AssertClientDisconnectTest : public NetworkManagerAssertTest {
};

TEST_F(AssertClientDisconnectTest, ValidClientDisconnect) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 42); // player_id
    setUint8At(buffer, 4, protocol::DISCONNECT_REASON_MIN); // Valid reason
    packet.header.flags = 0x01; // FLAG_RELIABLE
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertClientDisconnect(packet));
}

TEST_F(AssertClientDisconnectTest, InvalidPayloadSize) {
    auto buffer = createBuffer(4); // Wrong size
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientDisconnect(packet));
}

TEST_F(AssertClientDisconnectTest, ZeroPlayerId) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 0); // player_id = 0 (invalid)
    setUint8At(buffer, 4, protocol::DISCONNECT_REASON_MIN);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientDisconnect(packet));
}

TEST_F(AssertClientDisconnectTest, MissingReliableFlag) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 42);
    setUint8At(buffer, 4, protocol::DISCONNECT_REASON_MIN);
    packet.header.flags = 0x00; // No FLAG_RELIABLE
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertClientDisconnect(packet));
}

TEST_F(AssertClientDisconnectTest, GenericErrorReason) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 42);
    setUint8At(buffer, 4, protocol::DISCONNECT_REASON_GENERIC_ERROR);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertClientDisconnect(packet));
}

// ============================================================================
// assertHeartBeat Tests (0x05)
// ============================================================================

class AssertHeartBeatTest : public NetworkManagerAssertTest {
};

TEST_F(AssertHeartBeatTest, ValidHeartBeat) {
    auto buffer = createBuffer(4);
    setUint32At(buffer, 0, 123); // player_id
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertHeartBeat(packet));
}

TEST_F(AssertHeartBeatTest, InvalidPayloadSize) {
    auto buffer = createBuffer(3); // Wrong size
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHeartBeat(packet));
}

TEST_F(AssertHeartBeatTest, ZeroPlayerId) {
    auto buffer = createBuffer(4);
    setUint32At(buffer, 0, 0); // player_id = 0 (invalid)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHeartBeat(packet));
}

// ============================================================================
// assertPlayerInput Tests (0x10)
// ============================================================================

class AssertPlayerInputTest : public NetworkManagerAssertTest {
};

TEST_F(AssertPlayerInputTest, ValidPlayerInput) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 42); // player_id
    setUint16At(buffer, 4, 0x0F); // input_state (valid bits)
    setInt16At(buffer, 6, 100); // aim_direction_x
    setInt16At(buffer, 8, 50); // aim_direction_y
    setUint16At(buffer, 10, 0); // padding/reserved
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPlayerInput(packet));
}

TEST_F(AssertPlayerInputTest, InvalidPayloadSize) {
    auto buffer = createBuffer(11); // Wrong size
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerInput(packet));
}

TEST_F(AssertPlayerInputTest, ZeroPlayerId) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 0); // player_id = 0 (invalid)
    setUint16At(buffer, 4, 0x0F);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerInput(packet));
}

TEST_F(AssertPlayerInputTest, InvalidInputState) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 42);
    setUint16At(buffer, 4, 0x200); // Invalid bits set (bit 9 outside mask)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerInput(packet));
}

TEST_F(AssertPlayerInputTest, ValidInputStateMask) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 42);
    // Test with mask: only bits 0-8 are valid (INPUT_FLAGS_MASK = 0x1FF)
    setUint16At(buffer, 4, 0x1FF);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPlayerInput(packet));
}

TEST_F(AssertPlayerInputTest, ZeroInputState) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 42);
    setUint16At(buffer, 4, 0x00); // No input
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPlayerInput(packet));
}

// ============================================================================
// assertWorldSnapshot Tests (0x20)
// ============================================================================

class AssertWorldSnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertWorldSnapshotTest, EmptyWorldSnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1); // world_tick = 1
    setUint16At(buffer, 4, 0); // entity_count = 0
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertWorldSnapshot(packet));
}

TEST_F(AssertWorldSnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5); // Less than 6 bytes
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWorldSnapshot(packet));
}

TEST_F(AssertWorldSnapshotTest, MismatchedEntityCount) {
    auto buffer = createBuffer(6 + 15); // Room for 1 entity
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 2); // entity_count = 2 (but only 1 entity space)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWorldSnapshot(packet));
}

TEST_F(AssertWorldSnapshotTest, OneValidEntity) {
    auto buffer = createBuffer(6 + 15); // Header + 1 entity
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 1); // entity_count = 1

    // Valid EntityState at offset 6
    setUint32At(buffer, 6, 100); // entity_id
    setUint8At(buffer, 10, 0x02); // entity_type = ENEMY (valid: 0x01-0x08)
    setUint16At(buffer, 11, 512); // position_x
    setUint16At(buffer, 13, 256); // position_y
    setInt16At(buffer, 15, 10); // velocity_x
    setInt16At(buffer, 17, -5); // velocity_y
    setUint8At(buffer, 19, 50); // health > 0
    setUint8At(buffer, 20, 0x00); // state_flags
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertWorldSnapshot(packet));
}

// ============================================================================
// assertEntitySpawn Tests (0x21)
// ============================================================================

class AssertEntitySpawnTest : public NetworkManagerAssertTest {
};

TEST_F(AssertEntitySpawnTest, ValidEntitySpawn) {
    auto buffer = createBuffer(15);
    setUint32At(buffer, 0, 123); // entity_id
    setUint8At(buffer, 4, 0x01); // entity_type = PLAYER (valid)
    setUint16At(buffer, 5, 512); // position_x
    setUint16At(buffer, 7, 256); // position_y
    setInt16At(buffer, 9, 0); // velocity_x
    setInt16At(buffer, 11, 0); // velocity_y
    setUint8At(buffer, 13, 100); // health > 0
    setUint8At(buffer, 14, 0x00); // state_flags
    packet.header.flags = 0x01; // FLAG_RELIABLE
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertEntitySpawn(packet));
}

TEST_F(AssertEntitySpawnTest, InvalidPayloadSize) {
    auto buffer = createBuffer(14); // Wrong size
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntitySpawn(packet));
}

TEST_F(AssertEntitySpawnTest, ZeroEntityId) {
    auto buffer = createBuffer(15);
    setUint32At(buffer, 0, 0); // entity_id = 0 (invalid)
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 13, 100);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntitySpawn(packet));
}

TEST_F(AssertEntitySpawnTest, InvalidEntityType) {
    auto buffer = createBuffer(15);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, 0x00); // entity_type = 0 (invalid, min is 1)
    setUint8At(buffer, 13, 100);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntitySpawn(packet));
}

TEST_F(AssertEntitySpawnTest, ZeroHealth) {
    auto buffer = createBuffer(15);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, 0x01);
    setUint16At(buffer, 5, 512);
    setUint16At(buffer, 7, 256);
    setInt16At(buffer, 9, 0);
    setInt16At(buffer, 11, 0);
    setUint8At(buffer, 13, 0); // health = 0 (invalid for spawn)
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntitySpawn(packet));
}

TEST_F(AssertEntitySpawnTest, MissingReliableFlag) {
    auto buffer = createBuffer(15);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 13, 100);
    packet.header.flags = 0x00; // No FLAG_RELIABLE
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntitySpawn(packet));
}

// ============================================================================
// assertEntityDestroy Tests (0x22)
// ============================================================================

class AssertEntityDestroyTest : public NetworkManagerAssertTest {
};

TEST_F(AssertEntityDestroyTest, ValidEntityDestroy) {
    auto buffer = createBuffer(9);
    setUint32At(buffer, 0, 123); // entity_id
    setUint8At(buffer, 4, protocol::ENTITY_DESTROY_REASON_MIN); // destroy_reason
    setUint16At(buffer, 5, 512); // final_position_x
    setUint16At(buffer, 7, 256); // final_position_y
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertEntityDestroy(packet));
}

TEST_F(AssertEntityDestroyTest, InvalidPayloadSize) {
    auto buffer = createBuffer(8); // Wrong size
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityDestroy(packet));
}

TEST_F(AssertEntityDestroyTest, ZeroEntityId) {
    auto buffer = createBuffer(9);
    setUint32At(buffer, 0, 0); // entity_id = 0 (invalid)
    setUint8At(buffer, 4, protocol::ENTITY_DESTROY_REASON_MIN);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityDestroy(packet));
}

TEST_F(AssertEntityDestroyTest, InvalidDestroyReason) {
    auto buffer = createBuffer(9);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, 0xFF); // Invalid destroy reason (probably out of range)
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityDestroy(packet));
}

TEST_F(AssertEntityDestroyTest, MissingReliableFlag) {
    auto buffer = createBuffer(9);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, protocol::ENTITY_DESTROY_REASON_MIN);
    packet.header.flags = 0x00; // No FLAG_RELIABLE
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityDestroy(packet));
}

// ============================================================================
// assertEntityUpdate Tests (0x23)
// ============================================================================

class AssertEntityUpdateTest : public NetworkManagerAssertTest {
};

TEST_F(AssertEntityUpdateTest, ValidEntityUpdate) {
    auto buffer = createBuffer(16);
    setUint32At(buffer, 0, 123); // entity_id
    setUint8At(buffer, 4, protocol::ENTITY_UPDATE_FLAGS_MASK); // update_flags (all valid bits)
    setUint16At(buffer, 5, 512); // pos_x
    setUint16At(buffer, 7, 256); // pos_y
    setUint8At(buffer, 9, 75); // health
    setUint8At(buffer, 10, 100); // shield
    setUint8At(buffer, 11, 0x00); // state_flags
    setInt16At(buffer, 12, 10); // velocity_x
    setInt16At(buffer, 14, -5); // velocity_y
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertEntityUpdate(packet));
}

TEST_F(AssertEntityUpdateTest, InvalidPayloadSize) {
    auto buffer = createBuffer(15); // Wrong size
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityUpdate(packet));
}

TEST_F(AssertEntityUpdateTest, ZeroEntityId) {
    auto buffer = createBuffer(16);
    setUint32At(buffer, 0, 0); // entity_id = 0 (invalid)
    setUint8At(buffer, 4, 0x01);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityUpdate(packet));
}

TEST_F(AssertEntityUpdateTest, InvalidUpdateFlags) {
    auto buffer = createBuffer(16);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, 0xFF); // Invalid bits (ENTITY_UPDATE_FLAGS_MASK is typically 0x3F)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityUpdate(packet));
}

TEST_F(AssertEntityUpdateTest, NoUpdateFlags) {
    auto buffer = createBuffer(16);
    setUint32At(buffer, 0, 123);
    setUint8At(buffer, 4, 0x00); // No flags set
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertEntityUpdate(packet));
}

// ============================================================================
// assertTransformSnapshot Tests (0x24)
// ============================================================================

class AssertTransformSnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertTransformSnapshotTest, EmptyTransformSnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 0); // entity_count = 0
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertTransformSnapshot(packet));
}

TEST_F(AssertTransformSnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5); // Less than 6 bytes
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshot(packet));
}

TEST_F(AssertTransformSnapshotTest, InvalidDataSize) {
    // Size is 6 + 13 (not divisible by 12)
    auto buffer = createBuffer(19);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 0);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshot(packet));
}

TEST_F(AssertTransformSnapshotTest, OneEntity) {
    auto buffer = createBuffer(6 + 12); // Header + 1 entity (12 bytes each)
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 1); // entity_count = 1
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertTransformSnapshot(packet));
}

TEST_F(AssertTransformSnapshotTest, ManyEntities) {
    auto buffer = createBuffer(6 + (12 * 10)); // Header + 10 entities
    setUint32At(buffer, 0, 5); // world_tick
    setUint16At(buffer, 4, 10); // entity_count = 10
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertTransformSnapshot(packet));
}

TEST_F(AssertTransformSnapshotTest, SizeMismatch) {
    auto buffer = createBuffer(6 + 12); // Space for 1 entity
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 2); // entity_count = 2 (but only space for 1)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshot(packet));
}

// ============================================================================
// assertVelocitySnapshot Tests (0x25)
// ============================================================================

class AssertVelocitySnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertVelocitySnapshotTest, EmptyVelocitySnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 0); // entity_count = 0
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertVelocitySnapshot(packet));
}

TEST_F(AssertVelocitySnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5); // Less than 6 bytes
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVelocitySnapshot(packet));
}

TEST_F(AssertVelocitySnapshotTest, OneEntity) {
    auto buffer = createBuffer(6 + 12); // Header + 1 entity (12 bytes)
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 1); // entity_count = 1

    // Valid entity at offset 6
    setUint32At(buffer, 6, 100); // entity_id
    setInt16At(buffer, 10, 50); // vel_x
    setInt16At(buffer, 12, -30); // vel_y
    setInt16At(buffer, 14, 2); // acceleration_x
    setInt16At(buffer, 16, 1); // acceleration_y
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertVelocitySnapshot(packet));
}

TEST_F(AssertVelocitySnapshotTest, EntityWithZeroId) {
    auto buffer = createBuffer(6 + 12); // Header + 1 entity
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 1);

    // Invalid entity at offset 6
    setUint32At(buffer, 6, 0); // entity_id = 0 (invalid)
    setInt16At(buffer, 10, 50);
    setInt16At(buffer, 12, -30);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVelocitySnapshot(packet));
}

TEST_F(AssertVelocitySnapshotTest, ManyEntities) {
    auto buffer = createBuffer(6 + (12 * 5)); // Header + 5 entities
    setUint32At(buffer, 0, 10);
    setUint16At(buffer, 4, 5);

    // Add 5 valid entities
    for (int i = 0; i < 5; ++i) {
        size_t offset = 6 + (i * 12);
        setUint32At(buffer, offset, 100 + i); // entity_id
    }
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertVelocitySnapshot(packet));
}

TEST_F(AssertVelocitySnapshotTest, InvalidDataSize) {
    auto buffer = createBuffer(6 + 13); // Not divisible by 12
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 0);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVelocitySnapshot(packet));
}

// ============================================================================
// assertHealthSnapshot Tests (0x26)
// ============================================================================

class AssertHealthSnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertHealthSnapshotTest, EmptyHealthSnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 0); // entity_count = 0
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertHealthSnapshot(packet));
}

TEST_F(AssertHealthSnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5); // Less than 6 bytes
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshot(packet));
}

TEST_F(AssertHealthSnapshotTest, OneEntity) {
    auto buffer = createBuffer(6 + 8); // Header + 1 entity (8 bytes)
    setUint32At(buffer, 0, 1); // world_tick
    setUint16At(buffer, 4, 1); // entity_count = 1

    // Valid entity at offset 6 (entity_id(4) + health(1) + max_health(1) + shield(1) + max_shield(1))
    setUint32At(buffer, 6, 100); // entity_id
    setUint8At(buffer, 10, 100); // current_health
    setUint8At(buffer, 11, 100); // max_health > 0
    setUint8At(buffer, 12, 50); // current_shield
    setUint8At(buffer, 13, 100); // max_shield
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertHealthSnapshot(packet));
}

TEST_F(AssertHealthSnapshotTest, EntityWithZeroId) {
    auto buffer = createBuffer(6 + 8);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 1);

    setUint32At(buffer, 6, 0); // entity_id = 0 (invalid)
    setUint8At(buffer, 10, 75);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshot(packet));
}

TEST_F(AssertHealthSnapshotTest, ManyEntities) {
    auto buffer = createBuffer(6 + (8 * 10)); // Header + 10 entities
    setUint32At(buffer, 0, 50);
    setUint16At(buffer, 4, 10);

    for (int i = 0; i < 10; ++i) {
        size_t offset = 6 + (i * 8);
        setUint32At(buffer, offset, 200 + i); // entity_id
        setUint8At(buffer, offset + 4, 100); // current_health
        setUint8At(buffer, offset + 5, 100); // max_health > 0
        setUint8At(buffer, offset + 6, 50); // current_shield
        setUint8At(buffer, offset + 7, 100); // max_shield
    }
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertHealthSnapshot(packet));
}

TEST_F(AssertHealthSnapshotTest, InvalidDataSize) {
    auto buffer = createBuffer(6 + 7); // Not divisible by 8
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 0);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshot(packet));
}

TEST_F(AssertHealthSnapshotTest, SizeMismatch) {
    auto buffer = createBuffer(6 + 8); // Space for 1 entity
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 2); // entity_count = 2 (but only space for 1)
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshot(packet));
}

// ============================================================================
// assertWeaponSnapshot Tests (0x27)
// ============================================================================

class AssertWeaponSnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertWeaponSnapshotTest, EmptyWeaponSnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 0);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertWeaponSnapshot(packet));
}

TEST_F(AssertWeaponSnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWeaponSnapshot(packet));
}

TEST_F(AssertWeaponSnapshotTest, SizeMismatch) {
    auto buffer = createBuffer(6 + 9);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 2); // Claims 2 entities but only space for 1
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWeaponSnapshot(packet));
}

TEST_F(AssertWeaponSnapshotTest, OneEntity) {
    auto buffer = createBuffer(6 + 9);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertWeaponSnapshot(packet));
}

// ============================================================================
// assertAISnapshot Tests (0x28)
// ============================================================================

class AssertAISnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertAISnapshotTest, EmptyAISnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 0);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAISnapshot(packet));
}

TEST_F(AssertAISnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAISnapshot(packet));
}

TEST_F(AssertAISnapshotTest, SizeMismatch) {
    auto buffer = createBuffer(6 + 12);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 2);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAISnapshot(packet));
}

TEST_F(AssertAISnapshotTest, OneEntity) {
    auto buffer = createBuffer(6 + 12);
    setUint32At(buffer, 0, 5);
    setUint16At(buffer, 4, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAISnapshot(packet));
}

// ============================================================================
// assertAnimationSnapshot Tests (0x29)
// ============================================================================

class AssertAnimationSnapshotTest : public NetworkManagerAssertTest {
};

TEST_F(AssertAnimationSnapshotTest, EmptyAnimationSnapshot) {
    auto buffer = createBuffer(6);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 0);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAnimationSnapshot(packet));
}

TEST_F(AssertAnimationSnapshotTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAnimationSnapshot(packet));
}

TEST_F(AssertAnimationSnapshotTest, SizeMismatch) {
    auto buffer = createBuffer(6 + 11);
    setUint32At(buffer, 0, 1);
    setUint16At(buffer, 4, 2);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAnimationSnapshot(packet));
}

TEST_F(AssertAnimationSnapshotTest, OneEntity) {
    auto buffer = createBuffer(6 + 11);
    setUint32At(buffer, 0, 10);
    setUint16At(buffer, 4, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAnimationSnapshot(packet));
}

// ============================================================================
// assertComponentAdd Tests (0x2A)
// ============================================================================

class AssertComponentAddTest : public NetworkManagerAssertTest {
};

TEST_F(AssertComponentAddTest, ValidComponentAdd) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 5);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertComponentAdd(packet));
}

TEST_F(AssertComponentAddTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentAdd(packet));
}

TEST_F(AssertComponentAddTest, ZeroEntityId) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 0);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 5);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentAdd(packet));
}

TEST_F(AssertComponentAddTest, InvalidComponentType) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x00); // Invalid type
    setUint8At(buffer, 5, 5);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentAdd(packet));
}

TEST_F(AssertComponentAddTest, MissingReliableFlag) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 5);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentAdd(packet));
}

// ============================================================================
// assertComponentRemove Tests (0x2B)
// ============================================================================

class AssertComponentRemoveTest : public NetworkManagerAssertTest {
};

TEST_F(AssertComponentRemoveTest, ValidComponentRemove) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertComponentRemove(packet));
}

TEST_F(AssertComponentRemoveTest, InvalidPayloadSize) {
    auto buffer = createBuffer(4);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentRemove(packet));
}

TEST_F(AssertComponentRemoveTest, ZeroEntityId) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 0);
    setUint8At(buffer, 4, 0x01);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentRemove(packet));
}

TEST_F(AssertComponentRemoveTest, InvalidComponentType) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x00);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentRemove(packet));
}

TEST_F(AssertComponentRemoveTest, MissingReliableFlag) {
    auto buffer = createBuffer(5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertComponentRemove(packet));
}

// ============================================================================
// assertTransformSnapshotDelta Tests (0x2C)
// ============================================================================

class AssertTransformSnapshotDeltaTest : public NetworkManagerAssertTest {
};

TEST_F(AssertTransformSnapshotDeltaTest, ValidTransformSnapshotDelta) {
    auto buffer = createBuffer(10 + 12);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 50);
    setUint16At(buffer, 8, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertTransformSnapshotDelta(packet));
}

TEST_F(AssertTransformSnapshotDeltaTest, InvalidPayloadSize) {
    auto buffer = createBuffer(9);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshotDelta(packet));
}

TEST_F(AssertTransformSnapshotDeltaTest, BaseTick_NotLessThanWorldTick) {
    auto buffer = createBuffer(10 + 12);
    setUint32At(buffer, 0, 50);
    setUint32At(buffer, 4, 50); // base_tick == world_tick (invalid)
    setUint16At(buffer, 8, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshotDelta(packet));
}

TEST_F(AssertTransformSnapshotDeltaTest, BaseTick_GreaterThanWorldTick) {
    auto buffer = createBuffer(10 + 12);
    setUint32At(buffer, 0, 50);
    setUint32At(buffer, 4, 100); // base_tick > world_tick (invalid)
    setUint16At(buffer, 8, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshotDelta(packet));
}

TEST_F(AssertTransformSnapshotDeltaTest, SizeMismatch) {
    auto buffer = createBuffer(10 + 11);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 50);
    setUint16At(buffer, 8, 2);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertTransformSnapshotDelta(packet));
}

// ============================================================================
// assertHealthSnapshotDelta Tests (0x2D)
// ============================================================================

class AssertHealthSnapshotDeltaTest : public NetworkManagerAssertTest {
};

TEST_F(AssertHealthSnapshotDeltaTest, ValidHealthSnapshotDelta) {
    auto buffer = createBuffer(10 + 8);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 50);
    setUint16At(buffer, 8, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertHealthSnapshotDelta(packet));
}

TEST_F(AssertHealthSnapshotDeltaTest, InvalidPayloadSize) {
    auto buffer = createBuffer(9);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshotDelta(packet));
}

TEST_F(AssertHealthSnapshotDeltaTest, BaseTick_NotLessThanWorldTick) {
    auto buffer = createBuffer(10 + 8);
    setUint32At(buffer, 0, 50);
    setUint32At(buffer, 4, 50);
    setUint16At(buffer, 8, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshotDelta(packet));
}

TEST_F(AssertHealthSnapshotDeltaTest, SizeMismatch) {
    auto buffer = createBuffer(10 + 7);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 50);
    setUint16At(buffer, 8, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertHealthSnapshotDelta(packet));
}

// ============================================================================
// assertEntityFullState Tests (0x2E)
// ============================================================================

class AssertEntityFullStateTest : public NetworkManagerAssertTest {
};

TEST_F(AssertEntityFullStateTest, ValidEntityFullState) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 1);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertEntityFullState(packet));
}

TEST_F(AssertEntityFullStateTest, InvalidPayloadSize) {
    auto buffer = createBuffer(5);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityFullState(packet));
}

TEST_F(AssertEntityFullStateTest, ZeroEntityId) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 0);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 1);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityFullState(packet));
}

TEST_F(AssertEntityFullStateTest, InvalidEntityType) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x00);
    setUint8At(buffer, 5, 1);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityFullState(packet));
}

TEST_F(AssertEntityFullStateTest, ZeroComponentCount) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 0);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityFullState(packet));
}

TEST_F(AssertEntityFullStateTest, ComponentCountAboveLimit) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 16); // > 15
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityFullState(packet));
}

TEST_F(AssertEntityFullStateTest, MissingReliableFlag) {
    auto buffer = createBuffer(6 + 5);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 0x01);
    setUint8At(buffer, 5, 1);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertEntityFullState(packet));
}

// ============================================================================
// assertPlayerHit Tests (0x40)
// ============================================================================

class AssertPlayerHitTest : public NetworkManagerAssertTest {
};

TEST_F(AssertPlayerHitTest, ValidPlayerHit) {
    auto buffer = createBuffer(17);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 8, 25);
    setUint8At(buffer, 9, 75);
    setUint8At(buffer, 10, 50);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPlayerHit(packet));
}

TEST_F(AssertPlayerHitTest, InvalidPayloadSize) {
    auto buffer = createBuffer(16);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerHit(packet));
}

TEST_F(AssertPlayerHitTest, ZeroPlayerId) {
    auto buffer = createBuffer(17);
    setUint32At(buffer, 0, 0);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 8, 25);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerHit(packet));
}

TEST_F(AssertPlayerHitTest, ZeroAttackerId) {
    auto buffer = createBuffer(17);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 0);
    setUint8At(buffer, 8, 25);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerHit(packet));
}

TEST_F(AssertPlayerHitTest, ZeroDamage) {
    auto buffer = createBuffer(17);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 8, 0);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerHit(packet));
}

TEST_F(AssertPlayerHitTest, MissingReliableFlag) {
    auto buffer = createBuffer(17);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 8, 25);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerHit(packet));
}

// ============================================================================
// assertPlayerDeath Tests (0x41)
// ============================================================================

class AssertPlayerDeathTest : public NetworkManagerAssertTest {
};

TEST_F(AssertPlayerDeathTest, ValidPlayerDeath) {
    auto buffer = createBuffer(18);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint32At(buffer, 8, 5000);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPlayerDeath(packet));
}

TEST_F(AssertPlayerDeathTest, InvalidPayloadSize) {
    auto buffer = createBuffer(17);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerDeath(packet));
}

TEST_F(AssertPlayerDeathTest, ZeroPlayerId) {
    auto buffer = createBuffer(18);
    setUint32At(buffer, 0, 0);
    setUint32At(buffer, 4, 200);
    setUint32At(buffer, 8, 5000);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerDeath(packet));
}

TEST_F(AssertPlayerDeathTest, ZeroKillerId) {
    auto buffer = createBuffer(18);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 0);
    setUint32At(buffer, 8, 5000);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerDeath(packet));
}

TEST_F(AssertPlayerDeathTest, MissingReliableFlag) {
    auto buffer = createBuffer(18);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint32At(buffer, 8, 5000);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPlayerDeath(packet));
}

// ============================================================================
// assertScoreUpdate Tests (0x42)
// ============================================================================

class AssertScoreUpdateTest : public NetworkManagerAssertTest {
};

TEST_F(AssertScoreUpdateTest, ValidScoreUpdate) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 5500);
    setInt16At(buffer, 8, 500);
    setUint8At(buffer, 10, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertScoreUpdate(packet));
}

TEST_F(AssertScoreUpdateTest, InvalidPayloadSize) {
    auto buffer = createBuffer(10);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertScoreUpdate(packet));
}

TEST_F(AssertScoreUpdateTest, ZeroPlayerId) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 0);
    setUint32At(buffer, 4, 5500);
    setInt16At(buffer, 8, 500);
    setUint8At(buffer, 10, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertScoreUpdate(packet));
}

TEST_F(AssertScoreUpdateTest, ZeroScoreDelta) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 5500);
    setInt16At(buffer, 8, 0);
    setUint8At(buffer, 10, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertScoreUpdate(packet));
}

TEST_F(AssertScoreUpdateTest, InvalidReason) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 5500);
    setInt16At(buffer, 8, 500);
    setUint8At(buffer, 10, 6); // > 5
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertScoreUpdate(packet));
}

TEST_F(AssertScoreUpdateTest, NegativeScoreDelta) {
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 5000);
    setInt16At(buffer, 8, -500);
    setUint8At(buffer, 10, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertScoreUpdate(packet));
}

// ============================================================================
// assertPowerupPickup Tests (0x43)
// ============================================================================

class AssertPowerupPickupTest : public NetworkManagerAssertTest {
};

TEST_F(AssertPowerupPickupTest, ValidPowerupPickup) {
    auto buffer = createBuffer(10);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 500);
    setUint8At(buffer, 8, 2);
    setUint8At(buffer, 9, 30);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPowerupPickup(packet));
}

TEST_F(AssertPowerupPickupTest, InvalidPayloadSize) {
    auto buffer = createBuffer(9);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPowerupPickup(packet));
}

TEST_F(AssertPowerupPickupTest, ZeroPlayerId) {
    auto buffer = createBuffer(10);
    setUint32At(buffer, 0, 0);
    setUint32At(buffer, 4, 500);
    setUint8At(buffer, 8, 2);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPowerupPickup(packet));
}

TEST_F(AssertPowerupPickupTest, ZeroPowerupId) {
    auto buffer = createBuffer(10);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 0);
    setUint8At(buffer, 8, 2);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPowerupPickup(packet));
}

TEST_F(AssertPowerupPickupTest, InvalidPowerupType) {
    auto buffer = createBuffer(10);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 500);
    setUint8At(buffer, 8, 6); // > 5
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPowerupPickup(packet));
}

TEST_F(AssertPowerupPickupTest, MissingReliableFlag) {
    auto buffer = createBuffer(10);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 500);
    setUint8At(buffer, 8, 2);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPowerupPickup(packet));
}

// ============================================================================
// assertWeaponFire Tests (0x44)
// ============================================================================

class AssertWeaponFireTest : public NetworkManagerAssertTest {
};

TEST_F(AssertWeaponFireTest, ValidWeaponFire) {
    auto buffer = createBuffer(19);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setInt16At(buffer, 8, 512);
    setInt16At(buffer, 10, 256);
    setInt16At(buffer, 12, 100);
    setInt16At(buffer, 14, 50);
    setUint8At(buffer, 16, 1);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertWeaponFire(packet));
}

TEST_F(AssertWeaponFireTest, InvalidPayloadSize) {
    auto buffer = createBuffer(18);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWeaponFire(packet));
}

TEST_F(AssertWeaponFireTest, ZeroShooterId) {
    auto buffer = createBuffer(19);
    setUint32At(buffer, 0, 0);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 16, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWeaponFire(packet));
}

TEST_F(AssertWeaponFireTest, ZeroProjectileId) {
    auto buffer = createBuffer(19);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 0);
    setUint8At(buffer, 16, 1);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWeaponFire(packet));
}

TEST_F(AssertWeaponFireTest, InvalidWeaponType) {
    auto buffer = createBuffer(19);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 16, 6); // > 5
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertWeaponFire(packet));
}

// ============================================================================
// assertVisualEffect Tests (0x45)
// ============================================================================

class AssertVisualEffectTest : public NetworkManagerAssertTest {
};

TEST_F(AssertVisualEffectTest, ValidVisualEffect) {
    auto buffer = createBuffer(14);
    setUint8At(buffer, 0, 5);
    setInt16At(buffer, 1, 512);
    setInt16At(buffer, 3, 256);
    setUint16At(buffer, 5, 5000);
    setUint8At(buffer, 7, 100);
    setUint8At(buffer, 8, 255);
    setUint8At(buffer, 9, 128);
    setUint8At(buffer, 10, 64);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertVisualEffect(packet));
}

TEST_F(AssertVisualEffectTest, InvalidPayloadSize) {
    auto buffer = createBuffer(13);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVisualEffect(packet));
}

TEST_F(AssertVisualEffectTest, InvalidEffectType) {
    auto buffer = createBuffer(14);
    setUint8At(buffer, 0, 13); // Too high
    setUint16At(buffer, 5, 5000);
    setUint8At(buffer, 7, 100);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVisualEffect(packet));
}

TEST_F(AssertVisualEffectTest, ZeroDuration) {
    auto buffer = createBuffer(14);
    setUint8At(buffer, 0, 5);
    setUint16At(buffer, 5, 0);
    setUint8At(buffer, 7, 100);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVisualEffect(packet));
}

TEST_F(AssertVisualEffectTest, DurationTooLong) {
    auto buffer = createBuffer(14);
    setUint8At(buffer, 0, 5);
    setUint16At(buffer, 5, 60001);
    setUint8At(buffer, 7, 100);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVisualEffect(packet));
}

TEST_F(AssertVisualEffectTest, ZeroScale) {
    auto buffer = createBuffer(14);
    setUint8At(buffer, 0, 5);
    setUint16At(buffer, 5, 5000);
    setUint8At(buffer, 7, 0);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertVisualEffect(packet));
}

// ============================================================================
// assertAudioEffect Tests (0x46)
// ============================================================================

class AssertAudioEffectTest : public NetworkManagerAssertTest {
};

TEST_F(AssertAudioEffectTest, ValidAudioEffect) {
    auto buffer = createBuffer(7);
    setUint8At(buffer, 0, 3);
    setInt16At(buffer, 1, 512);
    setInt16At(buffer, 3, 256);
    setUint8At(buffer, 5, 200);
    setUint8At(buffer, 6, 100);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAudioEffect(packet));
}

TEST_F(AssertAudioEffectTest, InvalidPayloadSize) {
    auto buffer = createBuffer(6);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAudioEffect(packet));
}

TEST_F(AssertAudioEffectTest, InvalidEffectType) {
    auto buffer = createBuffer(7);
    setUint8At(buffer, 0, 13); // > 0x0C
    setUint8At(buffer, 6, 100);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAudioEffect(packet));
}

TEST_F(AssertAudioEffectTest, PitchTooLow) {
    auto buffer = createBuffer(7);
    setUint8At(buffer, 0, 3);
    setUint8At(buffer, 6, 49); // < 50
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAudioEffect(packet));
}

TEST_F(AssertAudioEffectTest, PitchTooHigh) {
    auto buffer = createBuffer(7);
    setUint8At(buffer, 0, 3);
    setUint8At(buffer, 6, 201); // > 200
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAudioEffect(packet));
}

TEST_F(AssertAudioEffectTest, ValidPitchRange) {
    auto buffer = createBuffer(7);
    setUint8At(buffer, 0, 3);
    setUint8At(buffer, 6, 50);
    setPacketData(buffer);
    EXPECT_TRUE(NetworkManager::assertAudioEffect(packet));

    buffer = createBuffer(7);
    setUint8At(buffer, 0, 3);
    setUint8At(buffer, 6, 200);
    setPacketData(buffer);
    EXPECT_TRUE(NetworkManager::assertAudioEffect(packet));

    buffer = createBuffer(7);
    setUint8At(buffer, 0, 3);
    setUint8At(buffer, 6, 125);
    setPacketData(buffer);
    EXPECT_TRUE(NetworkManager::assertAudioEffect(packet));
}

// ============================================================================
// assertParticleSpawn Tests (0x47)
// ============================================================================

class AssertParticleSpawnTest : public NetworkManagerAssertTest {
};

TEST_F(AssertParticleSpawnTest, InvalidPayloadSize) {
    auto buffer = createBuffer(25);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertParticleSpawn(packet));
}

TEST_F(AssertParticleSpawnTest, ZeroParticleCount) {
    auto buffer = createBuffer(26);
    setUint16At(buffer, 0, 1);
    setInt16At(buffer, 2, 512);
    setInt16At(buffer, 4, 256);
    setInt16At(buffer, 6, 10);
    setInt16At(buffer, 8, -5);
    setUint16At(buffer, 10, 0);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertParticleSpawn(packet));
}

TEST_F(AssertParticleSpawnTest, ValidParticleSpawn) {
    auto buffer = createBuffer(26);
    setUint16At(buffer, 0, 1);
    setInt16At(buffer, 2, 512);
    setInt16At(buffer, 4, 256);
    setInt16At(buffer, 6, 10);
    setInt16At(buffer, 8, -5);
    setUint16At(buffer, 10, 100);
    setUint16At(buffer, 12, 5000);
    setUint8At(buffer, 14, 255);
    setUint8At(buffer, 15, 128);
    setUint8At(buffer, 16, 64);
    setUint8At(buffer, 17, 200);
    setUint8At(buffer, 18, 100);
    setUint8At(buffer, 19, 50);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertParticleSpawn(packet));
}

// ============================================================================
// assertGameStart Tests (0x60)
// ============================================================================

class AssertGameStartTest : public NetworkManagerAssertTest {
};

TEST_F(AssertGameStartTest, ValidGameStart) {
    auto buffer = createBuffer(23);
    setUint32At(buffer, 0, 1); // game_instance_id
    setUint8At(buffer, 4, 2); // player_count
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertGameStart(packet));
}

TEST_F(AssertGameStartTest, InvalidPayloadSize) {
    auto buffer = createBuffer(22);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertGameStart(packet));
}

TEST_F(AssertGameStartTest, MissingReliableFlag) {
    auto buffer = createBuffer(10);
    setUint32At(buffer, 0, 1);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertGameStart(packet));
}

// ============================================================================
// assertGameEnd Tests (0x61)
// ============================================================================

class AssertGameEndTest : public NetworkManagerAssertTest {
};

TEST_F(AssertGameEndTest, ValidGameEnd) {
    auto buffer = createBuffer(22);
    setUint8At(buffer, 0, 1); // end_reason
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertGameEnd(packet));
}

TEST_F(AssertGameEndTest, InvalidPayloadSize) {
    auto buffer = createBuffer(21);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertGameEnd(packet));
}

TEST_F(AssertGameEndTest, MissingReliableFlag) {
    auto buffer = createBuffer(22);
    setUint8At(buffer, 0, 1);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertGameEnd(packet));
}

// ============================================================================
// assertLevelComplete Tests (0x62)
// ============================================================================

class AssertLevelCompleteTest : public NetworkManagerAssertTest {
};

TEST_F(AssertLevelCompleteTest, ValidLevelComplete) {
    auto buffer = createBuffer(8);
    setUint8At(buffer, 0, 1); // completed_level
    setUint8At(buffer, 1, 2); // next_level
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertLevelComplete(packet));
}

TEST_F(AssertLevelCompleteTest, InvalidPayloadSize) {
    auto buffer = createBuffer(7);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertLevelComplete(packet));
}

TEST_F(AssertLevelCompleteTest, MissingReliableFlag) {
    auto buffer = createBuffer(8);
    setUint8At(buffer, 0, 1);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertLevelComplete(packet));
}

// ============================================================================
// assertLevelStart Tests (0x63)
// ============================================================================

class AssertLevelStartTest : public NetworkManagerAssertTest {
};

TEST_F(AssertLevelStartTest, ValidLevelStart) {
    auto buffer = createBuffer(35);
    setUint8At(buffer, 0, 1); // level_id
    setStringAt(buffer, 1, "Level 1", 32); // level_name
    setUint16At(buffer, 33, 60); // estimated_duration
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertLevelStart(packet));
}

TEST_F(AssertLevelStartTest, InvalidPayloadSize) {
    auto buffer = createBuffer(34);
    packet.header.flags = 0x01;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertLevelStart(packet));
}

TEST_F(AssertLevelStartTest, MissingReliableFlag) {
    auto buffer = createBuffer(35);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "Level 1", 32);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertLevelStart(packet));
}

// ============================================================================
// assertForceState Tests (0x64)
// ============================================================================

class AssertForceStateTest : public NetworkManagerAssertTest {
};

TEST_F(AssertForceStateTest, ValidForceState) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 100); // force_entity_id
    setUint32At(buffer, 4, 200); // parent_ship_id
    setUint8At(buffer, 8, 0); // attachment_point
    setUint8At(buffer, 9, 3); // power_level (1-5)
    setUint8At(buffer, 10, 50); // charge_percentage
    setUint8At(buffer, 11, 0); // is_firing
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertForceState(packet));
}

TEST_F(AssertForceStateTest, InvalidPayloadSize) {
    auto buffer = createBuffer(11);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertForceState(packet));
}

TEST_F(AssertForceStateTest, ValidWithoutReliableFlag) {
    auto buffer = createBuffer(12);
    setUint32At(buffer, 0, 100);
    setUint32At(buffer, 4, 200);
    setUint8At(buffer, 8, 0);
    setUint8At(buffer, 9, 3);
    setUint8At(buffer, 10, 50);
    setUint8At(buffer, 11, 0);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertForceState(packet));
}

// ============================================================================
// assertAIState Tests (0x65)
// ============================================================================

class AssertAIStateTest : public NetworkManagerAssertTest {
};

TEST_F(AssertAIStateTest, ValidAIState) {
    auto buffer = createBuffer(18);
    setUint32At(buffer, 0, 100); // entity_id
    setUint8At(buffer, 4, 1); // current_state
    setUint8At(buffer, 5, 1); // behavior_type
    setUint32At(buffer, 6, 200); // target_entity_id
    setInt16At(buffer, 10, 512); // waypoint_x
    setInt16At(buffer, 12, 256); // waypoint_y
    setUint16At(buffer, 14, 1000); // state_timer
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAIState(packet));
}

TEST_F(AssertAIStateTest, InvalidPayloadSize) {
    auto buffer = createBuffer(17);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAIState(packet));
}

TEST_F(AssertAIStateTest, ValidWithoutReliableFlag) {
    auto buffer = createBuffer(18);
    setUint32At(buffer, 0, 100);
    setUint8At(buffer, 4, 1);
    setUint8At(buffer, 5, 1);
    setUint32At(buffer, 6, 200);
    setInt16At(buffer, 10, 512);
    setInt16At(buffer, 12, 256);
    setUint16At(buffer, 14, 1000);
    packet.header.flags = 0x00;
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAIState(packet));
}

// ============================================================================
// assertAcknowledgment Tests (0x70)
// ============================================================================

class AssertAcknowledgmentTest : public NetworkManagerAssertTest {
};

TEST_F(AssertAcknowledgmentTest, ValidAcknowledgment) {
    auto buffer = createBuffer(8);
    setUint32At(buffer, 0, 1);
    setUint32At(buffer, 4, 100);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertAcknowledgment(packet));
}

TEST_F(AssertAcknowledgmentTest, InvalidPayloadSize) {
    auto buffer = createBuffer(7);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertAcknowledgment(packet));
}

// ============================================================================
// assertPing Tests (0x71)
// ============================================================================

class AssertPingTest : public NetworkManagerAssertTest {
};

TEST_F(AssertPingTest, ValidPing) {
    auto buffer = createBuffer(4);
    setUint32At(buffer, 0, 12345);
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPing(packet));
}

TEST_F(AssertPingTest, InvalidPayloadSize) {
    auto buffer = createBuffer(3);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPing(packet));
}

// ============================================================================
// assertPong Tests (0x72)
// ============================================================================

class AssertPongTest : public NetworkManagerAssertTest {
};

TEST_F(AssertPongTest, ValidPong) {
    auto buffer = createBuffer(8);
    setUint32At(buffer, 0, 54321); // client_timestamp
    setUint32At(buffer, 4, 12345); // server_timestamp
    setPacketData(buffer);

    EXPECT_TRUE(NetworkManager::assertPong(packet));
}

TEST_F(AssertPongTest, InvalidPayloadSize) {
    auto buffer = createBuffer(7);
    setPacketData(buffer);

    EXPECT_FALSE(NetworkManager::assertPong(packet));
}

// Test FindHandler
class FindHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FindHandlerTest, FindHandlerClientConnect) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_CLIENT_CONNECT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerServerAccept) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_SERVER_ACCEPT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerServerReject) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_SERVER_REJECT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerClientDisconnect) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerHeartbeat) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_HEARTBEAT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerPlayerInput) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PLAYER_INPUT);
    EXPECT_NE(handler, nullptr);
}

/* TEST_F(FindHandlerTest, FindHandlerWorldSnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_WORLD_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}
 */
TEST_F(FindHandlerTest, FindHandlerEntitySpawn) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_ENTITY_SPAWN);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerEntityDestroy) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_ENTITY_DESTROY);
    EXPECT_NE(handler, nullptr);
}

/* TEST_F(FindHandlerTest, FindHandlerEntityUpdate) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_ENTITY_UPDATE);
    EXPECT_NE(handler, nullptr);
}
 */
TEST_F(FindHandlerTest, FindHandlerPlayerHit) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PLAYER_HIT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerPlayerDeath) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PLAYER_DEATH);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerScoreUpdate) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_SCORE_UPDATE);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerPowerPickup) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_POWER_PICKUP);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerWeaponFire) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_WEAPON_FIRE);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerGameStart) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_GAME_START);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerGameEnd) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_GAME_END);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerLevelComplete) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_LEVEL_COMPLETE);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerLevelStart) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_LEVEL_START);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerAck) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_ACK);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerPing) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PING);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerPong) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PONG);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerTransformSnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerVelocitySnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_VELOCITY_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerHealthSnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerWeaponSnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerAISnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_AI_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerAnimationSnapshot) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerComponentAdd) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_COMPONENT_ADD);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerComponentRemove) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_COMPONENT_REMOVE);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerTransformSnapshotDelta) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerHealthSnapshotDelta) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerEntityFullState) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_ENTITY_FULL_STATE);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerVisualEffect) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_VISUAL_EFFECT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerAudioEffect) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_AUDIO_EFFECT);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerParticleSpawn) {
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PARTICLE_SPAWN);
    EXPECT_NE(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerInvalidType) {
    // Try with an invalid type that doesn't exist
    auto handler = NetworkManager::findHandler(static_cast<protocol::PacketTypes>(0xFF));
    EXPECT_EQ(handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerMultipleCalls) {
    // Call findHandler multiple times and verify consistency
    auto handler1 = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PING);
    auto handler2 = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PING);
    
    EXPECT_NE(handler1, nullptr);
    EXPECT_NE(handler2, nullptr);
    EXPECT_EQ(handler1->type, handler2->type);
}

TEST_F(FindHandlerTest, FindHandlerDifferentTypes) {
    // Verify that different types return different handlers
    auto ping_handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PING);
    auto pong_handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PONG);
    
    EXPECT_NE(ping_handler, nullptr);
    EXPECT_NE(pong_handler, nullptr);
}

TEST_F(FindHandlerTest, FindHandlerValidateFunctions) {
    // Ensure handlers have valid function pointers
    auto handler = NetworkManager::findHandler(protocol::PacketTypes::TYPE_PING);
    
    EXPECT_NE(handler, nullptr);
    EXPECT_NE(handler->assertFunc, nullptr);
    EXPECT_NE(handler->createFunc, nullptr);
}
