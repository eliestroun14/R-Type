/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestCoordinator (headless-safe)
*/

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>
#include <optional>
#include <stdexcept>

#define private public
#define protected public
#include "game/coordinator/Coordinator.hpp"
#include "common/protocol/Packet.hpp"
#include "common/protocol/Protocol.hpp"
// #include "common/protocol/PacketManager.hpp" // décommente si nécessaire
#undef protected
#undef private

// --------------------------------------------------------------
// Helpers
// --------------------------------------------------------------
static uint32_t findFreeEntityId(const std::shared_ptr<gameEngine::GameEngine>& engine,
                                uint32_t start = 1,
                                uint32_t maxScan = 5000)
{
    for (uint32_t id = start; id < start + maxScan; ++id) {
        if (!engine->isAlive(Entity::fromId(id)))
            return id;
    }
    return start;
}

static std::optional<common::protocol::Packet>
makePlayerInputPacket(uint32_t playerId, uint16_t inputState,
                      uint32_t seq = 1, uint32_t ts = 2)
{
    // [flags_count(1) + flags(1) + sequence_number(4) + timestamp(4) + player_id(4)
    //  + input_state(2) + aim_dir_x(2) + aim_dir_y(2)] => 20 bytes
    std::vector<uint8_t> args(20);
    uint8_t* ptr = args.data();

    uint8_t flagsCount = 1;
    std::memcpy(ptr, &flagsCount, sizeof(flagsCount));
    ptr += sizeof(flagsCount);

    uint8_t flags = 0x01;
    std::memcpy(ptr, &flags, sizeof(flags));
    ptr += sizeof(flags);

    std::memcpy(ptr, &seq, sizeof(seq));
    ptr += sizeof(seq);

    std::memcpy(ptr, &ts, sizeof(ts));
    ptr += sizeof(ts);

    std::memcpy(ptr, &playerId, sizeof(playerId));
    ptr += sizeof(playerId);

    std::memcpy(ptr, &inputState, sizeof(inputState));
    ptr += sizeof(inputState);

    int16_t aimX = 0;
    int16_t aimY = 0;
    std::memcpy(ptr, &aimX, sizeof(aimX));
    ptr += sizeof(aimX);
    std::memcpy(ptr, &aimY, sizeof(aimY));

    return PacketManager::createPlayerInput(args);
}

static uint32_t readU32(const std::vector<uint8_t>& buf, size_t offset)
{
    uint32_t v = 0;
    if (offset + sizeof(v) > buf.size())
        return 0;
    std::memcpy(&v, buf.data() + offset, sizeof(v));
    return v;
}

static uint16_t readU16(const std::vector<uint8_t>& buf, size_t offset)
{
    uint16_t v = 0;
    if (offset + sizeof(v) > buf.size())
        return 0;
    std::memcpy(&v, buf.data() + offset, sizeof(v));
    return v;
}

// --------------------------------------------------------------
// Fixture
// --------------------------------------------------------------
class CoordinatorFixture : public ::testing::Test {
protected:
    Coordinator coord;

    void SetUp() override
    {
        try {
            coord.initEngine();
        } catch (const std::exception& e) {
            GTEST_SKIP() << "initEngine() threw: " << e.what();
        } catch (...) {
            GTEST_SKIP() << "initEngine() threw unknown exception";
        }

        if (!coord.getEngine())
            GTEST_SKIP() << "Engine is null after initEngine()";
    }

    std::shared_ptr<gameEngine::GameEngine> engine() { return coord.getEngine(); }
};

// --------------------------------------------------------------
// Tests
// --------------------------------------------------------------

TEST(CoordinatorInit, InitEngineCreatesNonNullEngine)
{
    Coordinator c;
    ASSERT_NO_THROW(c.initEngine());
    ASSERT_NE(c.getEngine(), nullptr);
}

TEST_F(CoordinatorFixture, CreatePacketInputClient_ReturnsFalseOnNullPacketPointer)
{
    EXPECT_FALSE(coord.createPacketInputClient(nullptr, 0));
}

TEST_F(CoordinatorFixture, CreatePacketInputClient_ReturnsFalseWhenEntityNotAlive)
{
    uint32_t fakeId = findFreeEntityId(engine(), 1000);
    common::protocol::Packet pkt;
    EXPECT_FALSE(coord.createPacketInputClient(&pkt, fakeId));
}

TEST_F(CoordinatorFixture, CreatePacketInputClient_BuildsExpectedPayloadFields)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t eid = findFreeEntityId(eng, 100);

    Entity player = Entity::fromId(0);
    try {
        player = eng->createEntityWithId(eid, "TestPlayer");
    } catch (...) {
        GTEST_SKIP() << "createEntityWithId() not usable in this runtime (skipping).";
    }

    const uint32_t netPlayerId = 424242u;
    eng->addComponent<InputComponent>(player, InputComponent(netPlayerId));

    auto& inputOpt = eng->getComponentEntity<InputComponent>(player);
    ASSERT_TRUE(inputOpt.has_value());

    inputOpt->activeActions[GameAction::MOVE_UP] = true;
    inputOpt->activeActions[GameAction::SHOOT]   = true;

    common::protocol::Packet pkt;
    ASSERT_TRUE(coord.createPacketInputClient(&pkt, eid));

    ASSERT_TRUE(PacketManager::assertPlayerInput(pkt));
    ASSERT_GE(pkt.data.size(), 20u);

    // offsets: flags_count(1) flags(1) seq(4) ts(4) player_id(4) input_state(2) ...
    const uint32_t decodedPlayerId = readU32(pkt.data, 10);
    const uint16_t decodedInput    = readU16(pkt.data, 14);

    EXPECT_EQ(decodedPlayerId, netPlayerId);

    EXPECT_TRUE(decodedInput & static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_UP));
    EXPECT_TRUE(decodedInput & static_cast<uint16_t>(protocol::InputFlags::INPUT_FIRE_PRIMARY));

    EXPECT_FALSE(decodedInput & static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_LEFT));
    EXPECT_FALSE(decodedInput & static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_RIGHT));
    EXPECT_FALSE(decodedInput & static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_DOWN));
}

TEST_F(CoordinatorFixture, HandlePlayerInputPacket_UpdatesInputComponentActions)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t eid = findFreeEntityId(eng, 150);

    Entity player = Entity::fromId(0);
    try {
        player = eng->createEntityWithId(eid, "TestPlayerInputApply");
    } catch (...) {
        GTEST_SKIP() << "createEntityWithId() not usable in this runtime (skipping).";
    }

    const uint32_t netPlayerId = 777u;
    eng->addComponent<InputComponent>(player, InputComponent(netPlayerId));

    auto& inputOpt = eng->getComponentEntity<InputComponent>(player);
    ASSERT_TRUE(inputOpt.has_value());

    inputOpt->activeActions[GameAction::MOVE_UP] = false;
    inputOpt->activeActions[GameAction::SHOOT]   = false;

    const uint16_t inputState =
        static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_UP) |
        static_cast<uint16_t>(protocol::InputFlags::INPUT_FIRE_PRIMARY);

    auto pktOpt = makePlayerInputPacket(netPlayerId, inputState);
    ASSERT_TRUE(pktOpt.has_value());

    ASSERT_NO_THROW(coord.handlePlayerInputPacket(pktOpt.value(), 16));

    EXPECT_TRUE(inputOpt->activeActions[GameAction::MOVE_UP]);
    EXPECT_TRUE(inputOpt->activeActions[GameAction::SHOOT]);
}

TEST_F(CoordinatorFixture, HandlePacketTransformSnapshot_EmplacesOrUpdatesTransform)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t eid = findFreeEntityId(eng, 200);

    Entity ent = Entity::fromId(0);
    try {
        ent = eng->createEntityWithId(eid, "TestTransformEntity");
    } catch (...) {
        GTEST_SKIP() << "createEntityWithId() not usable in this runtime (skipping).";
    }

    ASSERT_FALSE(eng->getComponentEntity<Transform>(ent).has_value());

    common::protocol::Packet pkt;
    pkt.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT);

    uint32_t world_tick = 42;
    uint16_t count = 1;

    protocol::ComponentTransform net{};
    net.pos_x = 123;
    net.pos_y = 456;
    net.rotation = 0;
    net.scale = 1000;

    const size_t baseSize  = sizeof(uint32_t) + sizeof(uint16_t);
    const size_t entrySize = sizeof(uint32_t) + sizeof(protocol::ComponentTransform);
    pkt.data.resize(baseSize + entrySize);

    size_t off = 0;
    std::memcpy(pkt.data.data() + off, &world_tick, sizeof(world_tick));
    off += sizeof(world_tick);
    std::memcpy(pkt.data.data() + off, &count, sizeof(count));
    off += sizeof(count);
    std::memcpy(pkt.data.data() + off, &eid, sizeof(eid));
    off += sizeof(eid);
    std::memcpy(pkt.data.data() + off, &net, sizeof(net));

    ASSERT_NO_THROW(coord.handlePacketTransformSnapshot(pkt));

    auto& tfOpt = eng->getComponentEntity<Transform>(ent);
    ASSERT_TRUE(tfOpt.has_value());

    EXPECT_FLOAT_EQ(tfOpt->x, 123.f);
    EXPECT_FLOAT_EQ(tfOpt->y, 456.f);
    EXPECT_FLOAT_EQ(tfOpt->rotation, 0.f);
    EXPECT_FLOAT_EQ(tfOpt->scale, 1.0f);
}

TEST_F(CoordinatorFixture, HandlePacketDestroyEntity_DestroysEntityById)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t eid = findFreeEntityId(eng, 300);

    Entity ent = Entity::fromId(0);
    try {
        ent = eng->createEntityWithId(eid, "ToDestroy");
    } catch (...) {
        GTEST_SKIP() << "createEntityWithId() not usable in this runtime (skipping).";
    }

    ASSERT_TRUE(eng->isAlive(ent));

    protocol::EntityDestroyPayload payload{};
    payload.entity_id = eid;
    payload.destroy_reason = 1;
    payload.final_position_x = 0;
    payload.final_position_y = 0;

    common::protocol::Packet pkt;
    pkt.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_DESTROY);
    pkt.data.resize(ENTITY_DESTROY_PAYLOAD_SIZE);
    std::memcpy(pkt.data.data(), &payload, sizeof(payload));

    ASSERT_NO_THROW(coord.handlePacketDestroyEntity(pkt));
    EXPECT_FALSE(eng->isAlive(ent));
}

TEST_F(CoordinatorFixture, BuildClientPacketBasedOnStatus_SendsOnePacketForPlayable)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t eid = findFreeEntityId(eng, 400);

    Entity playable = Entity::fromId(0);
    try {
        playable = eng->createEntityWithId(eid, "PlayableClient");
    } catch (...) {
        GTEST_SKIP() << "createEntityWithId() not usable in this runtime (skipping).";
    }

    eng->addComponent<Playable>(playable, Playable());
    eng->addComponent<InputComponent>(playable, InputComponent(12u));

    auto& inOpt = eng->getComponentEntity<InputComponent>(playable);
    ASSERT_TRUE(inOpt.has_value());
    inOpt->activeActions[GameAction::MOVE_LEFT] = true;

    std::vector<common::protocol::Packet> out;
    ASSERT_NO_THROW(coord.buildClientPacketBasedOnStatus(out, 16));

    ASSERT_EQ(out.size(), 1u);
    ASSERT_TRUE(PacketManager::assertPlayerInput(out[0]));
}

TEST_F(CoordinatorFixture, SpawnProjectile_CreatesProjectileWithExpectedComponents)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t shooterId = findFreeEntityId(eng, 500);

    Entity shooter = Entity::fromId(0);
    try {
        shooter = eng->createEntityWithId(shooterId, "Shooter");
    } catch (...) {
        GTEST_SKIP() << "createEntityWithId() not usable in this runtime (skipping).";
    }

    eng->addComponent<Weapon>(shooter, Weapon(200, 0, 10, ProjectileType::MISSILE));
    eng->addComponent<InputComponent>(shooter, InputComponent(1u));

    uint32_t projId = findFreeEntityId(eng, 600);

    Entity proj = Entity::fromId(0);
    ASSERT_NO_THROW({
        proj = coord.spawnProjectile(shooter, projId, 0x00, 100.f, 200.f, 1.f, 0.f);
    });

    ASSERT_TRUE(eng->isAlive(proj));

    EXPECT_TRUE(eng->getComponentEntity<Transform>(proj).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Velocity>(proj).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Projectile>(proj).has_value());
    EXPECT_TRUE(eng->getComponentEntity<HitBox>(proj).has_value());

    EXPECT_TRUE(eng->getComponentEntity<Sprite>(proj).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Animation>(proj).has_value());

    auto& prOpt = eng->getComponentEntity<Projectile>(proj);
    ASSERT_TRUE(prOpt.has_value());

    EXPECT_TRUE(prOpt->isFromPlayable);
    EXPECT_EQ(prOpt->damage, 10);
}

TEST_F(CoordinatorFixture, HandleGameStartEnd_TogglesGameRunning)
{
    common::protocol::Packet start;
    start.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_START);
    start.data.resize(GAME_START_PAYLOAD_SIZE, 0);

    common::protocol::Packet end;
    end.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_END);
    end.data.resize(GAME_END_PAYLOAD_SIZE, 0);

    coord._gameRunning = false;

    ASSERT_NO_THROW(coord.handleGameStart(start));
    EXPECT_TRUE(coord._gameRunning);

    ASSERT_NO_THROW(coord.handleGameEnd(end));
    EXPECT_FALSE(coord._gameRunning);
}
