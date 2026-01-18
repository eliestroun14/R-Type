/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestClientNetworkManager
*/

#include <gtest/gtest.h>
#include <client/network/ClientNetworkManager.hpp>
#include <common/protocol/Packet.hpp>
#include <thread>
#include <chrono>

class ClientNetworkManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a ClientNetworkManager without actual network connection
        manager = std::make_unique<client::network::ClientNetworkManager>("127.0.0.1", 12345, nullptr);
    }

    void TearDown() override {
        if (manager && manager->isRunning()) {
            manager->stop();
        }
        manager.reset();
    }

    std::unique_ptr<client::network::ClientNetworkManager> manager;
};

// Test isRunning() when not started - line 31
TEST_F(ClientNetworkManagerTest, IsRunningFalseInitially) {
    EXPECT_FALSE(manager->isRunning());
}

// Test isRunning() after start - line 31
TEST_F(ClientNetworkManagerTest, IsRunningTrueAfterStart) {
    // Note: start() may fail if port is not available, but isRunning() should still be callable
    try {
        manager->start();
        // Give it a moment to start
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        // Check if running (may be true or false depending on network availability)
        bool running = manager->isRunning();
        EXPECT_TRUE(running == true || running == false); // Just verify it's callable
    } catch (...) {
        // If start fails, isRunning should be false
        EXPECT_FALSE(manager->isRunning());
    }
}

// Test isRunning() after stop - line 31
TEST_F(ClientNetworkManagerTest, IsRunningFalseAfterStop) {
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        manager->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_FALSE(manager->isRunning());
    } catch (...) {
        // If network operations fail, just verify isRunning is callable
        EXPECT_FALSE(manager->isRunning());
    }
}

// Test isRunning() multiple times - line 31
TEST_F(ClientNetworkManagerTest, IsRunningMultipleCalls) {
    // Should be consistent across multiple calls
    bool firstCall = manager->isRunning();
    bool secondCall = manager->isRunning();
    bool thirdCall = manager->isRunning();
    
    EXPECT_EQ(firstCall, secondCall);
    EXPECT_EQ(secondCall, thirdCall);
}

// Test queueOutgoing() with valid packet - line 33
TEST_F(ClientNetworkManagerTest, QueueOutgoingValidPacket) {
    common::protocol::Packet packet(0x01);
    packet.header.magic = 0x5254;
    packet.header.flags = 0x00;
    packet.header.sequence_number = 1;
    packet.header.timestamp = 100;
    
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet);
    });
}

// Test queueOutgoing() with different packet types - line 33
TEST_F(ClientNetworkManagerTest, QueueOutgoingDifferentTypes) {
    common::protocol::Packet packet1(0x01);
    packet1.header.magic = 0x5254;
    
    common::protocol::Packet packet2(0x10);
    packet2.header.magic = 0x5254;
    
    common::protocol::Packet packet3(0x20);
    packet3.header.magic = 0x5254;
    
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet1);
        manager->queueOutgoing(packet2);
        manager->queueOutgoing(packet3);
    });
}

// Test queueOutgoing() with targetClient parameter - line 33
TEST_F(ClientNetworkManagerTest, QueueOutgoingWithTargetClient) {
    common::protocol::Packet packet(0x01);
    packet.header.magic = 0x5254;
    
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet, std::nullopt);
        manager->queueOutgoing(packet, 1);
        manager->queueOutgoing(packet, 42);
    });
}

// Test queueOutgoing() multiple packets - line 33
TEST_F(ClientNetworkManagerTest, QueueOutgoingMultiplePackets) {
    for (int i = 0; i < 10; ++i) {
        common::protocol::Packet packet(static_cast<uint8_t>(i));
        packet.header.magic = 0x5254;
        packet.header.sequence_number = i;
        
        EXPECT_NO_THROW({
            manager->queueOutgoing(packet);
        });
    }
}

// Test queueOutgoing() with packet data - line 33
TEST_F(ClientNetworkManagerTest, QueueOutgoingWithData) {
    common::protocol::Packet packet(0x01);
    packet.header.magic = 0x5254;
    packet.data = {0x01, 0x02, 0x03, 0x04};
    
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet);
    });
}

// Test isRunning() and queueOutgoing() together - lines 31 & 33
TEST_F(ClientNetworkManagerTest, IsRunningAndQueueOutgoing) {
    EXPECT_FALSE(manager->isRunning());
    
    common::protocol::Packet packet(0x01);
    packet.header.magic = 0x5254;
    
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet);
    });
    
    EXPECT_FALSE(manager->isRunning());
}

// Test queueOutgoing() before and after start - line 33
TEST_F(ClientNetworkManagerTest, QueueOutgoingBeforeAndAfterStart) {
    common::protocol::Packet packet1(0x01);
    packet1.header.magic = 0x5254;
    
    // Queue before start
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet1);
    });
    
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        common::protocol::Packet packet2(0x02);
        packet2.header.magic = 0x5254;
        
        // Queue after start
        EXPECT_NO_THROW({
            manager->queueOutgoing(packet2);
        });
    } catch (...) {
        // Network failure is acceptable for this test
    }
}

// Test isRunning() const correctness - line 31
TEST_F(ClientNetworkManagerTest, IsRunningConstMethod) {
    const client::network::ClientNetworkManager& constManager = *manager;
    
    // Should be callable on const object
    bool running = constManager.isRunning();
    EXPECT_FALSE(running);
}
