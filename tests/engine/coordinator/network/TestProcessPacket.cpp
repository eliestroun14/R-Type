/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PacketManager processPacket Tests
**
** These tests focus on the processPacket() method behavior:
** - Magic number validation
** - Unknown packet type handling
** - Correct optional return behavior
** - Packet data preservation
**
** For detailed validation of individual packet types, see TestPacketManagerAssert.cpp
*/

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <common/protocol/PacketManager.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/Protocol.hpp>

using namespace protocol;

// ============================================================================
// Test Fixtures
// ============================================================================

class ProcessPacketTest : public ::testing::Test {
protected:
    void SetUp() override {
        packet.header.magic = 0x5254;
        packet.header.flags = 0x01;
        packet.header.sequence_number = 1;
        packet.header.timestamp = 0;
    }

    common::protocol::Packet packet;
    PacketManager nm;

    void setPacketData(const std::vector<uint8_t>& data) {
        packet.data = data;
    }

    std::vector<uint8_t> createBuffer(size_t size) {
        return std::vector<uint8_t>(size, 0);
    }

    void setUint32At(std::vector<uint8_t>& buffer, size_t offset, uint32_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(uint32_t));
    }

    void setUint16At(std::vector<uint8_t>& buffer, size_t offset, uint16_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(uint16_t));
    }

    void setUint8At(std::vector<uint8_t>& buffer, size_t offset, uint8_t value) {
        std::memcpy(buffer.data() + offset, &value, sizeof(uint8_t));
    }

    void setStringAt(std::vector<uint8_t>& buffer, size_t offset, const std::string& str, size_t maxLen) {
        std::memcpy(buffer.data() + offset, str.c_str(), std::min(str.length(), maxLen));
    }
};

// ============================================================================
// Magic Number Validation Tests
// ============================================================================

class ProcessPacketMagicTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketMagicTest, InvalidMagicNumberZero) {
    packet.header.magic = 0x0000;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 42);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcessPacketMagicTest, InvalidMagicNumberWrong) {
    packet.header.magic = 0xDEAD;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 42);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcessPacketMagicTest, ValidMagicNumber) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 42);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_TRUE(result.has_value());
}

// ============================================================================
// Unknown Packet Type Tests
// ============================================================================

class ProcessPacketUnknownTypeTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketUnknownTypeTest, UnknownPacketTypeMax) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = 0xFF;
    
    auto buffer = createBuffer(10);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcessPacketUnknownTypeTest, UnknownPacketTypeRandom) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = 0x99;
    
    auto buffer = createBuffer(10);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Reserved Flags Validation Tests
// ============================================================================

class ProcessPacketFlagsTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketFlagsTest, ReservedFlagBitsRejectPacket) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    packet.header.flags = 0x80; // reserved bits outside 0-4 set

    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 42);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Return Value Behavior Tests
// ============================================================================

class ProcessPacketReturnValueTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketReturnValueTest, ValidPacketReturnsOptional) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "Player", 31);
    setUint32At(buffer, 33, 1);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcessPacketReturnValueTest, InvalidPacketReturnsNullopt) {
    packet.header.magic = 0x0000;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcessPacketReturnValueTest, InvalidDataReturnsNullopt) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    // Wrong payload size
    auto buffer = createBuffer(36);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Packet Data Preservation Tests
// ============================================================================

class ProcessPacketDataPreservationTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketDataPreservationTest, HeaderPreserved) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    packet.header.sequence_number = 42;
    packet.header.timestamp = 12345;
    packet.header.flags = 0x05;
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "TestPlayer", 31);
    setUint32At(buffer, 33, 100);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.magic, 0x5254);
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT));
    EXPECT_EQ(result->header.sequence_number, 42);
    EXPECT_EQ(result->header.timestamp, 12345);
    EXPECT_EQ(result->header.flags, 0x05);
}

TEST_F(ProcessPacketDataPreservationTest, PayloadPreserved) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "UniquePlayerName", 31);
    setUint32At(buffer, 33, 999);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->data.size(), 37);
    EXPECT_EQ(result->data[0], 1);
    EXPECT_EQ(result->data[33], packet.data[33]);
    EXPECT_EQ(result->data[34], packet.data[34]);
    EXPECT_EQ(result->data[35], packet.data[35]);
    EXPECT_EQ(result->data[36], packet.data[36]);
}

// ============================================================================
// Handler Dispatch Tests
// ============================================================================

class ProcessPacketDispatchTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketDispatchTest, DispatchClientConnectType) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer = createBuffer(37);
    setUint8At(buffer, 0, 1);
    setStringAt(buffer, 1, "Player", 31);
    setUint32At(buffer, 33, 1);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcessPacketDispatchTest, DispatchServerAcceptType) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_SERVER_ACCEPT);
    
    auto buffer = createBuffer(11);
    setUint32At(buffer, 0, 1);
    setUint8At(buffer, 4, 4);
    setUint32At(buffer, 5, 1000);
    setUint16At(buffer, 9, 60);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcessPacketDispatchTest, DispatchHeartbeatType) {
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_HEARTBEAT);
    
    auto buffer = createBuffer(4);
    setUint32At(buffer, 0, 1);
    setPacketData(buffer);

    auto result = nm.processPacket(packet);
    EXPECT_TRUE(result.has_value());
}

// ============================================================================
// createPacket Dispatch Tests
// ============================================================================

class CreatePacketDispatchTest : public ::testing::Test {
protected:
    PacketManager manager;
};

TEST_F(CreatePacketDispatchTest, UnknownTypeReturnsNullopt) {
    auto result = manager.createPacket(static_cast<PacketTypes>(0xFF), {});
    EXPECT_FALSE(result.has_value());
}

TEST_F(CreatePacketDispatchTest, CreatesHeartbeatPacket) {
    // flags_count(1) + flags(1) + sequence(4) + timestamp(4) + payload(4)
    std::vector<uint8_t> args;
    args.push_back(1);       // flags_count
    args.push_back(0x01);    // FLAG_RELIABLE
    uint32_t seq = 10;
    uint32_t ts = 20;
    uint32_t playerId = 2;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&seq), reinterpret_cast<uint8_t*>(&seq) + sizeof(uint32_t));
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&ts), reinterpret_cast<uint8_t*>(&ts) + sizeof(uint32_t));
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&playerId), reinterpret_cast<uint8_t*>(&playerId) + sizeof(uint32_t));

    auto result = manager.createPacket(PacketTypes::TYPE_HEARTBEAT, args);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->header.packet_type, static_cast<uint8_t>(PacketTypes::TYPE_HEARTBEAT));
    EXPECT_EQ(result->header.sequence_number, seq);
    EXPECT_EQ(result->header.timestamp, ts);
    ASSERT_EQ(result->data.size(), 4u);
    uint32_t parsedPlayerId;
    std::memcpy(&parsedPlayerId, result->data.data(), sizeof(uint32_t));
    EXPECT_EQ(parsedPlayerId, playerId);
}

// ============================================================================
// Integration Tests
// ============================================================================

class ProcessPacketIntegrationTest : public ProcessPacketTest {
};

TEST_F(ProcessPacketIntegrationTest, SequenceOfValidPackets) {
    // First packet
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    packet.header.sequence_number = 1;
    
    auto buffer1 = createBuffer(37);
    setUint8At(buffer1, 0, 1);
    setStringAt(buffer1, 1, "Player1", 31);
    setUint32At(buffer1, 33, 100);
    setPacketData(buffer1);

    auto result1 = nm.processPacket(packet);
    EXPECT_TRUE(result1.has_value());

    // Second packet
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_HEARTBEAT);
    packet.header.sequence_number = 2;
    
    auto buffer2 = createBuffer(4);
    setUint32At(buffer2, 0, 100);
    setPacketData(buffer2);

    auto result2 = nm.processPacket(packet);
    EXPECT_TRUE(result2.has_value());

    // Verify independence
    EXPECT_NE(result1->header.sequence_number, result2->header.sequence_number);
    EXPECT_NE(result1->header.packet_type, result2->header.packet_type);
}

TEST_F(ProcessPacketIntegrationTest, MixedValidAndInvalidPackets) {
    // Valid packet
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_CLIENT_CONNECT);
    
    auto buffer1 = createBuffer(37);
    setUint8At(buffer1, 0, 1);
    setStringAt(buffer1, 1, "Player", 31);
    setUint32At(buffer1, 33, 1);
    setPacketData(buffer1);

    auto result1 = nm.processPacket(packet);
    EXPECT_TRUE(result1.has_value());

    // Invalid packet - bad magic
    packet.header.magic = 0x0000;
    auto buffer2 = createBuffer(37);
    setPacketData(buffer2);

    auto result2 = nm.processPacket(packet);
    EXPECT_FALSE(result2.has_value());

    // Valid packet again
    packet.header.magic = 0x5254;
    packet.header.packet_type = static_cast<uint8_t>(PacketTypes::TYPE_HEARTBEAT);
    
    auto buffer3 = createBuffer(4);
    setUint32At(buffer3, 0, 1);
    setPacketData(buffer3);

    auto result3 = nm.processPacket(packet);
    EXPECT_TRUE(result3.has_value());
}
