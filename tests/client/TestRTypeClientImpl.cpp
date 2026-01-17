/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestRTypeClientImpl
*/

#include <client/RTypeClient.hpp>
#include <client/network/ClientNetworkManager.hpp>
#include <common/protocol/Packet.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <optional>

using namespace std::chrono_literals;

// Stub network manager to avoid real networking during tests
class StubNetworkManager : public client::network::ClientNetworkManager {
public:
    StubNetworkManager() : client::network::ClientNetworkManager("0.0.0.0", 0, nullptr) {}

    void start() override { started = true; }
    void stop() override { stopped = true; }
    bool isRunning() const override { return started && !stopped; }

    void queueOutgoing(const common::protocol::Packet &, std::optional<uint32_t> = std::nullopt) override {
        queuedCount++;
    }

    std::vector<common::network::ReceivedPacket> fetchIncoming() override { return {}; }

    void run() override { runCalled = true; }
    void sendConnectionRequest() override { sendCalled = true; }

    bool started = false;
    bool stopped = false;
    bool runCalled = false;
    bool sendCalled = false;
    int queuedCount = 0;
};

class RTypeClientImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        client = std::make_unique<RTypeClient>();
    }

    void TearDown() override {
        if (runner.joinable()) {
            client->stop();
            runner.join();
        }
        client.reset();
    }

    std::unique_ptr<RTypeClient> client;
    std::thread runner;
};

// Constructor and destructor paths
TEST_F(RTypeClientImplTest, ConstructorDestructor) {
    auto local = std::make_unique<RTypeClient>();
    EXPECT_FALSE(local->isRunningFlag());
    EXPECT_FALSE(local->isConnectedFlag());
    local.reset();
}
