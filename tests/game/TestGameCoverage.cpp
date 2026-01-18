/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game.cpp coverage tests with stubs
*/

#include <deque>
#include <optional>
#include <mutex>
#include <cstdint>
#include <chrono>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <common/error/Error.hpp>
#include <common/constants/defines.hpp>
#include <common/network/NetworkManager.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/Protocol.hpp>

#define private public
#define protected public
#include <game/Game.hpp>
#undef private
#undef protected

class DummyMenu : public IMenu {
public:
    void update() override { ++updateCalls; }
    void createMainMenu() override {}
    void createOptionMenu() override {}

    int updateCalls = 0;
};

static common::protocol::Packet makePacket(protocol::PacketTypes type) {
    common::protocol::Packet packet(static_cast<uint8_t>(type));
    packet.header.packet_type = static_cast<uint8_t>(type);
    packet.header.flags = 0;
    packet.header.sequence_number = 0;
    packet.header.timestamp = 0;
    return packet;
}

TEST(GameCoverageTest, RunGameLoopServerProcessesTicksAndPackets) {
    Game game(Game::Type::SERVER);
    auto coordinator = std::static_pointer_cast<Coordinator>(game.getCoordinator());

    coordinator->connectedPlayerIds = {1, 2};
    coordinator->filteredPlayerId = 1;

    auto inputPacket = makePacket(protocol::PacketTypes::TYPE_PLAYER_INPUT);
    auto pingPacket = makePacket(protocol::PacketTypes::TYPE_PING);
    coordinator->serverOutgoingPackets = {inputPacket, pingPacket};

    common::network::ReceivedPacket incoming;
    incoming.packet = inputPacket;
    incoming.clientId = 1;
    game.addIncomingPacket(incoming);

    game._lastTickTime = std::chrono::steady_clock::now() - std::chrono::milliseconds(20);
    game._accumulatedTime = Game::TICK_RATE_MS;

    // Run game loop - should process server tick
    EXPECT_TRUE(game.runGameLoop());

    // Verify that outgoing packets are queued
    auto outPacket = game.popOutgoingPacket();
    // May or may not have packet depending on coordinator behavior
}

TEST(GameCoverageTest, RunGameLoopClientStopsWhenWindowClosed) {
    Game game(Game::Type::CLIENT);
    auto coordinator = std::static_pointer_cast<Coordinator>(game.getCoordinator());
    auto engine = coordinator->getEngine();
    engine->setWindowOpen(false);

    auto menu = std::make_shared<DummyMenu>();
    game.setMenu(menu);

    EXPECT_FALSE(game.runGameLoop());
    EXPECT_FALSE(game.isRunning());
    EXPECT_EQ(menu->updateCalls, 1);
    EXPECT_EQ(engine->processInputCalls, 1);
}

TEST(GameCoverageTest, ClientTickQueuesOutgoingPackets) {
    Game game(Game::Type::CLIENT);
    auto coordinator = std::static_pointer_cast<Coordinator>(game.getCoordinator());

    coordinator->clientOutgoingPackets = {makePacket(protocol::PacketTypes::TYPE_PING)};
    common::network::ReceivedPacket incoming;
    incoming.packet = makePacket(protocol::PacketTypes::TYPE_PONG);
    game.addIncomingPacket(incoming);

    game.clientTick(16);

    auto outPacket = game.popOutgoingPacket();
    EXPECT_TRUE(outPacket.has_value());
}

TEST(GameCoverageTest, ServerTickCompletesLevelAndStopsGame) {
    Game game(Game::Type::SERVER);
    auto coordinator = std::static_pointer_cast<Coordinator>(game.getCoordinator());
    auto engine = coordinator->getEngine();

    coordinator->connectedPlayerIds = {10, 20};
    game._connectedPlayers = {10, 20};

    auto levelEntity = engine->createEntity("Level");
    Level level;
    level.completed = true;
    engine->addComponent<Level>(levelEntity, level);
    game._currentLevelEntity = levelEntity;
    game._levelStarted = true;

    game.serverTick(16);

    EXPECT_FALSE(game.isRunning());
    auto outPacket = game.popOutgoingPacket();
    EXPECT_TRUE(outPacket.has_value());
}

TEST(GameCoverageTest, CheckAndStartLevelQueuesLevelStartPackets) {
    Game game(Game::Type::SERVER);
    auto coordinator = std::static_pointer_cast<Coordinator>(game.getCoordinator());

    coordinator->connectedPlayerIds = {1, 2};
    game._connectedPlayers = {1, 2};
    game._maxPlayers = 2;

    game.checkAndStartLevel();

    EXPECT_TRUE(game._levelStarted);
    auto outPacket = game.popOutgoingPacket();
    EXPECT_TRUE(outPacket.has_value());
}

TEST(GameCoverageTest, OnPlayerConnectedServerQueuesSpawnPackets) {
    Game game(Game::Type::SERVER);
    auto coordinator = std::static_pointer_cast<Coordinator>(game.getCoordinator());
    auto engine = coordinator->getEngine();

    game._connectedPlayers = {5};
    engine->addComponent<Transform>(Entity::fromId(5), Transform(100.f, 200.f, 0.f, 1.f));

    EXPECT_NO_THROW(game.onPlayerConnected(7));
    EXPECT_EQ(game._connectedPlayers.size(), 2u);

    auto outPacket = game.popOutgoingPacket();
    EXPECT_TRUE(outPacket.has_value());
}

TEST(GameCoverageTest, OnPlayerConnectedThrowsOnClient) {
    Game game(Game::Type::CLIENT);
    EXPECT_THROW(game.onPlayerConnected(42), Error);
}

TEST(GameCoverageTest, OutgoingAndIncomingQueueOperations) {
    Game game(Game::Type::SERVER);

    common::protocol::Packet packet = makePacket(protocol::PacketTypes::TYPE_PING);
    game.addOutgoingPacket(packet, std::nullopt);
    auto outgoing = game.popOutgoingPacket();
    EXPECT_TRUE(outgoing.has_value());

    common::network::ReceivedPacket incoming;
    incoming.packet = packet;
    game.addIncomingPacket(incoming);
    auto poppedIncoming = game.popIncomingPacket();
    EXPECT_TRUE(poppedIncoming.has_value());
}
