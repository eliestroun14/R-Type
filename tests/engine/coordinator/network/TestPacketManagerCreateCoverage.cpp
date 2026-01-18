/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** PacketManager Create Coverage Tests
*/

#include <gtest/gtest.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include <common/constants/defines.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/PacketManager.hpp>
#include <common/protocol/Protocol.hpp>

namespace {
void writeUint32(std::vector<uint8_t> &buffer, size_t offset, uint32_t value) {
    std::memcpy(buffer.data() + offset, &value, sizeof(uint32_t));
}

void writeUint16(std::vector<uint8_t> &buffer, size_t offset, uint16_t value) {
    std::memcpy(buffer.data() + offset, &value, sizeof(uint16_t));
}

void writeInt16(std::vector<uint8_t> &buffer, size_t offset, int16_t value) {
    std::memcpy(buffer.data() + offset, &value, sizeof(int16_t));
}

void writeUint8(std::vector<uint8_t> &buffer, size_t offset, uint8_t value) {
    std::memcpy(buffer.data() + offset, &value, sizeof(uint8_t));
}

std::vector<uint8_t> buildArgs(uint8_t flagsCount,
                              const std::vector<uint8_t> &flags,
                              uint32_t sequenceNumber,
                              uint32_t timestamp,
                              const std::vector<uint8_t> &payload,
                              size_t minSize)
{
    std::vector<uint8_t> args;
    args.reserve(std::max(minSize, static_cast<size_t>(1 + flagsCount + 8 + payload.size())));

    args.push_back(flagsCount);
    for (uint8_t flag : flags) {
        args.push_back(flag);
    }

    args.insert(args.end(), reinterpret_cast<uint8_t*>(&sequenceNumber),
                reinterpret_cast<uint8_t*>(&sequenceNumber) + sizeof(uint32_t));
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp),
                reinterpret_cast<uint8_t*>(&timestamp) + sizeof(uint32_t));

    args.insert(args.end(), payload.begin(), payload.end());

    if (args.size() < minSize) {
        args.resize(minSize, 0);
    }

    return args;
}
}

TEST(PacketManagerCreateCoverage, CreatePacketAndFindHandler) {
    PacketManager manager;

    auto handler = PacketManager::findHandler(protocol::PacketTypes::TYPE_PING);
    EXPECT_NE(handler, nullptr);

    auto missingHandler = PacketManager::findHandler(static_cast<protocol::PacketTypes>(0xFF));
    EXPECT_EQ(missingHandler, nullptr);

    std::vector<uint8_t> pingPayload(PING_PAYLOAD_SIZE, 0);
    writeUint32(pingPayload, 0, 1234);
    auto pingArgs = buildArgs(0, {}, 42, 1337, pingPayload, PING_MIN_ARGS_SIZE + 1);

    auto pingPacket = manager.createPacket(protocol::PacketTypes::TYPE_PING, pingArgs);
    ASSERT_TRUE(pingPacket.has_value());
    EXPECT_EQ(pingPacket->header.packet_type, static_cast<uint8_t>(protocol::PacketTypes::TYPE_PING));

    auto invalidPacket = manager.createPacket(static_cast<protocol::PacketTypes>(0xFF), pingArgs);
    EXPECT_FALSE(invalidPacket.has_value());
}

TEST(PacketManagerCreateCoverage, CreateConnectionPackets) {
    std::vector<uint8_t> clientConnectPayload(CLIENT_CONNECT_PAYLOAD_SIZE, 0);
    writeUint8(clientConnectPayload, 0, PROTOCOL_VERSION);
    const char *playerName = "CoveragePlayer";
    std::memcpy(clientConnectPayload.data() + 1, playerName, std::strlen(playerName));
    writeUint32(clientConnectPayload, 33, 777);
    auto clientConnectArgs = buildArgs(0, {}, 1, 2, clientConnectPayload, CLIENT_CONNECT_MIN_ARGS_SIZE + 1);
    auto clientConnectPacket = PacketManager::createClientConnect(clientConnectArgs);
    ASSERT_TRUE(clientConnectPacket.has_value());
    EXPECT_EQ(clientConnectPacket->data.size(), CLIENT_CONNECT_PAYLOAD_SIZE);

    std::vector<uint8_t> clientDisconnectPayload(CLIENT_DISCONNECT_PAYLOAD_SIZE, 0);
    writeUint32(clientDisconnectPayload, 0, 55);
    writeUint8(clientDisconnectPayload, 4, static_cast<uint8_t>(protocol::DisconnectReasons::REASON_NORMAL_DISCONNECT));
    auto clientDisconnectArgs = buildArgs(0, {}, 11, 22, clientDisconnectPayload, CLIENT_DISCONNECT_MIN_ARGS_SIZE + 1);
    auto clientDisconnectPacket = PacketManager::createClientDisconnect(clientDisconnectArgs);
    ASSERT_TRUE(clientDisconnectPacket.has_value());
    EXPECT_EQ(clientDisconnectPacket->data.size(), CLIENT_DISCONNECT_PAYLOAD_SIZE);

    std::vector<uint8_t> heartbeatPayload(HEARTBEAT_PAYLOAD_SIZE, 0);
    writeUint32(heartbeatPayload, 0, 99);
    auto heartbeatArgs = buildArgs(0, {}, 33, 44, heartbeatPayload, HEARTBEAT_MIN_ARGS_SIZE + 1);
    auto heartbeatPacket = PacketManager::createHeartBeat(heartbeatArgs);
    ASSERT_TRUE(heartbeatPacket.has_value());
    EXPECT_EQ(heartbeatPacket->data.size(), HEARTBEAT_PAYLOAD_SIZE);

    std::vector<uint8_t> playerInputPayload(PLAYER_INPUT_PAYLOAD_SIZE, 0);
    writeUint32(playerInputPayload, 0, 1);
    writeUint16(playerInputPayload, 4, static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_RIGHT));
    writeUint16(playerInputPayload, 6, 100);
    writeUint16(playerInputPayload, 8, 200);
    auto playerInputArgs = buildArgs(0, {}, 55, 66, playerInputPayload, PLAYER_INPUT_MIN_ARGS_SIZE + 1);
    auto playerInputPacket = PacketManager::createPlayerInput(playerInputArgs);
    ASSERT_TRUE(playerInputPacket.has_value());
    EXPECT_EQ(playerInputPacket->data.size(), PLAYER_INPUT_PAYLOAD_SIZE);
}

TEST(PacketManagerCreateCoverage, CreateWorldAndComponentPackets) {
    std::vector<uint8_t> entityDestroyPayload(ENTITY_DESTROY_PAYLOAD_SIZE, 0);
    writeUint32(entityDestroyPayload, 0, 123);
    writeUint8(entityDestroyPayload, 4, protocol::ENTITY_DESTROY_REASON_MIN);
    writeUint16(entityDestroyPayload, 5, 77);
    writeUint16(entityDestroyPayload, 7, 88);
    auto entityDestroyArgs = buildArgs(0, {}, 1, 2, entityDestroyPayload, ENTITY_DESTROY_MIN_ARGS_SIZE + 1);
    auto entityDestroyPacket = PacketManager::createEntityDestroy(entityDestroyArgs);
    ASSERT_TRUE(entityDestroyPacket.has_value());
    EXPECT_EQ(entityDestroyPacket->data.size(), ENTITY_DESTROY_PAYLOAD_SIZE);

    std::vector<uint8_t> componentAddPayload(COMPONENT_ADD_BASE_SIZE + 2, 0);
    writeUint32(componentAddPayload, 0, 500);
    writeUint8(componentAddPayload, 4, 1);
    writeUint8(componentAddPayload, 5, 2);
    componentAddPayload[6] = 0xAA;
    componentAddPayload[7] = 0xBB;
    auto componentAddArgs = buildArgs(0, {}, 3, 4, componentAddPayload, COMPONENT_ADD_MIN_ARGS_SIZE + 1);
    auto componentAddPacket = PacketManager::createComponentAdd(componentAddArgs);
    ASSERT_TRUE(componentAddPacket.has_value());

    std::vector<uint8_t> componentRemovePayload(COMPONENT_REMOVE_PAYLOAD_SIZE, 0);
    writeUint32(componentRemovePayload, 0, 501);
    writeUint8(componentRemovePayload, 4, 2);
    auto componentRemoveArgs = buildArgs(0, {}, 5, 6, componentRemovePayload, COMPONENT_REMOVE_MIN_ARGS_SIZE + 1);
    auto componentRemovePacket = PacketManager::createComponentRemove(componentRemoveArgs);
    ASSERT_TRUE(componentRemovePacket.has_value());
    EXPECT_EQ(componentRemovePacket->data.size(), COMPONENT_REMOVE_PAYLOAD_SIZE);
}

TEST(PacketManagerCreateCoverage, CreateGameEventPackets) {
    std::vector<uint8_t> playerHitPayload(PLAYER_HIT_PAYLOAD_SIZE, 0);
    writeUint32(playerHitPayload, 0, 10);
    writeUint32(playerHitPayload, 4, 20);
    writeUint8(playerHitPayload, 8, 3);
    writeUint8(playerHitPayload, 9, 90);
    writeUint8(playerHitPayload, 10, 5);
    writeInt16(playerHitPayload, 11, -12);
    writeInt16(playerHitPayload, 13, 24);
    auto playerHitArgs = buildArgs(0, {}, 7, 8, playerHitPayload, PLAYER_HIT_MIN_ARGS_SIZE + 1);
    auto playerHitPacket = PacketManager::createPlayerHit(playerHitArgs);
    ASSERT_TRUE(playerHitPacket.has_value());

    std::vector<uint8_t> playerDeathPayload(PLAYER_DEATH_PAYLOAD_SIZE, 0);
    writeUint32(playerDeathPayload, 0, 10);
    writeUint32(playerDeathPayload, 4, 20);
    writeUint32(playerDeathPayload, 8, 3000);
    writeInt16(playerDeathPayload, 12, -1);
    writeInt16(playerDeathPayload, 14, 2);
    auto playerDeathArgs = buildArgs(0, {}, 9, 10, playerDeathPayload, PLAYER_DEATH_MIN_ARGS_SIZE + 1);
    auto playerDeathPacket = PacketManager::createPlayerDeath(playerDeathArgs);
    ASSERT_TRUE(playerDeathPacket.has_value());

    std::vector<uint8_t> scoreUpdatePayload(SCORE_UPDATE_PAYLOAD_SIZE, 0);
    writeUint32(scoreUpdatePayload, 0, 3);
    writeUint32(scoreUpdatePayload, 4, 1200);
    writeUint16(scoreUpdatePayload, 8, 5);
    writeUint8(scoreUpdatePayload, 10, 1);
    auto scoreUpdateArgs = buildArgs(0, {}, 11, 12, scoreUpdatePayload, SCORE_UPDATE_MIN_ARGS_SIZE + 1);
    auto scoreUpdatePacket = PacketManager::createScoreUpdate(scoreUpdateArgs);
    ASSERT_TRUE(scoreUpdatePacket.has_value());

    std::vector<uint8_t> powerupPayload(POWER_PICKUP_PAYLOAD_SIZE, 0);
    writeUint32(powerupPayload, 0, 12);
    writeUint32(powerupPayload, 4, 45);
    writeUint8(powerupPayload, 8, 1);
    writeUint8(powerupPayload, 9, 10);
    auto powerupArgs = buildArgs(0, {}, 13, 14, powerupPayload, POWER_PICKUP_MIN_ARGS_SIZE + 1);
    auto powerupPacket = PacketManager::createPowerupPickup(powerupArgs);
    ASSERT_TRUE(powerupPacket.has_value());

    std::vector<uint8_t> visualEffectPayload(VISUAL_EFFECT_PAYLOAD_SIZE, 0);
    writeUint8(visualEffectPayload, 0, 1);
    writeInt16(visualEffectPayload, 1, 10);
    writeInt16(visualEffectPayload, 3, 20);
    writeUint16(visualEffectPayload, 5, 500);
    writeUint8(visualEffectPayload, 7, 2);
    visualEffectPayload[8] = 100;
    visualEffectPayload[9] = 110;
    visualEffectPayload[10] = 120;
    auto visualEffectArgs = buildArgs(0, {}, 15, 16, visualEffectPayload, VISUAL_EFFECT_MIN_ARGS_SIZE + 1);
    auto visualEffectPacket = PacketManager::createVisualEffect(visualEffectArgs);
    ASSERT_TRUE(visualEffectPacket.has_value());

    std::vector<uint8_t> audioEffectPayload(AUDIO_EFFECT_PAYLOAD_SIZE, 0);
    writeUint8(audioEffectPayload, 0, 2);
    writeInt16(audioEffectPayload, 1, -5);
    writeInt16(audioEffectPayload, 3, 5);
    writeUint8(audioEffectPayload, 5, 120);
    writeUint8(audioEffectPayload, 6, 100);
    auto audioEffectArgs = buildArgs(0, {}, 17, 18, audioEffectPayload, AUDIO_EFFECT_MIN_ARGS_SIZE + 1);
    auto audioEffectPacket = PacketManager::createAudioEffect(audioEffectArgs);
    ASSERT_TRUE(audioEffectPacket.has_value());

    std::vector<uint8_t> particlePayload(PARTICLE_SPAWN_PAYLOAD_SIZE, 0);
    writeUint16(particlePayload, 0, 3);
    writeInt16(particlePayload, 2, 10);
    writeInt16(particlePayload, 4, 20);
    writeInt16(particlePayload, 6, 5);
    writeInt16(particlePayload, 8, -5);
    writeUint16(particlePayload, 10, 12);
    writeUint16(particlePayload, 12, 500);
    particlePayload[14] = 10;
    particlePayload[15] = 20;
    particlePayload[16] = 30;
    particlePayload[17] = 40;
    particlePayload[18] = 50;
    particlePayload[19] = 60;
    auto particleArgs = buildArgs(0, {}, 19, 20, particlePayload, PARTICLE_SPAWN_MIN_ARGS_SIZE + 1);
    auto particlePacket = PacketManager::createParticleSpawn(particleArgs);
    ASSERT_TRUE(particlePacket.has_value());
}

TEST(PacketManagerCreateCoverage, CreateGameControlPackets) {
    std::vector<uint8_t> gameStartPayload(GAME_START_PAYLOAD_SIZE, 0);
    writeUint32(gameStartPayload, 0, 101);
    writeUint8(gameStartPayload, 4, 2);
    writeUint32(gameStartPayload, 5, 1);
    writeUint32(gameStartPayload, 9, 2);
    writeUint32(gameStartPayload, 13, 3);
    writeUint32(gameStartPayload, 17, 4);
    writeUint8(gameStartPayload, 21, 7);
    writeUint8(gameStartPayload, 22, 1);
    auto gameStartArgs = buildArgs(0, {}, 21, 22, gameStartPayload, GAME_START_MIN_ARGS_SIZE + 1);
    auto gameStartPacket = PacketManager::createGameStart(gameStartArgs);
    ASSERT_TRUE(gameStartPacket.has_value());

    std::vector<uint8_t> gameEndPayload(GAME_END_PAYLOAD_SIZE, 0);
    writeUint8(gameEndPayload, 0, 1);
    writeUint32(gameEndPayload, 1, 100);
    writeUint32(gameEndPayload, 5, 200);
    writeUint32(gameEndPayload, 9, 300);
    writeUint32(gameEndPayload, 13, 400);
    writeUint8(gameEndPayload, 17, 2);
    writeUint32(gameEndPayload, 18, 900);
    auto gameEndArgs = buildArgs(0, {}, 23, 24, gameEndPayload, GAME_END_MIN_ARGS_SIZE + 1);
    auto gameEndPacket = PacketManager::createGameEnd(gameEndArgs);
    ASSERT_TRUE(gameEndPacket.has_value());

    std::vector<uint8_t> levelCompletePayload(LEVEL_COMPLETE_PAYLOAD_SIZE, 0);
    writeUint8(levelCompletePayload, 0, 1);
    writeUint8(levelCompletePayload, 1, 2);
    writeUint32(levelCompletePayload, 2, 5000);
    writeUint16(levelCompletePayload, 6, 60);
    auto levelCompleteArgs = buildArgs(0, {}, 25, 26, levelCompletePayload, LEVEL_COMPLETE_MIN_ARGS_SIZE + 1);
    auto levelCompletePacket = PacketManager::createLevelComplete(levelCompleteArgs);
    ASSERT_TRUE(levelCompletePacket.has_value());

    std::vector<uint8_t> levelStartPayload(LEVEL_START_PAYLOAD_SIZE, 0);
    writeUint8(levelStartPayload, 0, 1);
    const char *levelName = "Level A";
    std::memcpy(levelStartPayload.data() + 1, levelName, std::strlen(levelName));
    writeUint16(levelStartPayload, 33, 120);
    auto levelStartArgs = buildArgs(0, {}, 27, 28, levelStartPayload, LEVEL_START_MIN_ARGS_SIZE + 1);
    auto levelStartPacket = PacketManager::createLevelStart(levelStartArgs);
    ASSERT_TRUE(levelStartPacket.has_value());

    std::vector<uint8_t> forceStatePayload(FORCE_STATE_PAYLOAD_SIZE, 0);
    writeUint32(forceStatePayload, 0, 1000);
    writeUint32(forceStatePayload, 4, 2000);
    writeUint8(forceStatePayload, 8, 1);
    writeUint8(forceStatePayload, 9, 3);
    writeUint8(forceStatePayload, 10, 50);
    writeUint8(forceStatePayload, 11, 1);
    auto forceStateArgs = buildArgs(0, {}, 29, 30, forceStatePayload, FORCE_STATE_MIN_ARGS_SIZE + 1);
    auto forceStatePacket = PacketManager::createForceState(forceStateArgs);
    ASSERT_TRUE(forceStatePacket.has_value());

    std::vector<uint8_t> aiStatePayload(AI_STATE_PAYLOAD_SIZE, 0);
    writeUint32(aiStatePayload, 0, 77);
    writeUint8(aiStatePayload, 4, 2);
    writeUint8(aiStatePayload, 5, 1);
    writeUint32(aiStatePayload, 6, 88);
    writeInt16(aiStatePayload, 10, 10);
    writeInt16(aiStatePayload, 12, 11);
    writeUint16(aiStatePayload, 14, 500);
    auto aiStateArgs = buildArgs(0, {}, 31, 32, aiStatePayload, AI_STATE_MIN_ARGS_SIZE + 1);
    auto aiStatePacket = PacketManager::createAIState(aiStateArgs);
    ASSERT_TRUE(aiStatePacket.has_value());
}

TEST(PacketManagerCreateCoverage, CreateProtocolControlPackets) {
    std::vector<uint8_t> ackPayload(ACK_PAYLOAD_SIZE, 0);
    writeUint32(ackPayload, 0, 123);
    writeUint32(ackPayload, 4, 456);
    auto ackArgs = buildArgs(0, {}, 33, 34, ackPayload, ACK_MIN_ARGS_SIZE + 1);
    auto ackPacket = PacketManager::createAcknowledgment(ackArgs);
    ASSERT_TRUE(ackPacket.has_value());

    std::vector<uint8_t> pingPayload(PING_PAYLOAD_SIZE, 0);
    writeUint32(pingPayload, 0, 1111);
    auto pingArgs = buildArgs(0, {}, 35, 36, pingPayload, PING_MIN_ARGS_SIZE + 1);
    auto pingPacket = PacketManager::createPing(pingArgs);
    ASSERT_TRUE(pingPacket.has_value());

    std::vector<uint8_t> pongPayload(PONG_PAYLOAD_SIZE, 0);
    writeUint32(pongPayload, 0, 2222);
    writeUint32(pongPayload, 4, 3333);
    auto pongArgs = buildArgs(0, {}, 37, 38, pongPayload, PONG_MIN_ARGS_SIZE + 1);
    auto pongPacket = PacketManager::createPong(pongArgs);
    ASSERT_TRUE(pongPacket.has_value());
}

TEST(PacketManagerCreateCoverage, ParseWeaponFireCoverage) {
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE));
    packet.data.resize(WEAPON_FIRE_PAYLOAD_SIZE, 0);
    writeUint32(packet.data, 0, 10);
    writeUint32(packet.data, 4, 20);
    writeInt16(packet.data, 8, -10);
    writeInt16(packet.data, 10, 10);
    writeInt16(packet.data, 12, 1);
    writeInt16(packet.data, 14, 0);
    writeUint8(packet.data, 16, 2);

    auto parsed = PacketManager::parseWeaponFire(packet);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->shooterId, 10u);
    EXPECT_EQ(parsed->projectileId, 20u);
    EXPECT_EQ(parsed->weaponType, 2u);

    packet.data[16] = 0xFF;
    EXPECT_FALSE(PacketManager::assertWeaponFire(packet));
    EXPECT_FALSE(PacketManager::parseWeaponFire(packet).has_value());
}
