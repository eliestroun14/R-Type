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

static std::optional<common::protocol::Packet>
makeEntitySpawnPacket(uint32_t entityId,
                      uint16_t posX,
                      uint16_t posY,
                      uint8_t entityType,
                      uint8_t isPlayable)
{
    std::vector<uint8_t> args;
    args.reserve(32);

    auto appendU32 = [&args](uint32_t v) {
        const auto* raw = reinterpret_cast<uint8_t*>(&v);
        args.insert(args.end(), raw, raw + sizeof(v));
    };

    auto appendU16 = [&args](uint16_t v) {
        const auto* raw = reinterpret_cast<uint8_t*>(&v);
        args.insert(args.end(), raw, raw + sizeof(v));
    };

    auto appendI16 = [&args](int16_t v) {
        const auto* raw = reinterpret_cast<uint8_t*>(&v);
        args.insert(args.end(), raw, raw + sizeof(v));
    };

    args.push_back(1); // flags_count
    args.push_back(static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE));

    appendU32(1); // sequence_number
    appendU32(2); // timestamp
    appendU32(entityId);

    args.push_back(entityType);

    appendU16(posX);
    appendU16(posY);

    args.push_back(0);      // mob_variant
    args.push_back(100);    // initial_health
    appendI16(0);           // initial_velocity_x
    appendI16(0);           // initial_velocity_y
    args.push_back(isPlayable);

    return PacketManager::createEntitySpawn(args);
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

TEST_F(CoordinatorFixture, CreatePlayerEntity_AddsPlayableAndRenderComponents)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t playerId = findFreeEntityId(eng, 700);
    Entity entity = coord.createPlayerEntity(playerId, 10.f, 20.f, 1.f, 2.f, 150, true, true);

    ASSERT_TRUE(eng->isAlive(entity));

    auto& networkId = eng->getComponentEntity<NetworkId>(entity);
    auto& sprite    = eng->getComponentEntity<Sprite>(entity);
    auto& animation = eng->getComponentEntity<Animation>(entity);
    auto& playable  = eng->getComponentEntity<Playable>(entity);
    auto& input     = eng->getComponentEntity<InputComponent>(entity);
    auto& weapon    = eng->getComponentEntity<Weapon>(entity);

    ASSERT_TRUE(networkId.has_value());
    ASSERT_TRUE(sprite.has_value());
    ASSERT_TRUE(animation.has_value());
    ASSERT_TRUE(playable.has_value());
    ASSERT_TRUE(input.has_value());
    ASSERT_TRUE(weapon.has_value());

    EXPECT_EQ(networkId->id, playerId);
    EXPECT_EQ(input->playerId, playerId);
    EXPECT_EQ(weapon->damage, 10);
}
TEST_F(CoordinatorFixture, CreatePlayerEntity_SkipsRenderWhenRequested)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t playerId = findFreeEntityId(eng, 800);
    Entity entity = coord.createPlayerEntity(playerId, 5.f, 6.f, 0.f, 0.f, 80, false, false);

    ASSERT_TRUE(eng->isAlive(entity));

    auto& sprite    = eng->getComponentEntity<Sprite>(entity);
    auto& animation = eng->getComponentEntity<Animation>(entity);

    EXPECT_FALSE(sprite.has_value());
    EXPECT_FALSE(animation.has_value());

    EXPECT_TRUE(eng->getComponentEntity<NetworkId>(entity).has_value());
    EXPECT_TRUE(eng->getComponentEntity<InputComponent>(entity).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Health>(entity).has_value());
}

TEST_F(CoordinatorFixture, GetPlayablePlayerIds_ReturnsInputPlayerIds)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t eid = findFreeEntityId(eng, 900);
    Entity playable = eng->createEntityWithId(eid, "PlayableEntity");
    const uint32_t playerId = 4242;

    eng->addComponent<Playable>(playable, Playable());
    eng->addComponent<InputComponent>(playable, InputComponent(playerId));

    auto ids = coord.getPlayablePlayerIds();

    ASSERT_EQ(ids.size(), 1u);
    EXPECT_EQ(ids[0], playerId);
}

TEST_F(CoordinatorFixture, ProcessClientPackets_HandlesEntitySpawn)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t entityId = findFreeEntityId(eng, 1200);
    auto packetOpt = makeEntitySpawnPacket(
        entityId,
        /*posX=*/50,
        /*posY=*/60,
        static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER),
        /*isPlayable=*/1
    );

    ASSERT_TRUE(packetOpt.has_value());

    std::vector<common::protocol::Packet> incoming;
    incoming.push_back(packetOpt.value());

    coord.processClientPackets(incoming, 0);

    Entity created = Entity::fromId(entityId);
    EXPECT_TRUE(eng->isAlive(created));
    EXPECT_TRUE(eng->getComponentEntity<Transform>(created).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Health>(created).has_value());
    EXPECT_TRUE(eng->getComponentEntity<InputComponent>(created).has_value());
}

// ==============================================================
// Entity Creation Tests (Enemy, Projectile, Setup Methods)
// ==============================================================

TEST_F(CoordinatorFixture, CreateEnemyEntity_CreatesEntityWithComponents) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t enemyId = findFreeEntityId(eng, 2000);
    Entity enemy = coord.createEnemyEntity(
        enemyId,
        /*posX=*/100.0f,
        /*posY=*/200.0f,
        /*velX=*/-1.0f,
        /*velY=*/0.0f,
        /*initialHealth=*/50,
        /*withRenderComponents=*/true
    );

    EXPECT_TRUE(eng->isAlive(enemy));
    EXPECT_TRUE(eng->getComponentEntity<Transform>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Velocity>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Health>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Sprite>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<HitBox>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Weapon>(enemy).has_value());
}

TEST_F(CoordinatorFixture, CreateProjectileEntity_CreatesProjectile) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    uint32_t projectileId = findFreeEntityId(eng, 3000);
    Entity projectile = coord.createProjectileEntity(
        projectileId,
        /*posX=*/150.0f,
        /*posY=*/250.0f,
        /*velX=*/5.0f,
        /*velY=*/0.0f,
        /*isPlayerProjectile=*/true,
        /*damage=*/10,
        /*withRenderComponents=*/true
    );

    EXPECT_TRUE(eng->isAlive(projectile));
    EXPECT_TRUE(eng->getComponentEntity<Transform>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Velocity>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Projectile>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<HitBox>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Sprite>(projectile).has_value());
}

TEST_F(CoordinatorFixture, SetupEnemyEntity_AddsAllComponents) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity enemy = eng->createEntity("TestEnemy");
    coord.setupEnemyEntity(
        enemy,
        /*enemyId=*/123,
        /*posX=*/75.0f,
        /*posY=*/125.0f,
        /*velX=*/-0.5f,
        /*velY=*/0.0f,
        /*initialHealth=*/30,
        /*withRenderComponents=*/false
    );

    EXPECT_TRUE(eng->getComponentEntity<Transform>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Velocity>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Health>(enemy).has_value());
    EXPECT_FALSE(eng->getComponentEntity<Sprite>(enemy).has_value());  // No render
}

TEST_F(CoordinatorFixture, SetupProjectileEntity_AddsProjectileComponents) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity projectile = eng->createEntity("TestProjectile");
    Entity shooter = eng->createEntity("Shooter");

    coord.setupProjectileEntity(
        projectile,
        /*projectileId=*/456,
        /*posX=*/200.0f,
        /*posY=*/150.0f,
        /*velX=*/3.0f,
        /*velY=*/0.0f,
        /*isPlayerProjectile=*/false,
        /*damage=*/15,
        /*withRenderComponents=*/false
    );

    EXPECT_TRUE(eng->getComponentEntity<Transform>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Velocity>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Projectile>(projectile).has_value());
    EXPECT_FALSE(eng->getComponentEntity<Sprite>(projectile).has_value());
}

// ==============================================================
// Server Packet Processing Tests
// ==============================================================

TEST_F(CoordinatorFixture, QueueWeaponFire_AddsToQueue) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    EXPECT_EQ(coord._pendingWeaponFires.size(), 0);

    coord.queueWeaponFire(
        /*shooterId=*/123,
        /*originX=*/100.0f,
        /*originY=*/50.0f,
        /*directionX=*/1.0f,
        /*directionY=*/0.0f,
        /*weaponType=*/0x00
    );

    EXPECT_EQ(coord._pendingWeaponFires.size(), 1);
    EXPECT_EQ(coord._pendingWeaponFires[0].shooterId, 123);
}

TEST_F(CoordinatorFixture, SpawnPlayerOnServer_CreatesPacket) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    auto packetOpt = coord.spawnPlayerOnServer(
        /*playerId=*/999,
        /*posX=*/50.0f,
        /*posY=*/75.0f
    );

    EXPECT_TRUE(packetOpt.has_value());
    if (packetOpt.has_value()) {
        EXPECT_EQ(packetOpt->header.packet_type, static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));
    }
}

// ==============================================================
// Visual/Audio Effects Tests
// ==============================================================

TEST_F(CoordinatorFixture, HandlePacketVisualEffect_SpawnsEffect) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    // Create VISUAL_EFFECT packet
    std::vector<uint8_t> args;
    uint8_t flagsCount = 0;
    args.push_back(flagsCount);

    uint32_t seq = 1;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&seq), reinterpret_cast<uint8_t*>(&seq) + 4);

    uint32_t timestamp = 100;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), reinterpret_cast<uint8_t*>(&timestamp) + 4);

    uint8_t effectType = 0x00;  // EXPLOSION_SMALL
    args.push_back(effectType);

    uint16_t posX = 150;
    uint16_t posY = 200;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&posX), reinterpret_cast<uint8_t*>(&posX) + 2);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&posY), reinterpret_cast<uint8_t*>(&posY) + 2);

    uint8_t scale = 100;  // 1.0x scale
    args.push_back(scale);

    uint16_t durationMs = 500;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&durationMs), reinterpret_cast<uint8_t*>(&durationMs) + 2);

    uint8_t colorR = 255;
    uint8_t colorG = 128;
    uint8_t colorB = 0;
    args.push_back(colorR);
    args.push_back(colorG);
    args.push_back(colorB);

    auto packetOpt = PacketManager::createVisualEffect(args);
    ASSERT_TRUE(packetOpt.has_value());

    coord.handlePacketVisualEffect(packetOpt.value());

    // Visual effect should be spawned as an entity
    // We can't easily verify this without engine internals
}

/*DISABLED: HandlePacketAudioEffect_PlaysSound
TEST_F(CoordinatorFixture, HandlePacketAudioEffect_PlaysSound) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    // Create AUDIO_EFFECT packet
    std::vector<uint8_t> args;
    uint8_t flagsCount = 0;
    args.push_back(flagsCount);

    uint32_t seq = 1;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&seq), reinterpret_cast<uint8_t*>(&seq) + 4);

    uint32_t timestamp = 100;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), reinterpret_cast<uint8_t*>(&timestamp) + 4);

    uint8_t effectType = 0x00;  // SFX_SHOOT_BASIC
    args.push_back(effectType);

    uint16_t posX = 100;
    uint16_t posY = 100;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&posX), reinterpret_cast<uint8_t*>(&posX) + 2);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&posY), reinterpret_cast<uint8_t*>(&posY) + 2);

    uint8_t volume = 128;  // 50% volume
    uint8_t pitch = 100;   // Normal pitch
    args.push_back(volume);
    args.push_back(pitch);

    auto packetOpt = PacketManager::createAudioEffect(args);
    ASSERT_TRUE(packetOpt.has_value());

    coord.handlePacketAudioEffect(packetOpt.value());
}*/

TEST_F(CoordinatorFixture, SpawnVisualEffect_CreatesEntity) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    size_t entityCountBefore = 0;
    for (size_t i = 0; i < 1000; ++i) {
        if (eng->isAlive(Entity::fromId(i))) entityCountBefore++;
    }

    coord.spawnVisualEffect(
        protocol::VisualEffectType::VFX_EXPLOSION_SMALL,
        50.0f,  // x
        75.0f,  // y
        1.5f,   // scale
        0.5f,   // duration
        1.0f,   // color_r
        0.5f,   // color_g
        0.0f    // color_b
    );

    size_t entityCountAfter = 0;
    for (size_t i = 0; i < 1000; ++i) {
        if (eng->isAlive(Entity::fromId(i))) entityCountAfter++;
    }

    EXPECT_GT(entityCountAfter, entityCountBefore);
}


// ==============================================================
// Level and State Management Tests
// ==============================================================

TEST_F(CoordinatorFixture, HandlePacketLevelStart_SetsGameRunning) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    coord._gameRunning = false;

    // Create LEVEL_START packet
    std::vector<uint8_t> args;
    uint8_t flagsCount = 0;
    args.push_back(flagsCount);

    uint32_t seq = 1;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&seq), reinterpret_cast<uint8_t*>(&seq) + 4);

    uint32_t timestamp = 100;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), reinterpret_cast<uint8_t*>(&timestamp) + 4);

    uint8_t levelId = 1;
    args.push_back(levelId);

    char levelName[32] = "Test Level";
    args.insert(args.end(), levelName, levelName + 32);

    uint16_t estimatedDuration = 300;  // 5 minutes
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&estimatedDuration), reinterpret_cast<uint8_t*>(&estimatedDuration) + 2);

    auto packetOpt = PacketManager::createLevelStart(args);
    ASSERT_TRUE(packetOpt.has_value());

    coord.handlePacketLevelStart(packetOpt.value());

    EXPECT_TRUE(coord._gameRunning);
}

TEST_F(CoordinatorFixture, HandlePacketLevelComplete_ProcessesCompletion) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    coord._gameRunning = true;

    // Create LEVEL_COMPLETE packet
    std::vector<uint8_t> args;
    uint8_t flagsCount = 0;
    args.push_back(flagsCount);

    uint32_t seq = 1;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&seq), reinterpret_cast<uint8_t*>(&seq) + 4);

    uint32_t timestamp = 100;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), reinterpret_cast<uint8_t*>(&timestamp) + 4);

    uint8_t completedLevel = 1;
    args.push_back(completedLevel);

    uint8_t nextLevel = 2;
    args.push_back(nextLevel);

    uint32_t bonusScore = 1000;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&bonusScore), reinterpret_cast<uint8_t*>(&bonusScore) + 4);

    uint16_t completionTime = 180;  // 3 minutes
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&completionTime), reinterpret_cast<uint8_t*>(&completionTime) + 2);

    auto packetOpt = PacketManager::createLevelComplete(args);
    ASSERT_TRUE(packetOpt.has_value());

    coord.handlePacketLevelComplete(packetOpt.value());

    // Should still be running for next level
    EXPECT_TRUE(coord._gameRunning);
}
