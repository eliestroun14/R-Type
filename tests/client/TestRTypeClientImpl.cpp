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

// init sets up network manager, game, and flags
/*DISABLED: InitSetsFields
TEST_F(RTypeClientImplTest, InitSetsFields) {
    client->init("127.0.0.1", 4242, "PlayerA");

    ASSERT_NE(client->getNetworkManager(), nullptr);
    ASSERT_NE(client->getGame(), nullptr);
    EXPECT_EQ(client->getPlayerName(), "PlayerA");
    EXPECT_FALSE(client->isRunningFlag());
    EXPECT_FALSE(client->isConnectedFlag());
}
*/

// stop should flip running to false and call network stop when present
/*DISABLED: StopCallsNetworkStop
TEST_F(RTypeClientImplTest, StopCallsNetworkStop) {
    client->init("127.0.0.1", 4242, "PlayerStop");
    auto stub = std::make_unique<StubNetworkManager>();
    auto *stubPtr = stub.get();
    client->setNetworkManager(std::move(stub));

    client->stop();

    EXPECT_FALSE(client->isRunningFlag());
    EXPECT_TRUE(stubPtr->stopped);
}
*/

// run should start, send connection request, launch run thread, and stop gracefully
/*DISABLED: RunStartsAndStopsGracefully
TEST_F(RTypeClientImplTest, RunStartsAndStopsGracefully) {
    client->init("127.0.0.1", 4242, "Runner");

    auto stub = std::make_unique<StubNetworkManager>();
    auto *stubPtr = stub.get();
    client->setNetworkManager(std::move(stub));

    // Remove game to avoid heavy game loop
    client->resetGame();

    runner = std::thread([this]() { client->run(); });

    // Allow run() to enter loop
    std::this_thread::sleep_for(30ms);
    client->stop();
    if (runner.joinable()) {
        runner.join();
    }

    EXPECT_TRUE(stubPtr->started);
    EXPECT_TRUE(stubPtr->sendCalled);
    EXPECT_TRUE(stubPtr->runCalled);
    EXPECT_TRUE(stubPtr->stopped);
    EXPECT_FALSE(client->isRunningFlag());
}
*/

// run should queue outgoing packets from game loop when present (simulated via manual enqueue)
/*DISABLED: RunQueuesOutgoingPackets
TEST_F(RTypeClientImplTest, RunQueuesOutgoingPackets) {
    client->init("127.0.0.1", 4242, "QueueTest");
    auto stub = std::make_unique<StubNetworkManager>();
    auto *stubPtr = stub.get();
    client->setNetworkManager(std::move(stub));

    // Simulate a single loop iteration by manually toggling flags
    client->setRunning(true);
    client->resetGame();

    // Manually mimic main loop behaviour
    client->getNetworkManager()->start();
    client->getNetworkManager()->sendConnectionRequest();
    client->setRunning(false);

    EXPECT_TRUE(stubPtr->started);
    EXPECT_TRUE(stubPtr->sendCalled);
    EXPECT_EQ(stubPtr->queuedCount, 0);
}
*/
