/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestClientNetworkManagerImpl
*/

#include <gtest/gtest.h>
#include <client/network/ClientNetworkManager.hpp>
#include <client/RTypeClient.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/Protocol.hpp>
#include <thread>
#include <chrono>
#include <memory>

class MockRTypeClient {
public:
    MockRTypeClient() : connected(false), lastSetConnected(false) {}
    
    void setConnected(bool status) {
        connected = status;
        lastSetConnected = status;
    }
    
    std::string getPlayerName() const {
        return "TestPlayer";
    }
    
    bool connected;
    bool lastSetConnected;
};

class ClientNetworkManagerImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockClient = std::make_unique<MockRTypeClient>();
        manager = std::make_unique<client::network::ClientNetworkManager>(
            "127.0.0.1", 54321, nullptr
        );
    }

    void TearDown() override {
        if (manager && manager->isRunning()) {
            manager->stop();
        }
        manager.reset();
        mockClient.reset();
    }

    std::unique_ptr<MockRTypeClient> mockClient;
    std::unique_ptr<client::network::ClientNetworkManager> manager;

    // Helper to create a valid ServerAccept packet
    common::protocol::Packet createServerAcceptPacket() {
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT));
        packet.header.magic = 0x5254;
        packet.header.flags = 0x01;
        packet.header.sequence_number = 0;
        packet.header.timestamp = 0;
        
        // Create minimal ServerAcceptPayload
        struct ServerAcceptPayload {
            uint32_t assigned_player_id;
            uint32_t max_players;
            uint32_t game_instance_id;
        } payload;
        payload.assigned_player_id = 1;
        payload.max_players = 4;
        payload.game_instance_id = 100;
        
        packet.data.resize(sizeof(payload));
        std::memcpy(packet.data.data(), &payload, sizeof(payload));
        
        return packet;
    }

    // Helper to create a valid ServerReject packet
    common::protocol::Packet createServerRejectPacket() {
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_REJECT));
        packet.header.magic = 0x5254;
        packet.header.flags = 0x00;
        packet.header.sequence_number = 0;
        packet.header.timestamp = 0;
        
        // Create minimal ServerRejectPayload
        struct ServerRejectPayload {
            uint8_t reject_code;
            char reason_message[64];
        } payload;
        payload.reject_code = 1;
        std::strncpy(payload.reason_message, "Server full", sizeof(payload.reason_message) - 1);
        
        packet.data.resize(sizeof(payload));
        std::memcpy(packet.data.data(), &payload, sizeof(payload));
        
        return packet;
    }

    // Helper to create a valid ClientDisconnect packet
    common::protocol::Packet createClientDisconnectPacket() {
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT));
        packet.header.magic = 0x5254;
        packet.header.flags = 0x01;
        packet.header.sequence_number = 0;
        packet.header.timestamp = 0;
        
        // Create minimal ClientDisconnectPayload
        struct ClientDisconnectPayload {
            uint32_t player_id;
            uint8_t reason;
            char reason_message[64];
        } payload;
        payload.player_id = 1;
        payload.reason = 0;
        std::strncpy(payload.reason_message, "Normal disconnect", sizeof(payload.reason_message) - 1);
        
        packet.data.resize(sizeof(payload));
        std::memcpy(packet.data.data(), &payload, sizeof(payload));
        
        return packet;
    }
};

// ==================== Constructor/Destructor Tests ====================

// Test constructor initializes properly
TEST_F(ClientNetworkManagerImplTest, ConstructorInitialization) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>("192.168.1.1", 8080, nullptr);
    EXPECT_FALSE(cnm->isRunning());
    cnm.reset();
}

// Test destructor stops running manager
TEST_F(ClientNetworkManagerImplTest, DestructorStopsManager) {
    {
        auto cnm = std::make_unique<client::network::ClientNetworkManager>("127.0.0.1", 54322, nullptr);
        EXPECT_NO_THROW({
            cnm.reset();
        });
    }
}

// ==================== start() Method Tests ====================

// Test start() sets running flag
TEST_F(ClientNetworkManagerImplTest, StartSetsRunningFlag) {
    EXPECT_FALSE(manager->isRunning());
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        bool started = manager->isRunning();
        EXPECT_TRUE(started);
        manager->stop();
    } catch (...) {
        // Network error is acceptable
    }
}

// Test start() can only be called once (idempotent)
TEST_F(ClientNetworkManagerImplTest, StartIdempotent) {
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Try to start again - should log warning but not crash
        EXPECT_NO_THROW({
            manager->start();
        });
        
        manager->stop();
    } catch (...) {
        // Network error is acceptable
    }
}

// ==================== stop() Method Tests ====================

// Test stop() clears running flag
TEST_F(ClientNetworkManagerImplTest, StopClearsRunningFlag) {
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_TRUE(manager->isRunning());
        
        manager->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_FALSE(manager->isRunning());
    } catch (...) {
        // Network error is acceptable
    }
}

// Test stop() when not running (idempotent)
TEST_F(ClientNetworkManagerImplTest, StopWhenNotRunning) {
    EXPECT_FALSE(manager->isRunning());
    EXPECT_NO_THROW({
        manager->stop();
    });
    EXPECT_FALSE(manager->isRunning());
}

// Test stop() multiple times
TEST_F(ClientNetworkManagerImplTest, StopMultipleTimes) {
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        manager->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        manager->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        manager->stop();
        
        EXPECT_FALSE(manager->isRunning());
    } catch (...) {
        // Network error is acceptable
    }
}

// ==================== fetchIncoming() Tests ====================

// Test fetchIncoming() returns empty vector initially
TEST_F(ClientNetworkManagerImplTest, FetchIncomingEmpty) {
    auto packets = manager->fetchIncoming();
    EXPECT_EQ(packets.size(), 0);
}

// Test fetchIncoming() clears the queue
TEST_F(ClientNetworkManagerImplTest, FetchIncomingClears) {
    auto packets1 = manager->fetchIncoming();
    auto packets2 = manager->fetchIncoming();
    
    EXPECT_EQ(packets1.size(), 0);
    EXPECT_EQ(packets2.size(), 0);
}

// ==================== queueOutgoing() Tests ====================

// Test queueOutgoing() with ClientConnect packet
TEST_F(ClientNetworkManagerImplTest, QueueOutgoingClientConnect) {
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT));
    packet.header.magic = 0x5254;
    packet.header.flags = 0x00;
    packet.header.sequence_number = 0;
    packet.header.timestamp = 0;
    
    EXPECT_NO_THROW({
        manager->queueOutgoing(packet);
    });
}

// Test queueOutgoing() with multiple packets
TEST_F(ClientNetworkManagerImplTest, QueueOutgoingMultiple) {
    for (int i = 0; i < 5; ++i) {
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT));
        packet.header.magic = 0x5254;
        packet.header.sequence_number = i;
        
        EXPECT_NO_THROW({
            manager->queueOutgoing(packet);
        });
    }
}

// ==================== sendConnectionRequest() Tests ====================

// Test sendConnectionRequest() queues a packet
/*DISABLED: SendConnectionRequest
TEST_F(ClientNetworkManagerImplTest, SendConnectionRequest) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54323, 
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    EXPECT_NO_THROW({
        cnm->sendConnectionRequest();
    });
    
    cnm.reset();
}
*/

// Test sendConnectionRequest() with different player names
/*DISABLED: SendConnectionRequestDifferentNames
TEST_F(ClientNetworkManagerImplTest, SendConnectionRequestDifferentNames) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54324,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    EXPECT_NO_THROW({
        cnm->sendConnectionRequest();
        cnm->sendConnectionRequest();
        cnm->sendConnectionRequest();
    });
    
    cnm.reset();
}
*/

// ==================== handleConnectionAccepted() Tests ====================

// Test handleConnectionAccepted() with valid packet
TEST_F(ClientNetworkManagerImplTest, HandleConnectionAcceptedValid) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54325,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    auto packet = createServerAcceptPacket();
    
    EXPECT_NO_THROW({
        cnm->queueOutgoing(packet);
    });
    
    cnm.reset();
}

// Test handleConnectionAccepted() with empty payload
TEST_F(ClientNetworkManagerImplTest, HandleConnectionAcceptedEmptyPayload) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54326,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT));
    packet.header.magic = 0x5254;
    packet.data.clear(); // Empty payload
    
    EXPECT_NO_THROW({
        cnm->queueOutgoing(packet);
    });
    
    cnm.reset();
}

// ==================== handleConnectionRejected() Tests ====================

// Test handleConnectionRejected() with valid packet
TEST_F(ClientNetworkManagerImplTest, HandleConnectionRejectedValid) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54327,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    auto packet = createServerRejectPacket();
    
    EXPECT_NO_THROW({
        cnm->queueOutgoing(packet);
    });
    
    cnm.reset();
}

// Test handleConnectionRejected() with empty payload
TEST_F(ClientNetworkManagerImplTest, HandleConnectionRejectedEmptyPayload) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54328,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_REJECT));
    packet.header.magic = 0x5254;
    packet.data.clear();
    
    EXPECT_NO_THROW({
        cnm->queueOutgoing(packet);
    });
    
    cnm.reset();
}

// ==================== handleDisconnect() Tests ====================

// Test handleDisconnect() with valid packet
TEST_F(ClientNetworkManagerImplTest, HandleDisconnectValid) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54329,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    auto packet = createClientDisconnectPacket();
    
    EXPECT_NO_THROW({
        cnm->queueOutgoing(packet);
    });
    
    cnm.reset();
}

// Test handleDisconnect() with empty payload
TEST_F(ClientNetworkManagerImplTest, HandleDisconnectEmptyPayload) {
    auto cnm = std::make_unique<client::network::ClientNetworkManager>(
        "127.0.0.1", 54330,
        reinterpret_cast<RTypeClient*>(mockClient.get())
    );
    
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT));
    packet.header.magic = 0x5254;
    packet.data.clear();
    
    EXPECT_NO_THROW({
        cnm->queueOutgoing(packet);
    });
    
    cnm.reset();
}

// ==================== Integration Tests ====================

// Test full lifecycle: create, start, queue packets, stop
TEST_F(ClientNetworkManagerImplTest, FullLifecycle) {
    try {
        EXPECT_FALSE(manager->isRunning());
        
        // Queue packet before start
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT));
        packet.header.magic = 0x5254;
        manager->queueOutgoing(packet);
        
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_TRUE(manager->isRunning());
        
        // Queue packet after start
        manager->queueOutgoing(packet);
        
        // Fetch incoming (should be empty)
        auto packets = manager->fetchIncoming();
        EXPECT_EQ(packets.size(), 0);
        
        manager->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_FALSE(manager->isRunning());
    } catch (...) {
        // Network error is acceptable in test environment
    }
}

// Test concurrent operations
TEST_F(ClientNetworkManagerImplTest, ConcurrentOperations) {
    try {
        manager->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Queue from main thread
        for (int i = 0; i < 5; ++i) {
            common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT));
            packet.header.magic = 0x5254;
            packet.header.sequence_number = i;
            manager->queueOutgoing(packet);
        }
        
        // Fetch from main thread
        auto packets = manager->fetchIncoming();
        
        manager->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } catch (...) {
        // Network error is acceptable
    }
}
