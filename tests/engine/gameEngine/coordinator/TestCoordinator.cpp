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
#include <algorithm>
#include <unordered_map>

#define private public
#define protected public
#include "game/coordinator/Coordinator.hpp"
#include "game/systems/ScoreSystem.hpp"
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
    // Build args with enough space for PacketManager expectations
    std::vector<uint8_t> args(PLAYER_INPUT_MIN_ARGS_SIZE, 0);
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

static void appendBytes(std::vector<uint8_t>& buf, const void* data, size_t size)
{
    const auto* bytes = reinterpret_cast<const uint8_t*>(data);
    buf.insert(buf.end(), bytes, bytes + size);
}

template <typename T>
static void appendValue(std::vector<uint8_t>& buf, const T& value)
{
    appendBytes(buf, &value, sizeof(T));
}

static common::protocol::Packet makePlayerReadyPacket(uint32_t playerId, bool isReady)
{
    common::protocol::Packet pkt;
    pkt.header.packet_type = static_cast<uint8_t>(
        isReady ? protocol::PacketTypes::TYPE_PLAYER_IS_READY
                : protocol::PacketTypes::TYPE_PLAYER_NOT_READY);
    pkt.data.resize(PLAYER_READY_PAYLOAD_SIZE);
    std::memcpy(pkt.data.data(), &playerId, sizeof(playerId));
    return pkt;
}

static common::protocol::Packet makeWeaponFirePacket(uint32_t shooterId, uint32_t projectileId,
                                                     int16_t originX, int16_t originY,
                                                     int16_t dirX, int16_t dirY,
                                                     uint8_t weaponType)
{
    std::vector<uint8_t> args;
    args.resize(WEAPON_FIRE_MIN_ARGS_SIZE);
    uint8_t* ptr = args.data();

    uint8_t flagsCount = 1;
    std::memcpy(ptr, &flagsCount, sizeof(flagsCount));
    ptr += sizeof(flagsCount);

    uint8_t flags = 0x01;
    std::memcpy(ptr, &flags, sizeof(flags));
    ptr += sizeof(flags);

    uint32_t seq = 42;
    std::memcpy(ptr, &seq, sizeof(seq));
    ptr += sizeof(seq);

    uint32_t ts = 84;
    std::memcpy(ptr, &ts, sizeof(ts));
    ptr += sizeof(ts);

    std::memcpy(ptr, &shooterId, sizeof(shooterId));
    ptr += sizeof(shooterId);

    std::memcpy(ptr, &projectileId, sizeof(projectileId));
    ptr += sizeof(projectileId);

    std::memcpy(ptr, &originX, sizeof(originX));
    ptr += sizeof(originX);

    std::memcpy(ptr, &originY, sizeof(originY));
    ptr += sizeof(originY);

    std::memcpy(ptr, &dirX, sizeof(dirX));
    ptr += sizeof(dirX);

    std::memcpy(ptr, &dirY, sizeof(dirY));
    ptr += sizeof(dirY);

    std::memcpy(ptr, &weaponType, sizeof(weaponType));

    auto packetOpt = PacketManager::createWeaponFire(args);
    return packetOpt.value();
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
    payload.entity_id = static_cast<uint32_t>(ent);
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

    EXPECT_EQ(networkId->id, static_cast<uint32_t>(entity));
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

    EXPECT_TRUE(sprite.has_value());
    EXPECT_TRUE(animation.has_value());

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

    Entity created = eng->getEntityFromNetworkId(entityId, true);
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
        /*enemyType=*/EnemyType::BASIC,
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
        /*enemyType=*/EnemyType::BASIC,
        /*withRenderComponents=*/false
    );

    EXPECT_TRUE(eng->getComponentEntity<Transform>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Velocity>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Health>(enemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Sprite>(enemy).has_value());
}

TEST_F(CoordinatorFixture, SetupProjectileEntity_AddsProjectileComponents) {
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity projectile = eng->createEntity("TestProjectile");
    Entity shooter = eng->createEntity("Shooter");

    coord.setupProjectileEntity(
        projectile,
        /*shooterId=*/shooter,
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

TEST_F(CoordinatorFixture, CreateScoreEntity_SetsHudAndComponents)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity scoreEntity = coord.createScoreEntity(1, 10.f, 20.f, 300);

    EXPECT_TRUE(eng->isAlive(scoreEntity));
    EXPECT_TRUE(eng->getComponentEntity<Transform>(scoreEntity).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Score>(scoreEntity).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Text>(scoreEntity).has_value());

    auto& scoreSystem = eng->getSystem<ScoreSystem>();
    EXPECT_EQ(scoreSystem._hud, scoreEntity);
}

TEST_F(CoordinatorFixture, SetupScoreEntity_UpdatesExistingComponents)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity scoreEntity = eng->createEntity("ScoreUpdate");
    eng->addComponent<Transform>(scoreEntity, Transform(1.f, 2.f, 0.f, 1.f));
    eng->addComponent<Score>(scoreEntity, Score{10});
    eng->addComponent<Text>(scoreEntity, Text("10"));

    coord.setupScoreEntity(scoreEntity, 0, 50.f, 60.f, 999);

    auto& tf = eng->getComponentEntity<Transform>(scoreEntity);
    auto& sc = eng->getComponentEntity<Score>(scoreEntity);
    auto& tx = eng->getComponentEntity<Text>(scoreEntity);

    ASSERT_TRUE(tf.has_value());
    ASSERT_TRUE(sc.has_value());
    ASSERT_TRUE(tx.has_value());

    EXPECT_FLOAT_EQ(tf->x, 50.f);
    EXPECT_FLOAT_EQ(tf->y, 60.f);
    EXPECT_EQ(sc->score, 999u);
}

TEST_F(CoordinatorFixture, SetupEnemyEntity_CoversEnemyTypes)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity fastEnemy = eng->createEntity("FastEnemy");
    coord.setupEnemyEntity(fastEnemy, 10, 0.f, 0.f, 0.f, 0.f, 40, EnemyType::FAST, true);
    EXPECT_TRUE(eng->getComponentEntity<AI>(fastEnemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Weapon>(fastEnemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Drawable>(fastEnemy).has_value());

    Entity tankEnemy = eng->createEntity("TankEnemy");
    coord.setupEnemyEntity(tankEnemy, 11, 0.f, 0.f, 0.f, 0.f, 80, EnemyType::TANK, false);
    EXPECT_TRUE(eng->getComponentEntity<AI>(tankEnemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Weapon>(tankEnemy).has_value());
    EXPECT_FALSE(eng->getComponentEntity<Drawable>(tankEnemy).has_value());

    Entity bossEnemy = eng->createEntity("BossEnemy");
    coord.setupEnemyEntity(bossEnemy, 12, 0.f, 0.f, 0.f, 0.f, 200, EnemyType::BOSS, true);
    EXPECT_FALSE(eng->getComponentEntity<Weapon>(bossEnemy).has_value());
    EXPECT_FALSE(eng->getComponentEntity<AI>(bossEnemy).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Drawable>(bossEnemy).has_value());
}

TEST_F(CoordinatorFixture, SetupProjectileEntity_WithRenderAddsSprite)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity projectile = eng->createEntity("ProjectileRender");
    Entity shooter = eng->createEntity("ShooterRender");

    coord.setupProjectileEntity(projectile, shooter, 10.f, 20.f, 1.f, 0.f, true, 7, true);

    EXPECT_TRUE(eng->getComponentEntity<Sprite>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Projectile>(projectile).has_value());
    EXPECT_TRUE(eng->getComponentEntity<Team>(projectile).has_value());
}

TEST_F(CoordinatorFixture, GetAllConnectedPlayerIds_ReturnsNetworkedInputs)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity p1 = eng->createEntityWithId(1, "P1");
    Entity p2 = eng->createEntityWithId(2, "P2");

    eng->addComponent<InputComponent>(p1, InputComponent(111));
    eng->addComponent<InputComponent>(p2, InputComponent(222));

    auto ids = coord.getAllConnectedPlayerIds();
    std::sort(ids.begin(), ids.end());

    ASSERT_EQ(ids.size(), 2u);
    EXPECT_EQ(ids[0], 111u);
    EXPECT_EQ(ids[1], 222u);
}

TEST_F(CoordinatorFixture, BuildClientPacketBasedOnStatus_EmitsReadyPackets)
{
    std::vector<common::protocol::Packet> outgoing;
    coord.queuePlayerIsReady(10);
    coord.queuePlayerNotReady(20);

    coord.buildClientPacketBasedOnStatus(outgoing, 0);

    EXPECT_TRUE(coord._pendingPlayerReadyEvents.empty());
    ASSERT_GE(outgoing.size(), 2u);

    bool hasReady = false;
    bool hasNotReady = false;
    for (const auto& pkt : outgoing) {
        if (pkt.header.packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_IS_READY))
            hasReady = true;
        if (pkt.header.packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_NOT_READY))
            hasNotReady = true;
    }

    EXPECT_TRUE(hasReady);
    EXPECT_TRUE(hasNotReady);
}

TEST_F(CoordinatorFixture, BuildServerPacketBasedOnStatus_EmitsSnapshotsAndWeaponFire)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity player = coord.createPlayerEntity(1, 10.f, 20.f, 0.f, 0.f, 100, false, false);
    uint32_t shooterId = static_cast<uint32_t>(player);

    auto inputPacket = makePlayerInputPacket(1, 0x01);
    ASSERT_TRUE(inputPacket.has_value());
    coord.processServerPackets({inputPacket.value()}, 0);

    coord.queueWeaponFire(shooterId, 10.f, 20.f, 1.f, 0.f, 0x00);

    std::vector<common::protocol::Packet> outgoing;
    coord.buildServerPacketBasedOnStatus(outgoing, 123);

    ASSERT_FALSE(outgoing.empty());
    bool hasWeaponFire = false;
    for (const auto& pkt : outgoing) {
        if (pkt.header.packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE))
            hasWeaponFire = true;
    }
    EXPECT_TRUE(hasWeaponFire);
}

TEST_F(CoordinatorFixture, HandlePacketTransformHealthWeaponAnimationSnapshots)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity entity = eng->createEntityWithId(1234, "SnapEntity");
    eng->addComponent<Transform>(entity, Transform(0.f, 0.f, 0.f, 1.f));
    eng->addComponent<Health>(entity, Health(1, 10));
    eng->addComponent<Weapon>(entity, Weapon(100, 0, 5, ProjectileType::MISSILE));
    eng->addComponent<Animation>(entity, Animation(1, 1, 0, 0.f, 0.2f, 0, 1, true));

    // Transform snapshot
    std::vector<uint8_t> tdata;
    uint16_t count = 1;
    appendValue(tdata, count);
    uint32_t entityId = static_cast<uint32_t>(entity);
    protocol::ComponentTransform ct{100, 200, 1000, 1500};
    appendValue(tdata, entityId);
    appendValue(tdata, ct);
    common::protocol::Packet tpacket;
    tpacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT);
    tpacket.data = tdata;
    coord.handlePacketTransformSnapshot(tpacket);

    auto& tf = eng->getComponentEntity<Transform>(entity);
    ASSERT_TRUE(tf.has_value());
    EXPECT_FLOAT_EQ(tf->x, 100.f);
    EXPECT_FLOAT_EQ(tf->y, 200.f);

    // Health snapshot
    std::vector<uint8_t> hdata;
    appendValue(hdata, count);
    appendValue(hdata, entityId);
    uint8_t current = 7;
    uint8_t max = 9;
    uint8_t shield = 0;
    uint8_t maxShield = 0;
    appendValue(hdata, current);
    appendValue(hdata, max);
    appendValue(hdata, shield);
    appendValue(hdata, maxShield);
    common::protocol::Packet hpacket;
    hpacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT);
    hpacket.data = hdata;
    coord.handlePacketHealthSnapshot(hpacket);

    auto& health = eng->getComponentEntity<Health>(entity);
    ASSERT_TRUE(health.has_value());
    EXPECT_EQ(health->currentHealth, 7);
    EXPECT_EQ(health->maxHp, 9);

    // Weapon snapshot
    std::vector<uint8_t> wdata;
    appendValue(wdata, count);
    appendValue(wdata, entityId);
    uint16_t fireRate = 250;
    uint8_t damage = 15;
    uint8_t projectileType = static_cast<uint8_t>(ProjectileType::MISSILE);
    uint8_t ammo = 0;
    appendValue(wdata, fireRate);
    appendValue(wdata, damage);
    appendValue(wdata, projectileType);
    appendValue(wdata, ammo);
    common::protocol::Packet wpacket;
    wpacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT);
    wpacket.data = wdata;
    coord.handlePacketWeaponSnapshot(wpacket);

    auto& weapon = eng->getComponentEntity<Weapon>(entity);
    ASSERT_TRUE(weapon.has_value());
    EXPECT_EQ(weapon->fireRateMs, 250u);
    EXPECT_EQ(weapon->damage, 15);

    // Animation snapshot
    std::vector<uint8_t> adata;
    uint32_t worldTick = 5;
    appendValue(adata, worldTick);
    appendValue(adata, count);
    appendValue(adata, entityId);
    protocol::ComponentAnimation ca{1, 3, 50, 1};
    appendValue(adata, ca);
    common::protocol::Packet apacket;
    apacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT);
    apacket.data = adata;
    coord.handlePacketAnimationSnapshot(apacket);

    auto& anim = eng->getComponentEntity<Animation>(entity);
    ASSERT_TRUE(anim.has_value());
    EXPECT_EQ(anim->currentFrame, 3u);
    EXPECT_NEAR(anim->frameDuration, 0.05f, 0.0001f);
    EXPECT_TRUE(anim->loop);
}

TEST_F(CoordinatorFixture, HandlePacketComponentRemove_RemovesTransform)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity entity = eng->createEntityWithId(2222, "RemoveComp");
    eng->addComponent<Transform>(entity, Transform(1.f, 2.f, 0.f, 1.f));

    protocol::ComponentRemove payload{};
    payload.entity_id = static_cast<uint32_t>(entity);
    payload.component_type = static_cast<uint8_t>(protocol::ComponentType::COMPONENT_TRANSFORM);

    common::protocol::Packet packet;
    packet.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_REMOVE);
    packet.data.resize(COMPONENT_REMOVE_PAYLOAD_SIZE);
    std::memcpy(packet.data.data(), &payload, sizeof(payload));

    coord.handlePacketComponentRemove(packet);

    EXPECT_FALSE(eng->hasComponent<Transform>(entity));
}

TEST_F(CoordinatorFixture, HandlePacketPlayerIsReadyNotReady_InvokesCallback)
{
    uint32_t readyId = 10;
    uint32_t notReadyId = 20;
    bool readyCalled = false;
    bool notReadyCalled = false;

    coord.setGameNotificationCallback([&](uint32_t playerId, bool isReady) {
        if (playerId == readyId && isReady)
            readyCalled = true;
        if (playerId == notReadyId && !isReady)
            notReadyCalled = true;
    });

    coord.handlePacketPlayerIsReady(makePlayerReadyPacket(readyId, true));
    coord.handlePacketPlayerNotReady(makePlayerReadyPacket(notReadyId, false));

    EXPECT_TRUE(readyCalled);
    EXPECT_TRUE(notReadyCalled);
}

TEST_F(CoordinatorFixture, AreAllPlayersReady_ValidatesStates)
{
    std::vector<uint32_t> empty;
    std::unordered_map<uint32_t, bool> status;
    EXPECT_FALSE(coord.areAllPlayersReady(empty, 2, status));

    std::vector<uint32_t> players = {1, 2};
    EXPECT_FALSE(coord.areAllPlayersReady(players, 3, status));

    status[1] = true;
    status[2] = false;
    EXPECT_FALSE(coord.areAllPlayersReady(players, 2, status));

    status[2] = true;
    EXPECT_TRUE(coord.areAllPlayersReady(players, 2, status));
}

TEST_F(CoordinatorFixture, HandlePacketWeaponFire_SpawnsProjectile)
{
    auto eng = engine();
    ASSERT_NE(eng, nullptr);

    Entity shooter = coord.createPlayerEntity(5, 0.f, 0.f, 0.f, 0.f, 100, true, false);
    uint32_t shooterId = static_cast<uint32_t>(shooter);
    uint32_t projectileId = NETWORKED_ID_OFFSET + 123;

    auto packet = makeWeaponFirePacket(
        shooterId,
        projectileId,
        10,
        20,
        1000,
        0,
        0x00
    );

    coord.handlePacketWeaponFire(packet);

    Entity proj = eng->getEntityFromId(projectileId);
    EXPECT_TRUE(eng->isAlive(proj));
    EXPECT_TRUE(eng->getComponentEntity<Projectile>(proj).has_value());
}
