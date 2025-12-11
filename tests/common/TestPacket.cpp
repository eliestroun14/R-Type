/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Packet unit tests
*/

#include <gtest/gtest.h>
#include <common/protocol/Packet.hpp>
#include <vector>
#include <cstring>

using namespace common::protocol;

class PacketTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test Packet Constructor
class PacketConstructorTest : public PacketTest {
};

TEST_F(PacketConstructorTest, DefaultConstructor) {
    Packet packet(0x01);
    
    EXPECT_EQ(packet.header.magic, 0x5254);
    EXPECT_EQ(packet.header.packet_type, 0x01);
    EXPECT_EQ(packet.header.flags, 0);
    EXPECT_EQ(packet.header.sequence_number, 0);
    EXPECT_EQ(packet.header.timestamp, 0);
    EXPECT_TRUE(packet.data.empty());
}

TEST_F(PacketConstructorTest, PacketTypeZero) {
    Packet packet(0x00);
    
    EXPECT_EQ(packet.header.magic, 0x5254);
    EXPECT_EQ(packet.header.packet_type, 0x00);
    EXPECT_EQ(packet.header.flags, 0);
    EXPECT_EQ(packet.header.sequence_number, 0);
    EXPECT_EQ(packet.header.timestamp, 0);
}

TEST_F(PacketConstructorTest, PacketTypeMax) {
    Packet packet(0xFF);
    
    EXPECT_EQ(packet.header.magic, 0x5254);
    EXPECT_EQ(packet.header.packet_type, 0xFF);
    EXPECT_EQ(packet.header.flags, 0);
}

TEST_F(PacketConstructorTest, MultiplePacketCreation) {
    Packet packet1(0x01);
    Packet packet2(0x02);
    Packet packet3(0x03);
    
    EXPECT_EQ(packet1.header.packet_type, 0x01);
    EXPECT_EQ(packet2.header.packet_type, 0x02);
    EXPECT_EQ(packet3.header.packet_type, 0x03);
    
    // Verify they don't interfere with each other
    EXPECT_EQ(packet1.header.magic, packet2.header.magic);
    EXPECT_EQ(packet2.header.magic, packet3.header.magic);
}

// Test Packet Serialization
class PacketSerializeTest : public PacketTest {
};

TEST_F(PacketSerializeTest, SerializeEmptyPacket) {
    Packet packet(0x01);
    std::vector<uint8_t> buffer;
    
    packet.serialize(buffer);
    
    // Buffer should remain empty (not implemented)
    // This test verifies the function doesn't crash
}

TEST_F(PacketSerializeTest, SerializePacketWithData) {
    Packet packet(0x02);
    packet.data = {0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> buffer;
    
    packet.serialize(buffer);
    
    // Buffer should remain empty (not implemented)
    // This test verifies the function doesn't crash
}

TEST_F(PacketSerializeTest, SerializeMultiplePackets) {
    Packet packet1(0x01);
    Packet packet2(0x02);
    Packet packet3(0x03);
    
    std::vector<uint8_t> buffer1, buffer2, buffer3;
    
    packet1.serialize(buffer1);
    packet2.serialize(buffer2);
    packet3.serialize(buffer3);
    
    // All buffers should remain empty (not implemented)
}

TEST_F(PacketSerializeTest, SerializeWithLargePayload) {
    Packet packet(0x01);
    packet.data.resize(10000);
    std::fill(packet.data.begin(), packet.data.end(), 0xAB);
    
    std::vector<uint8_t> buffer;
    packet.serialize(buffer);
    
    // Function shouldn't crash with large payload
}

TEST_F(PacketSerializeTest, SerializeAfterModifyingHeaders) {
    Packet packet(0x01);
    packet.header.flags = 0xFF;
    packet.header.sequence_number = 12345;
    packet.header.timestamp = 0xDEADBEEF;
    
    std::vector<uint8_t> buffer;
    packet.serialize(buffer);
    
    // Function shouldn't crash
}

// Test Packet Deserialization
class PacketDeserializeTest : public PacketTest {
};

TEST_F(PacketDeserializeTest, DeserializeEmptyBuffer) {
    Packet packet(0x00);
    std::vector<uint8_t> buffer;
    
    bool result = packet.deserialize(buffer);
    
    EXPECT_FALSE(result);
}

TEST_F(PacketDeserializeTest, DeserializeSmallBuffer) {
    Packet packet(0x00);
    std::vector<uint8_t> buffer = {0x01, 0x02, 0x03};
    
    bool result = packet.deserialize(buffer);
    
    EXPECT_FALSE(result);
}

TEST_F(PacketDeserializeTest, DeserializeValidBuffer) {
    Packet packet(0x00);
    std::vector<uint8_t> buffer(20, 0x00);
    
    bool result = packet.deserialize(buffer);
    
    EXPECT_FALSE(result); // Not implemented
}

TEST_F(PacketDeserializeTest, DeserializeMultipleTimes) {
    Packet packet(0x00);
    std::vector<uint8_t> buffer1(20, 0x01);
    std::vector<uint8_t> buffer2(20, 0x02);
    
    bool result1 = packet.deserialize(buffer1);
    bool result2 = packet.deserialize(buffer2);
    
    EXPECT_FALSE(result1);
    EXPECT_FALSE(result2);
}

TEST_F(PacketDeserializeTest, DeserializeWithDifferentSizes) {
    Packet packet(0x00);
    
    for (size_t size = 0; size <= 100; ++size) {
        std::vector<uint8_t> buffer(size, 0xAB);
        bool result = packet.deserialize(buffer);
        EXPECT_FALSE(result); // Not implemented
    }
}

TEST_F(PacketDeserializeTest, DeserializeDoesNotCrash) {
    Packet packet(0x01);
    std::vector<uint8_t> buffer(1000);
    std::fill(buffer.begin(), buffer.end(), 0xFF);
    
    bool result = packet.deserialize(buffer);
    
    EXPECT_FALSE(result);
}

TEST_F(PacketDeserializeTest, DeserializeAfterConstruction) {
    Packet packet(0x05);
    
    EXPECT_EQ(packet.header.packet_type, 0x05);
    
    std::vector<uint8_t> buffer(50, 0x00);
    bool result = packet.deserialize(buffer);
    
    EXPECT_FALSE(result);
    // Verify packet type is not changed by failed deserialization
    EXPECT_EQ(packet.header.packet_type, 0x05);
}

// Test Packet Data Management
class PacketDataTest : public PacketTest {
};

TEST_F(PacketDataTest, DataInitiallyEmpty) {
    Packet packet(0x01);
    
    EXPECT_TRUE(packet.data.empty());
    EXPECT_EQ(packet.data.size(), 0);
}

TEST_F(PacketDataTest, CanAddDataToPacket) {
    Packet packet(0x01);
    packet.data = {0x01, 0x02, 0x03, 0x04};
    
    EXPECT_EQ(packet.data.size(), 4);
    EXPECT_EQ(packet.data[0], 0x01);
    EXPECT_EQ(packet.data[3], 0x04);
}

TEST_F(PacketDataTest, CanClearDataFromPacket) {
    Packet packet(0x01);
    packet.data = {0x01, 0x02, 0x03};
    
    EXPECT_FALSE(packet.data.empty());
    
    packet.data.clear();
    
    EXPECT_TRUE(packet.data.empty());
}

TEST_F(PacketDataTest, CanModifyDataInPacket) {
    Packet packet(0x01);
    packet.data = {0x01, 0x02, 0x03};
    
    packet.data[1] = 0xFF;
    
    EXPECT_EQ(packet.data[1], 0xFF);
}

TEST_F(PacketDataTest, LargeDataPayload) {
    Packet packet(0x01);
    const size_t large_size = 65536;
    packet.data.resize(large_size);
    std::fill(packet.data.begin(), packet.data.end(), 0xAB);
    
    EXPECT_EQ(packet.data.size(), large_size);
    EXPECT_EQ(packet.data.front(), 0xAB);
    EXPECT_EQ(packet.data.back(), 0xAB);
}

// Test Header Structure
class PacketHeaderTest : public PacketTest {
};

TEST_F(PacketHeaderTest, HeaderMagicNumber) {
    Packet packet(0x01);
    
    EXPECT_EQ(packet.header.magic, 0x5254);
}

TEST_F(PacketHeaderTest, HeaderFlagsModification) {
    Packet packet(0x01);
    
    packet.header.flags = 0x01;
    EXPECT_EQ(packet.header.flags, 0x01);
    
    packet.header.flags = 0xFF;
    EXPECT_EQ(packet.header.flags, 0xFF);
    
    packet.header.flags = 0x00;
    EXPECT_EQ(packet.header.flags, 0x00);
}

TEST_F(PacketHeaderTest, HeaderSequenceNumberModification) {
    Packet packet(0x01);
    
    packet.header.sequence_number = 12345;
    EXPECT_EQ(packet.header.sequence_number, 12345);
    
    packet.header.sequence_number = 0xFFFFFFFF;
    EXPECT_EQ(packet.header.sequence_number, 0xFFFFFFFF);
}

TEST_F(PacketHeaderTest, HeaderTimestampModification) {
    Packet packet(0x01);
    
    packet.header.timestamp = 0x12345678;
    EXPECT_EQ(packet.header.timestamp, 0x12345678);
    
    packet.header.timestamp = 0xDEADBEEF;
    EXPECT_EQ(packet.header.timestamp, 0xDEADBEEF);
}

TEST_F(PacketHeaderTest, HeaderPacketTypeModification) {
    Packet packet(0x01);
    
    EXPECT_EQ(packet.header.packet_type, 0x01);
    
    packet.header.packet_type = 0x50;
    EXPECT_EQ(packet.header.packet_type, 0x50);
}

TEST_F(PacketHeaderTest, IndependentHeaderFields) {
    Packet packet(0x01);
    
    packet.header.flags = 0xAA;
    packet.header.sequence_number = 1111;
    packet.header.timestamp = 2222;
    
    EXPECT_EQ(packet.header.magic, 0x5254);
    EXPECT_EQ(packet.header.packet_type, 0x01);
    EXPECT_EQ(packet.header.flags, 0xAA);
    EXPECT_EQ(packet.header.sequence_number, 1111);
    EXPECT_EQ(packet.header.timestamp, 2222);
}

// Test Packet Copy Behavior
class PacketCopyTest : public PacketTest {
};

TEST_F(PacketCopyTest, CopyConstructor) {
    Packet packet1(0x01);
    packet1.header.flags = 0x42;
    packet1.header.sequence_number = 999;
    packet1.data = {0x01, 0x02, 0x03};
    
    Packet packet2 = packet1;
    
    EXPECT_EQ(packet2.header.packet_type, 0x01);
    EXPECT_EQ(packet2.header.flags, 0x42);
    EXPECT_EQ(packet2.header.sequence_number, 999);
    EXPECT_EQ(packet2.data.size(), 3);
}

TEST_F(PacketCopyTest, AssignmentOperator) {
    Packet packet1(0x01);
    packet1.header.flags = 0x42;
    packet1.data = {0x01, 0x02, 0x03};
    
    Packet packet2(0x02);
    packet2 = packet1;
    
    EXPECT_EQ(packet2.header.packet_type, 0x01);
    EXPECT_EQ(packet2.header.flags, 0x42);
    EXPECT_EQ(packet2.data.size(), 3);
}

TEST_F(PacketCopyTest, MoveSemantics) {
    Packet packet1(0x01);
    packet1.data = {0x01, 0x02, 0x03};
    
    Packet packet2(0x02);
    packet2 = std::move(packet1);
    
    EXPECT_EQ(packet2.header.packet_type, 0x01);
    EXPECT_EQ(packet2.data.size(), 3);
}
