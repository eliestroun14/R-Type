/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Coordinator
*/

#include "game/coordinator/Coordinator.hpp"
#include "game/systems/AnimationSystem.hpp"
#include "game/systems/AudioSystem.hpp"
#include "game/systems/BackgroundSystem.hpp"
#include "game/systems/CollisionSystem.hpp"
#include "game/systems/ScoreSystem.hpp"
#include "game/systems/LevelSystem.hpp"
#include "game/systems/LevelTimerSystem.hpp"

void Coordinator::initEngine()
{
    this->_engine = std::make_shared<gameEngine::GameEngine>();
    this->_engine->init();

    // Register all component types used in the game
    this->_engine->registerComponent<Transform>();
    this->_engine->registerComponent<Velocity>();
    this->_engine->registerComponent<NetworkId>();
    this->_engine->registerComponent<HitBox>();
    this->_engine->registerComponent<Sprite>();
    this->_engine->registerComponent<Animation>();
    this->_engine->registerComponent<Text>();
    this->_engine->registerComponent<ScrollingBackground>();
    this->_engine->registerComponent<VisualEffect>();
    this->_engine->registerComponent<Lifetime>();
    this->_engine->registerComponent<Health>();
    this->_engine->registerComponent<Powerup>();
    this->_engine->registerComponent<Weapon>();
    this->_engine->registerComponent<Clickable>();
    this->_engine->registerComponent<Drawable>();
    this->_engine->registerComponent<Playable>();
    this->_engine->registerComponent<InputComponent>();
    this->_engine->registerComponent<Enemy>();
    this->_engine->registerComponent<Projectile>();
    this->_engine->registerComponent<MovementPattern>();
    this->_engine->registerComponent<AI>();
    this->_engine->registerComponent<ButtonComponent>();
    this->_engine->registerComponent<GameConfig>();
    this->_engine->registerComponent<Rebind>();
    this->_engine->registerComponent<AudioSource>();
    this->_engine->registerComponent<AudioEffect>();
    this->_engine->registerComponent<Team>();
    this->_engine->registerComponent<Score>();
    this->_engine->registerComponent<Level>();
    this->_engine->registerComponent<TimerUI>();

    // Register gameplay systems (both client and server)
    auto playerSystem = this->_engine->registerSystem<PlayerSystem>(*this->_engine);
    this->_engine->setSystemSignature<PlayerSystem, Velocity, InputComponent>();

    auto movementSystem = this->_engine->registerSystem<MovementSystem>(*this->_engine);
    this->_engine->setSystemSignature<MovementSystem, Transform, Velocity>();

    auto shootSystem = this->_engine->registerSystem<ShootSystem>(*this->_engine, *this, this->_isServer);
    this->_engine->setSystemSignature<ShootSystem, Weapon, Transform>();

    // Register LevelSystem (server-side only, but registered for both)
    auto levelSystem = this->_engine->registerSystem<LevelSystem>(*this->_engine, this);
    this->_engine->setSystemSignature<LevelSystem, Level>();
    auto buttonSystem = this->_engine->registerSystem<ButtonSystem>(*this->_engine, this->_isServer);
    this->_engine->setSystemSignature<ButtonSystem, ButtonComponent, Transform>();

    auto accessibilitySystem = this->_engine->registerSystem<AccessibilitySystem>(*this->_engine);
    this->_engine->setSystemSignature<AccessibilitySystem, GameConfig>();

    auto rebindSystem = this->_engine->registerSystem<RebindSystem>(*this->_engine);
    this->_engine->setSystemSignature<RebindSystem, Rebind>();

    // Score system
    auto scoreSystem = this->_engine->registerSystem<ScoreSystem>(*this->_engine);
    this->_engine->setSystemSignature<ScoreSystem, Score, Text>();

    // Register CollisionSystem to handle collision detection and team-based damage
    // Must run on both client AND server for authoritative damage
    auto collisionSystem = this->_engine->registerSystem<CollisionSystem>(*this->_engine);
    this->_engine->setSystemSignature<CollisionSystem, Transform, Sprite, HitBox>();
}

void Coordinator::initEngineRender()  // Nouvelle méthode
{
    this->_engine->initRender();
    this->_engine->initAudio();

    // Register BackgroundSystem to handle scrolling backgrounds
    auto backgroundSystem = this->_engine->registerSystem<BackgroundSystem>(*this->_engine,  this->_isServer);
    this->_engine->setSystemSignature<BackgroundSystem, Transform, Sprite, ScrollingBackground>();

    // Register AnimationSystem before RenderSystem (animation must update before rendering)
    auto animationSystem = this->_engine->registerSystem<AnimationSystem>(*this->_engine);
    this->_engine->setSystemSignature<AnimationSystem, Animation, Sprite>();

    // Register AudioSystem to handle entity audio
    auto audioSystem = this->_engine->registerSystem<AudioSystem>(*this->_engine);
    this->_engine->setSystemSignature<AudioSystem, AudioSource>();

    // Register RenderSystem to handle entity rendering
    auto renderSystem = this->_engine->registerSystem<RenderSystem>(*this->_engine);

    // Set signature: RenderSystem needs Transform and Sprite components
    this->_engine->setSystemSignature<RenderSystem, Transform, Sprite>();

    // Register LevelTimerSystem to update countdown timers
    auto levelTimerSystem = this->_engine->registerSystem<LevelTimerSystem>(*this->_engine);
    this->_engine->setSystemSignature<LevelTimerSystem, TimerUI, Text>();
}


// ==============================================================
//                       Entity creation helpers
// ==============================================================

Entity Coordinator::createPlayerEntity(
    uint32_t playerId,
    float posX,
    float posY,
    float velX,
    float velY,
    uint16_t initialHealth,
    bool isPlayable,
    bool withRenderComponents
)
{
    // Use createEntityWithId to ensure the entity ID matches the player ID
    Entity entity = this->_engine->createEntityWithId(playerId, "Player_" + std::to_string(playerId));
    this->setupPlayerEntity(
        entity,
        playerId,
        posX,
        posY,
        velX,
        velY,
        initialHealth,
        isPlayable,
        withRenderComponents
    );
    return entity;
}

Entity Coordinator::createScoreEntity(
    uint32_t scoreId,
    float posX,
    float posY,
    uint32_t initialScore
)
{
    Entity entity = this->_engine->createEntity(
        "Score_" + std::to_string(scoreId),
        EntityCategory::LOCAL
    );

    LOG_INFO_CAT("UI", "Score entity created: scoreId={} entityIndex={}",
        scoreId, static_cast<std::size_t>(entity));

    this->setupScoreEntity(entity, scoreId, posX, posY, initialScore);

    // Bind this entity as the HUD score target for ScoreSystem
    this->_engine->getSystem<ScoreSystem>().setHudEntity(entity);
    LOG_INFO_CAT("UI", "ScoreSystem HUD entity set: entityIndex={}",
        static_cast<std::size_t>(entity));

    return entity;
}


Entity Coordinator::createEnemyEntity(
    uint32_t enemyId,
    float posX,
    float posY,
    float velX,
    float velY,
    uint16_t initialHealth,
    EnemyType enemyType,
    bool withRenderComponents
)
{
    // Use createEntityWithId to ensure the entity ID matches the enemy ID
    Entity entity = this->_engine->createEntityWithId(enemyId, "Enemy_" + std::to_string(enemyId));
    this->setupEnemyEntity(
        entity,
        enemyId,
        posX,
        posY,
        velX,
        velY,
        initialHealth,
        enemyType,
        withRenderComponents
    );
    return entity;
}

Entity Coordinator::createProjectileEntity(
    uint32_t projectileId,
    float posX,
    float posY,
    float velX,
    float velY,
    bool isPlayerProjectile,
    uint16_t damage,
    bool withRenderComponents
)
{
    // Use createEntityWithId to ensure the entity ID matches the projectile ID
    Entity entity = this->_engine->createEntityWithId(projectileId, "Projectile_" + std::to_string(projectileId));
    // Note: shooterId is unknown here, using invalid entity. This function is legacy.
    this->setupProjectileEntity(
        entity,
        Entity::fromId(0),  // Unknown shooter - this function should be deprecated
        posX,
        posY,
        velX,
        velY,
        isPlayerProjectile,
        damage,
        withRenderComponents
    );
    return entity;
}

// ==============================================================
//                               Setup
// ==============================================================

void Coordinator::setupPlayerEntity(
    Entity entity,
    uint32_t playerId,
    float posX,
    float posY,
    float velX,
    float velY,
    uint16_t initialHealth,
    bool isPlayable,
    bool withRenderComponents
)
{
    // Common gameplay components (server + client)
    // CRITICAL: Add NetworkId component so entity is included in server snapshots
    // Use the entity's actual internal ID (which includes NETWORKED_ID_OFFSET)
    this->_engine->addComponent<NetworkId>(entity, NetworkId(static_cast<uint32_t>(entity)));

    // Always add Sprite and Animation (needed for CollisionSystem even on server)
    Assets spriteAsset = _playerSpriteAllocator.allocate(playerId);
    this->_engine->addComponent<Sprite>(entity, Sprite(spriteAsset, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 15)));
    this->_engine->addComponent<Animation>(entity, Animation(33, 15, 2, 0.f, 0.1f, 2, 2, true));

    this->_engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, 1.5f));
    this->_engine->addComponent<Velocity>(entity, Velocity(velX, velY));
    this->_engine->addComponent<Health>(entity, Health(initialHealth, initialHealth));
    this->_engine->addComponent<HitBox>(entity, HitBox());
    this->_engine->addComponent<Weapon>(entity, Weapon(200, 0, 10, ProjectileType::MISSILE));
    this->_engine->addComponent<InputComponent>(entity, InputComponent(playerId));
    this->_engine->addComponent<Team>(entity, TeamType::PLAYER);

    // If this is the playable player, set it as local player
    if (isPlayable) {
        this->_engine->addComponent<Playable>(entity, Playable());

        //FIXME: fix here the method in game engine
        // this->_engine->setLocalPlayerEntity(entity, playerId);
        LOG_INFO_CAT("Coordinator", "Local player created with ID {}", playerId);
    }
}

void Coordinator::setupScoreEntity(
    Entity entity,
    uint32_t /*scoreId*/,
    float posX,
    float posY,
    uint32_t initialScore
)
{
    if (!_engine->hasComponent<Transform>(entity)) {
        _engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, 1.f));
    } else {
        _engine->updateComponent<Transform>(entity, Transform(posX, posY, 0.f, 1.f));
    }

    if (!_engine->hasComponent<Score>(entity)) {
        _engine->addComponent<Score>(entity, Score{initialScore});
    } else {
        _engine->updateComponent<Score>(entity, Score{initialScore});
    }

    const std::string scoreStr = std::to_string(initialScore);

    if (!_engine->hasComponent<Text>(entity)) {
        _engine->addComponent<Text>(
            entity,
            Text(scoreStr.c_str(), sf::Color::White, 30, ZIndex::IS_UI_HUD)
        );
    } else {
        _engine->updateComponent<Text>(
            entity,
            Text(scoreStr.c_str(), sf::Color::White, 30, ZIndex::IS_UI_HUD)
        );
    }
}

void Coordinator::setupEnemyEntity(
    Entity entity,
    uint32_t enemyId,
    float posX,
    float posY,
    float velX,
    float velY,
    uint16_t initialHealth,
    EnemyType enemyType,
    bool withRenderComponents
)
{
    // Determine scale based on enemy type
    float enemyScale = BASE_ENEMY_SCALE;
    switch (enemyType) {
        case EnemyType::BASIC:
            enemyScale = BASE_ENEMY_SCALE;
            break;
        case EnemyType::FAST:
            enemyScale = FAST_ENEMY_SCALE;
            break;
        case EnemyType::TANK:
            enemyScale = TANK_ENEMY_SCALE;
            break;
        default:
            enemyScale = BASE_ENEMY_SCALE;
            break;
    }

    this->_engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, enemyScale));
    this->_engine->addComponent<Velocity>(entity, Velocity(velX, velY));
    this->_engine->addComponent<Health>(entity, Health(initialHealth, initialHealth));
    
    // Add NetworkId so enemy is synchronized to clients
    // Use the entity's actual internal ID (which includes NETWORKED_ID_OFFSET)
    this->_engine->addComponent<NetworkId>(entity, NetworkId{static_cast<uint32_t>(entity), false});
    
    // Always add Sprite (needed for CollisionSystem even on server)
    // IntRect: (left, top, width, height) - defines which part of the texture to display
    this->_engine->addComponent<Sprite>(entity, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, BASE_ENEMY_SPRITE_WIDTH, BASE_ENEMY_SPRITE_HEIGHT)));
    this->_engine->addComponent<HitBox>(entity, HitBox());
    
    // Note: No Animation component - enemies are static sprites (no animation/rotation)
    // If animation is needed later, configure startFrame == endFrame for static display
    // or set proper animation frames for actual sprite animation
    
    LOG_DEBUG_CAT("Coordinator", "setupEnemyEntity: entity={} enemyId={} type={} scale={} sprite.rect=({},{},{}x{})",
        static_cast<size_t>(entity), enemyId, static_cast<int>(enemyType), enemyScale, 0, 0, BASE_ENEMY_SPRITE_WIDTH, BASE_ENEMY_SPRITE_HEIGHT);
    
    // Add Weapon and AI components based on enemy type
    switch (enemyType) {
        case EnemyType::BASIC:
            this->_engine->addComponent<Weapon>(entity, Weapon(BASE_ENEMY_WEAPON_FIRE_RATE, 0, BASE_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
            this->_engine->addComponent<AI>(entity, AI(AiBehaviour::SHOOTER_TACTIC, 50.f, 50.f));
            break;
        case EnemyType::FAST:
            this->_engine->addComponent<Weapon>(entity, Weapon(FAST_ENEMY_WEAPON_FIRE_RATE, 0, FAST_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
            this->_engine->addComponent<AI>(entity, AI(AiBehaviour::KAMIKAZE, 50.f, 50.f));
            break;
        case EnemyType::TANK:
            this->_engine->addComponent<Weapon>(entity, Weapon(TANK_ENEMY_WEAPON_FIRE_RATE, 0, TANK_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
            this->_engine->addComponent<AI>(entity, AI(AiBehaviour::SHOOTER_TACTIC, 50.f, 50.f));
            break;
        case EnemyType::BOSS:
            // TODO: Implement boss weapons/AI
            break;
        default:
            break;
    }

    this->_engine->addComponent<Team>(entity, TeamType::ENEMY);
    this->_engine->addComponent<Enemy>(entity, Enemy{enemyType});
    
    if (withRenderComponents) {
        this->_engine->addComponent<Drawable>(entity, Drawable{});
    }
}

void Coordinator::setupProjectileEntity(
    Entity entity,
    Entity shooterId,
    float posX,
    float posY,
    float velX,
    float velY,
    bool isPlayerProjectile,
    uint16_t damage,
    bool withRenderComponents
)
{
    // IMPORTANT: Projectiles do NOT get a NetworkId component!
    // This is intentional - projectiles have predictable linear movement
    // and don't need to be synchronized via transform snapshots.
    // The server spawns them via WEAPON_FIRE packets with position + velocity,
    // and clients simulate movement locally using the MovementSystem.
    // The server only sends ENTITY_DESTROY or hit events when needed.
    
    this->_engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, 1.f));
    this->_engine->addComponent<Velocity>(entity, Velocity(velX, velY));
    if (withRenderComponents) {
        this->_engine->addComponent<Sprite>(entity, Sprite(DEFAULT_BULLET, ZIndex::IS_GAME, sf::IntRect(0, 0, 16, 16)));
    }
    this->_engine->addComponent<HitBox>(entity, HitBox());
    this->_engine->addComponent<Projectile>(entity, Projectile(shooterId, isPlayerProjectile, damage));
    // Projectiles inherit team from their shooter
    this->_engine->addComponent<Team>(entity, isPlayerProjectile ? TeamType::PLAYER : TeamType::ENEMY);
}

Entity Coordinator::createLevelEntity(int levelNumber, float duration, const std::string& backgroundAsset, const std::string& soundTheme)
{
    // Create level entity with LOCAL category - use IDs in the LOCAL range (0 to 9,999)
    // Use a reserved range starting from 5,000 for special server-only level entities
    constexpr uint32_t LEVEL_ENTITY_ID_BASE = 5000;
    uint32_t levelEntityId = LEVEL_ENTITY_ID_BASE + static_cast<uint32_t>(levelNumber);
    Entity levelEntity = this->_engine->createEntityWithId(levelEntityId, "Level_" + std::to_string(levelNumber), EntityCategory::LOCAL);

    Level level;
    level.levelDuration = duration;
    level.backgroundAsset = backgroundAsset;
    level.soundTheme = soundTheme;
    level.started = false;
    level.completed = false;
    level.elapsedTime = 0.f;
    level.currentWaveIndex = 0;

    // Define level waves based on level number
    // Example for Level 1
    if (levelNumber == LEVEL_1_NUMBER) {
        // Wave 1: Basic enemies
        Wave wave1;
        wave1.startTime = LEVEL_1_WAVE_1_START_TIME;
        wave1.enemies.push_back({EnemyType::BASIC, 800.f, 100.f, 0.f});
        wave1.enemies.push_back({EnemyType::BASIC, 800.f, 200.f, 1.5f});
        wave1.enemies.push_back({EnemyType::BASIC, 800.f, 300.f, 1.5f});

        // Wave 2: Mix of basic and fast enemies
        //Wave wave2;
        //wave2.startTime = LEVEL_1_WAVE_2_START_TIME;
        //wave2.enemies.push_back({EnemyType::FAST, 800.f, 150.f, 0.f});
        //wave2.enemies.push_back({EnemyType::BASIC, 800.f, 250.f, 1.0f});
        //wave2.enemies.push_back({EnemyType::FAST, 800.f, 350.f, 1.0f});
        //wave2.enemies.push_back({EnemyType::BASIC, 800.f, 450.f, 1.0f});

        // Wave 3: Tank wave
        //Wave wave3;
        //wave3.startTime = LEVEL_1_WAVE_3_START_TIME;
        //wave3.enemies.push_back({EnemyType::TANK, 800.f, 200.f, 0.f});
        //wave3.enemies.push_back({EnemyType::FAST, 800.f, 100.f, 2.0f});
        //wave3.enemies.push_back({EnemyType::FAST, 800.f, 300.f, 0.5f});

        // Wave 4: Final assault
        //Wave wave4;
        //wave4.startTime = LEVEL_1_WAVE_4_START_TIME;
        //wave4.enemies.push_back({EnemyType::FAST, 800.f, 100.f, 0.f});
        //wave4.enemies.push_back({EnemyType::BASIC, 800.f, 200.f, 0.5f});
        //wave4.enemies.push_back({EnemyType::TANK, 800.f, 300.f, 0.5f});
        //wave4.enemies.push_back({EnemyType::FAST, 800.f, 400.f, 0.5f});
        //wave4.enemies.push_back({EnemyType::BASIC, 800.f, 500.f, 0.5f});

        level.waves.push_back(wave1);
        //level.waves.push_back(wave2);
        //level.waves.push_back(wave3);
        //level.waves.push_back(wave4);
    }
    // Can add more levels later
    else {
        LOG_WARN_CAT("Coordinator", "Level {} not configured, using default waves", levelNumber);
        // Default simple wave
        Wave defaultWave;
        defaultWave.startTime = 2.0f;
        defaultWave.enemies.push_back({EnemyType::BASIC, 800.f, 200.f, 0.f});
        level.waves.push_back(defaultWave);
    }

    this->_engine->addComponent<Level>(levelEntity, level);
    LOG_INFO_CAT("Coordinator", "Created level {} with {} waves, duration={}s, background={}, music={}", 
                 levelNumber, level.waves.size(), duration, backgroundAsset, soundTheme);

    return levelEntity;
}

void Coordinator::processServerPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
{
    LOG_INFO_CAT("Coordinator", "processServerPackets: processing {} packets", packetsToProcess.size());

    for (const auto& packet : packetsToProcess) {
        const auto packetType = static_cast<protocol::PacketTypes>(packet.header.packet_type);
        
        if (packetType == protocol::PacketTypes::TYPE_PLAYER_INPUT && PacketManager::assertPlayerInput(packet)) {
            LOG_INFO_CAT("Coordinator", "Processing PLAYER_INPUT packet");
            
            // Parse to get the source player ID
            auto parsed = PacketManager::parsePlayerInput(packet);
            if (parsed.has_value()) {
                handlePlayerInputPacket(packet, elapsedMs);
                // Queue this packet to be relayed to OTHER clients (exclude the source)
                RelayPacket relayPacket;
                relayPacket.packet = packet;
                relayPacket.sourcePlayerId = parsed->playerId;
                _packetsToRelay.push_back(relayPacket);
                LOG_DEBUG_CAT("Coordinator", "Queued PLAYER_INPUT packet from player {} for relay to other clients (excluding source)", parsed->playerId);
            } else {
                LOG_ERROR_CAT("Coordinator", "Failed to parse PLAYER_INPUT packet");
            }
        } else if (packetType == protocol::PacketTypes::TYPE_PLAYER_IS_READY && PacketManager::assertPlayerIsReady(packet)) {
            LOG_INFO_CAT("Coordinator", "Processing PLAYER_IS_READY packet");
            handlePacketPlayerIsReady(packet);
        } else if (packetType == protocol::PacketTypes::TYPE_PLAYER_NOT_READY && PacketManager::assertPlayerNotReady(packet)) {
            LOG_INFO_CAT("Coordinator", "Processing PLAYER_NOT_READY packet");
            handlePacketPlayerNotReady(packet);
        } else {
            LOG_WARN_CAT("Coordinator", "Received non-input packet type {} on server", static_cast<int>(packet.header.packet_type));
        }
    }
}

void Coordinator::processClientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
{
    for (const auto& packet : packetsToProcess) {
        // Check packet type first, then validate
        uint8_t packetType = packet.header.packet_type;

        switch (packetType) {
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT):
                if (PacketManager::assertPlayerInput(packet)) {
                    LOG_DEBUG_CAT("Coordinator", "Client received relayed PLAYER_INPUT packet");
                    handleRelayedPlayerInputPacket(packet, elapsedMs);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN):
                if (PacketManager::assertEntitySpawn(packet)) {
                    this->handlePacketCreateEntity(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_DESTROY):
                if (PacketManager::assertEntityDestroy(packet)) {
                    handlePacketDestroyEntity(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT):
                if (PacketManager::assertTransformSnapshot(packet)) {
                    this->handlePacketTransformSnapshot(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT):
                if (PacketManager::assertHealthSnapshot(packet)) {
                    handlePacketHealthSnapshot(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT):
                if (PacketManager::assertWeaponSnapshot(packet)) {
                    handlePacketWeaponSnapshot(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT):
                if (PacketManager::assertAnimationSnapshot(packet)) {
                    // TODO: handle animation snapshot
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_REMOVE):
                if (PacketManager::assertComponentRemove(packet)) {
                    handlePacketComponentRemove(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA):
                if (PacketManager::assertTransformSnapshotDelta(packet)) {
                    handlePacketTransformSnapshotDelta(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA):
                if (PacketManager::assertHealthSnapshotDelta(packet)) {
                    handlePacketHealthSnapshotDelta(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_HIT):
                if (PacketManager::assertPlayerHit(packet)) {
                    handlePacketPlayerHit(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_DEATH):
                if (PacketManager::assertPlayerDeath(packet)) {
                    handlePacketPlayerDeath(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_SCORE_UPDATE):
                if (PacketManager::assertScoreUpdate(packet)) {
                    handlePacketScoreUpdate(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_POWER_PICKUP):
                if (PacketManager::assertPowerupPickup(packet)) {
                    handlePacketPowerupPickup(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE):
                if (PacketManager::assertWeaponFire(packet)) {
                    handlePacketWeaponFire(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_VISUAL_EFFECT):
                if (PacketManager::assertVisualEffect(packet)) {
                    handlePacketVisualEffect(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_AUDIO_EFFECT):
                if (PacketManager::assertAudioEffect(packet)) {
                    handlePacketAudioEffect(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PARTICLE_SPAWN):
                if (PacketManager::assertParticleSpawn(packet)) {
                    handlePacketParticleSpawn(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_START):
                if (PacketManager::assertGameStart(packet)) {
                    this->handleGameStart(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_END):
                if (PacketManager::assertGameEnd(packet)) {
                    this->handleGameEnd(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_COMPLETE):
                if (PacketManager::assertLevelComplete(packet)) {
                    handlePacketLevelComplete(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_START):
                if (PacketManager::assertLevelStart(packet)) {
                    handlePacketLevelStart(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_FORCE_STATE):
                if (PacketManager::assertForceState(packet)) {
                    handlePacketForceState(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_STATE):
                if (PacketManager::assertAIState(packet)) {
                    handlePacketAIState(packet);
                }
                break;
            default:
                // Unknown packet type, ignore
                break;
        }
    }
}

void Coordinator::queueWeaponFire(uint32_t shooterId, float originX, float originY,
                                  float directionX, float directionY, uint8_t weaponType)
{
    // Create projectile immediately to reserve the entity ID
    // (prevents race condition where LevelSystem might take the ID before we use it)
    // Use getNextNetworkedEntityId() because projectiles are networked entities
    uint32_t projectileId = _engine->getNextNetworkedEntityId();
    Entity shooterEntity = _engine->getEntityFromId(shooterId);
    Entity projectile = spawnProjectile(shooterEntity, projectileId, weaponType, originX, originY, directionX, directionY);
    
    // Queue the weapon fire event with the already-created projectile
    WeaponFireEvent event;
    event.shooterId = shooterId;
    event.projectileId = projectileId;
    event.originX = originX;
    event.originY = originY;
    event.directionX = directionX;
    event.directionY = directionY;
    event.weaponType = weaponType;
    
    _pendingWeaponFires.push_back(event);
    
    LOG_DEBUG_CAT("Coordinator", "queueWeaponFire: shooter={} projectile={} origin=({}, {}) dir=({}, {})",
                  shooterId, event.projectileId, originX, originY, directionX, directionY);
}

// Should create packets based on server game state using define.hpp 
void Coordinator::buildServerPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
{
    // ============================================================================
    // RELAY PLAYER INPUT PACKETS
    // ============================================================================
    // Relay any PLAYER_INPUT packets received from clients to all OTHER clients
    // This ensures all clients see each other's input for animation updates
    // IMPORTANT: Each packet is marked with sourcePlayerId so the network layer
    // can exclude sending the packet back to the originating player
    for (const auto& relayPacket : _packetsToRelay) {
        outgoingPackets.push_back(relayPacket.packet);
        LOG_DEBUG_CAT("Coordinator", "Relaying PLAYER_INPUT packet from player {} to other clients (network layer should exclude source)", relayPacket.sourcePlayerId);
    }
    _packetsToRelay.clear();

    // ============================================================================
    // NEW ENTITY BROADCAST
    // ============================================================================
    // Detect newly created entities with NetworkId and broadcast ENTITY_SPAWN packets
    // This ensures clients know about new enemies, powerups, etc. created server-side
    // ============================================================================
    auto& networkIdComponents = this->_engine->getComponents<NetworkId>();
    const auto& networkedEntities = this->_engine->getNetworkedEntities();
    static uint32_t entitySpawnSequence = 0;
    
    for (size_t entityId : networkedEntities) {
        if (entityId < networkIdComponents.size() && networkIdComponents[entityId].has_value()) {
            uint32_t networkId = networkIdComponents[entityId].value().id;
            Entity entity = Entity::fromId(static_cast<uint32_t>(entityId));
            
            // Check if this entity has been broadcast yet
            if (this->_engine->isAlive(entity) && _broadcastedEntityIds.find(networkId) == _broadcastedEntityIds.end()) {
                // New entity! Create and send ENTITY_SPAWN packet
                common::protocol::Packet spawnPacket;
                entitySpawnSequence++;
                if (createPacketEntitySpawn(&spawnPacket, networkId, entitySpawnSequence)) {
                    outgoingPackets.push_back(spawnPacket);
                    _broadcastedEntityIds.insert(networkId);
                    LOG_INFO_CAT("Coordinator", "Broadcasting ENTITY_SPAWN for new entity {}", networkId);
                }
            }
        }
    }

    // ============================================================================
    // SERVER SNAPSHOT GENERATION
    // ============================================================================
    // Only entities with NetworkId component are synchronized via snapshots.
    // Projectiles intentionally DON'T have NetworkId - they are spawned via
    // WEAPON_FIRE packets and simulated locally on clients using their velocity.
    // This avoids unnecessary network traffic for predictable linear movement.
    // ============================================================================
    
    // Get all networked entities (entities with NetworkId component)

    if (networkedEntities.size() == 0) {
        // No entities to sync
        return;
    }

    // Collect all entity IDs that need to be synchronized
    // (Players, enemies, powerups - but NOT projectiles)
    std::vector<uint32_t> entityIds;
    for (size_t entityId : networkedEntities) {
        if (entityId < networkIdComponents.size() && networkIdComponents[entityId].has_value()) {
            Entity entity = Entity::fromId(static_cast<uint32_t>(entityId));
            if (this->_engine->isAlive(entity)) {
                entityIds.push_back(networkIdComponents[entityId].value().id);
            }
        }
    }

    if (entityIds.empty()) {
        return;
    }

    // Use a static counter for sequence numbers
    static uint32_t sequenceNumber = 0;
    sequenceNumber++;

    // Create Transform Snapshot for all networked entities
    common::protocol::Packet transformPacket;
    if (createPacketTransformSnapshot(&transformPacket, entityIds, sequenceNumber)) {
        outgoingPackets.push_back(transformPacket);
        LOG_INFO_CAT("Coordinator", "buildServerPacketBasedOnStatus: created Transform snapshot for {} entities (seq={})", entityIds.size(), sequenceNumber);
    }

    // Create Health Snapshot for all networked entities with Health component
    std::vector<uint32_t> healthEntityIds;
    for (uint32_t entityId : entityIds) {
        Entity entity = this->_engine->getEntityFromId(entityId);
        if (this->_engine->isAlive(entity)) {
            auto healthOpt = this->_engine->getComponentEntity<Health>(entity);
            if (healthOpt.has_value()) {
                healthEntityIds.push_back(entityId);
            }
        }
    }

    if (!healthEntityIds.empty()) {
        common::protocol::Packet healthPacket;
        if (createPacketHealthSnapshot(&healthPacket, healthEntityIds, sequenceNumber)) {
            outgoingPackets.push_back(healthPacket);
            LOG_DEBUG_CAT("Coordinator", "buildSeverPacketBasedOnStatus: created Health snapshot for {} entities", healthEntityIds.size());
        }
    }

    // Create Weapon Snapshot for all networked entities with Weapon component
    std::vector<uint32_t> weaponEntityIds;
    for (uint32_t entityId : entityIds) {
        Entity entity = this->_engine->getEntityFromId(entityId);
        if (this->_engine->isAlive(entity)) {
            auto weaponOpt = this->_engine->getComponentEntity<Weapon>(entity);
            if (weaponOpt.has_value()) {
                weaponEntityIds.push_back(entityId);
            }
        }
    }

    if (!weaponEntityIds.empty()) {
        common::protocol::Packet weaponPacket;
        if (createPacketWeaponSnapshot(&weaponPacket, weaponEntityIds, sequenceNumber)) {
            outgoingPackets.push_back(weaponPacket);
            LOG_DEBUG_CAT("Coordinator", "buildSeverPacketBasedOnStatus: created Weapon snapshot for {} entities", weaponEntityIds.size());
        }
    }

    // ============================================================================
    // WEAPON FIRE EVENTS → PROJECTILE SPAWNING
    // ============================================================================
    // When an entity fires a weapon:
    // 1. Server spawns projectile locally with spawnProjectile()
    // 2. Server broadcasts WEAPON_FIRE packet to all clients
    // 3. Clients receive packet and spawn projectile locally with same velocity
    // 4. Both server and clients simulate projectile movement via MovementSystem
    // 5. Server sends ENTITY_DESTROY or collision events when needed
    // 
    // This eliminates the need to sync projectile positions every tick!
    // ============================================================================
    
    // Process pending weapon fire events and create packets
    for (const auto& fireEvent : _pendingWeaponFires) {
        // Projectile was already spawned in queueWeaponFire() to reserve the ID
        // Just verify the shooter is still alive and create the network packet
        Entity shooterEntity = this->_engine->getEntityFromId(fireEvent.shooterId);
        if (this->_engine->isAlive(shooterEntity)) {
            // Create weapon fire packet to broadcast to clients
            // Args format: flags_count(1) + flags(1+) + sequence_number(4) + timestamp(4) + payload(17)
            std::vector<uint8_t> weaponFireArgs;
            weaponFireArgs.resize(WEAPON_FIRE_MIN_ARGS_SIZE);
            uint8_t* ptr = weaponFireArgs.data();

            // flags_count (1 byte)
            uint8_t flagsCount = 1;
            std::memcpy(ptr, &flagsCount, sizeof(uint8_t));
            ptr += sizeof(uint8_t);

            // flags (1 byte) - RELIABLE
            uint8_t flags = 0x01;
            std::memcpy(ptr, &flags, sizeof(uint8_t));
            ptr += sizeof(uint8_t);

            // sequence_number (4 bytes)
            std::memcpy(ptr, &sequenceNumber, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            // timestamp (4 bytes)
            uint32_t timestamp = static_cast<uint32_t>(elapsedMs);
            std::memcpy(ptr, &timestamp, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            // shooter_id (4 bytes)
            std::memcpy(ptr, &fireEvent.shooterId, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            // projectile_id (4 bytes)
            std::memcpy(ptr, &fireEvent.projectileId, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            // origin_x (2 bytes)
            int16_t origin_x = static_cast<int16_t>(fireEvent.originX);
            std::memcpy(ptr, &origin_x, sizeof(int16_t));
            ptr += sizeof(int16_t);

            // origin_y (2 bytes)
            int16_t origin_y = static_cast<int16_t>(fireEvent.originY);
            std::memcpy(ptr, &origin_y, sizeof(int16_t));
            ptr += sizeof(int16_t);

            // direction_x (2 bytes) - normalized * 1000
            int16_t direction_x = static_cast<int16_t>(fireEvent.directionX * 1000.0f);
            std::memcpy(ptr, &direction_x, sizeof(int16_t));
            ptr += sizeof(int16_t);

            // direction_y (2 bytes) - normalized * 1000
            int16_t direction_y = static_cast<int16_t>(fireEvent.directionY * 1000.0f);
            std::memcpy(ptr, &direction_y, sizeof(int16_t));
            ptr += sizeof(int16_t);

            // weapon_type (1 byte)
            std::memcpy(ptr, &fireEvent.weaponType, sizeof(uint8_t));

            LOG_DEBUG_CAT("Coordinator", "WeaponFire packet: shooter={} proj={} origin=({},{}) dir=({},{}) weapon_type=0x{:02x}",
                fireEvent.shooterId, fireEvent.projectileId, origin_x, origin_y, direction_x, direction_y, fireEvent.weaponType);

            auto weaponFirePacket = PacketManager::createWeaponFire(weaponFireArgs);
            if (weaponFirePacket.has_value()) {
                outgoingPackets.push_back(weaponFirePacket.value());
                LOG_INFO_CAT("Coordinator", "buildServerPacketBasedOnStatus: created weapon fire packet for shooter {} projectile {}", 
                             fireEvent.shooterId, fireEvent.projectileId);
            }
        }
    }
    
    // Clear processed weapon fires
    _pendingWeaponFires.clear();

    LOG_DEBUG_CAT("Coordinator", "buildSeverPacketBasedOnStatus: created {} snapshot packets total", outgoingPackets.size());
}

std::optional<common::protocol::Packet> Coordinator::spawnPlayerOnServer(uint32_t playerId, float posX, float posY)
{
    // Create the player entity on the server
    Entity playerEntity = this->createPlayerEntity(
        playerId,
        posX,
        posY,
        0.0f,  // initial velocity X
        0.0f,  // initial velocity Y
        100,   // initial health
        false, // not playable on server
        false  // no render components on server
    );

    // Get the actual internal entity ID (with offset)
    uint32_t entityId = static_cast<uint32_t>(playerEntity);

    LOG_INFO_CAT("Coordinator", "Spawned player entity for client ID {} with internal entity ID {}", playerId, entityId);

    // Create ENTITY_SPAWN packet to broadcast to all clients
    std::vector<uint8_t> args;

    // flags_count (1 byte)
    args.push_back(1);

    // flags (FLAG_RELIABLE)
    args.push_back(static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE));

    // sequence_number (4 bytes)
    static uint32_t sequence = 0;
    sequence++;
    args.push_back(static_cast<uint8_t>(sequence & 0xFF));
    args.push_back(static_cast<uint8_t>((sequence >> 8) & 0xFF));
    args.push_back(static_cast<uint8_t>((sequence >> 16) & 0xFF));
    args.push_back(static_cast<uint8_t>((sequence >> 24) & 0xFF));

    // timestamp (4 bytes)
    uint32_t timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count());
    args.push_back(static_cast<uint8_t>(timestamp & 0xFF));
    args.push_back(static_cast<uint8_t>((timestamp >> 8) & 0xFF));
    args.push_back(static_cast<uint8_t>((timestamp >> 16) & 0xFF));
    args.push_back(static_cast<uint8_t>((timestamp >> 24) & 0xFF));

    // entity_id (4 bytes) - use the actual internal entity ID
    args.push_back(static_cast<uint8_t>(entityId & 0xFF));
    args.push_back(static_cast<uint8_t>((entityId >> 8) & 0xFF));
    args.push_back(static_cast<uint8_t>((entityId >> 16) & 0xFF));
    args.push_back(static_cast<uint8_t>((entityId >> 24) & 0xFF));

    // entity_type (1 byte) - ENTITY_TYPE_PLAYER
    args.push_back(static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER));

    // position_x (2 bytes)
    uint16_t px = static_cast<uint16_t>(posX);
    args.push_back(static_cast<uint8_t>(px & 0xFF));
    args.push_back(static_cast<uint8_t>((px >> 8) & 0xFF));

    // position_y (2 bytes)
    uint16_t py = static_cast<uint16_t>(posY);
    args.push_back(static_cast<uint8_t>(py & 0xFF));
    args.push_back(static_cast<uint8_t>((py >> 8) & 0xFF));

    // mob_variant (1 byte) - not used for player
    args.push_back(0);

    // initial_health (1 byte)
    args.push_back(100);

    // initial_velocity_x (2 bytes)
    args.push_back(0);
    args.push_back(0);

    // initial_velocity_y (2 bytes)
    args.push_back(0);
    args.push_back(0);

    // is_playable (1 byte) - will be 0 for other clients, 1 for owning client
    // Note: The network manager should handle setting this to 1 for the owning client
    args.push_back(0);

    // Mark this entity as already broadcasted to prevent duplicate ENTITY_SPAWN from auto-broadcast system
    // This must be done BEFORE returning so the next buildServerPacketBasedOnStatus() tick won't resend it
    // Use the actual internal entity ID (with offset)
    this->markEntityAsBroadcasted(entityId);

    return PacketManager::createEntitySpawn(args);
}

void Coordinator::markEntityAsBroadcasted(uint32_t entityId)
{
    _broadcastedEntityIds.insert(entityId);
    LOG_DEBUG_CAT("Coordinator", "Marked entity {} as already broadcasted", entityId);
}

bool Coordinator::shouldSendPacketToPlayer(const common::protocol::Packet& packet, uint32_t targetPlayerId) const
{
    // Check if this is a PLAYER_INPUT packet
    if (packet.header.packet_type != static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT)) {
        // Non-PLAYER_INPUT packets are sent to all players
        return true;
    }

    // This is a PLAYER_INPUT packet - parse it to get the source player
    auto parsed = PacketManager::parsePlayerInput(packet);
    if (!parsed.has_value()) {
        // If we can't parse it, be safe and exclude it
        LOG_WARN_CAT("Coordinator", "shouldSendPacketToPlayer: Failed to parse PLAYER_INPUT packet");
        return false;
    }

    // Don't send the packet to the player who sent it (avoid double-processing)
    // But send it to all other players
    if (parsed->playerId == targetPlayerId) {
        LOG_DEBUG_CAT("Coordinator", "Excluding PLAYER_INPUT from player {} to themselves", targetPlayerId);
        return false;
    }

    return true;
}

std::vector<uint32_t> Coordinator::getPlayablePlayerIds()
{
    std::vector<uint32_t> playerIds;
    auto& playableComponents = this->_engine->getComponents<Playable>();
    auto& inputComponents = this->_engine->getComponents<InputComponent>();

    // Iterate over networked entities only (not the entire vector)
    const auto& networkedEntities = this->_engine->getNetworkedEntities();
    LOG_DEBUG_CAT("Coordinator", "getPlayablePlayerIds: checking {} networked entities", networkedEntities.size());

    for (size_t entityId : networkedEntities) {
        if (entityId < playableComponents.size() && playableComponents[entityId].has_value()) {
            // Get the InputComponent to retrieve the actual player ID
            if (entityId < inputComponents.size() && inputComponents[entityId].has_value()) {
                uint32_t playerId = inputComponents[entityId].value().playerId;
                playerIds.push_back(playerId);
                LOG_DEBUG_CAT("Coordinator", "  Found playable entity {} with playerId {}", entityId, playerId);
            } else {
                LOG_WARN_CAT("Coordinator", "  Playable entity {} has no InputComponent", entityId);
            }
        }
    }

    LOG_DEBUG_CAT("Coordinator", "getPlayablePlayerIds: found {} playable entities", playerIds.size());
    return playerIds;
}

std::vector<uint32_t> Coordinator::getAllConnectedPlayerIds() const
{
    std::vector<uint32_t> playerIds;
    auto& inputComponents = this->_engine->getComponents<InputComponent>();

    // Iterate over networked entities only (not the entire vector)
    const auto& networkedEntities = this->_engine->getNetworkedEntities();
    LOG_DEBUG_CAT("Coordinator", "getAllConnectedPlayerIds: checking {} networked entities", networkedEntities.size());

    for (size_t entityId : networkedEntities) {
        if (entityId < inputComponents.size() && inputComponents[entityId].has_value()) {
            uint32_t playerId = inputComponents[entityId].value().playerId;
            playerIds.push_back(playerId);
            LOG_DEBUG_CAT("Coordinator", "  Found player entity {} with playerId {}", entityId, playerId);
        }
    }

    LOG_DEBUG_CAT("Coordinator", "getAllConnectedPlayerIds: found {} player entities", playerIds.size());
    return playerIds;
}

// Should only be called on client side to send input packets to server
void Coordinator::buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
{
    // ============================================================================
    // HANDLE PLAYER READY/NOT READY EVENTS
    // ============================================================================
    // Create PLAYER_IS_READY and PLAYER_NOT_READY packets from pending events
    for (const auto& event : _pendingPlayerReadyEvents) {
        std::vector<uint8_t> args;

        // flags_count (1 byte)
        args.push_back(1);

        // flags (FLAG_RELIABLE)
        args.push_back(static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE));

        // sequence_number (4 bytes)
        uint32_t sequence = static_cast<uint32_t>(std::time(nullptr));
        args.push_back(static_cast<uint8_t>(sequence & 0xFF));
        args.push_back(static_cast<uint8_t>((sequence >> 8) & 0xFF));
        args.push_back(static_cast<uint8_t>((sequence >> 16) & 0xFF));
        args.push_back(static_cast<uint8_t>((sequence >> 24) & 0xFF));

        // timestamp (4 bytes)
        uint32_t timestamp = static_cast<uint32_t>(std::time(nullptr));
        args.push_back(static_cast<uint8_t>(timestamp & 0xFF));
        args.push_back(static_cast<uint8_t>((timestamp >> 8) & 0xFF));
        args.push_back(static_cast<uint8_t>((timestamp >> 16) & 0xFF));
        args.push_back(static_cast<uint8_t>((timestamp >> 24) & 0xFF));

        // player_id (4 bytes)
        args.push_back(static_cast<uint8_t>(event.playerId & 0xFF));
        args.push_back(static_cast<uint8_t>((event.playerId >> 8) & 0xFF));
        args.push_back(static_cast<uint8_t>((event.playerId >> 16) & 0xFF));
        args.push_back(static_cast<uint8_t>((event.playerId >> 24) & 0xFF));

        // Create the appropriate packet type
        std::optional<common::protocol::Packet> packet;
        if (event.isReady) {
            packet = PacketManager::createPlayerIsReady(args);
            LOG_INFO_CAT("Coordinator", "buildClientPacketBasedOnStatus: created PLAYER_IS_READY packet for player {}", event.playerId);
        } else {
            packet = PacketManager::createPlayerNotReady(args);
            LOG_INFO_CAT("Coordinator", "buildClientPacketBasedOnStatus: created PLAYER_NOT_READY packet for player {}", event.playerId);
        }

        if (packet.has_value()) {
            outgoingPackets.push_back(packet.value());
        } else {
            LOG_ERROR_CAT("Coordinator", "buildClientPacketBasedOnStatus: failed to create player ready/not ready packet");
        }
    }
    _pendingPlayerReadyEvents.clear();

    // ============================================================================
    // THROTTLE INPUT PACKET SENDING
    // ============================================================================
    // Throttle input packet sending using INPUT_SEND_TICK_INTERVAL
    static uint64_t tickCounter = 0;
    tickCounter++;

    auto playerIds = getPlayablePlayerIds();

    LOG_DEBUG_CAT("Coordinator", "buildClientPacketBasedOnStatus: found {} playable players, tick={}", playerIds.size(), tickCounter);

    // Only send input every INPUT_SEND_TICK_INTERVAL ticks (defined in defines.hpp as 2)
    // This limits input packets to ~30 Hz instead of 60 Hz
    if (tickCounter % INPUT_SEND_TICK_INTERVAL != 0) {
        LOG_DEBUG_CAT("Coordinator", "buildClientPacketBasedOnStatus: created {} packets", outgoingPackets.size());
        return;
    }

    // Should only have one playable per client
    if (!playerIds.empty()) {
        LOG_INFO_CAT("Coordinator", "buildClientPacketBasedOnStatus: creating input packet for player {}", playerIds[0]);
        common::protocol::Packet packet;
        if (createPacketInputClient(&packet, playerIds[0])) {
            outgoingPackets.push_back(packet);
            LOG_INFO_CAT("Coordinator", "buildClientPacketBasedOnStatus: input packet created and queued for player {}", playerIds[0]);
        } else {
            LOG_WARN_CAT("Coordinator", "buildClientPacketBasedOnStatus: failed to create input packet for player {}", playerIds[0]);
        }
    } else {
        LOG_DEBUG_CAT("Coordinator", "buildClientPacketBasedOnStatus: no playable players found");
    }

    LOG_DEBUG_CAT("Coordinator", "buildClientPacketBasedOnStatus: created {} packets", outgoingPackets.size());
}

bool Coordinator::createPacketInputClient(common::protocol::Packet* packet, uint32_t playerId)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketInputClient: packet pointer is null");
        return false;
    }

    LOG_DEBUG_CAT("Coordinator", "createPacketInputClient: called with playerId={}", playerId);

    // Get the entity for the player ID (convert network-relative ID to internal ID)
    Entity playerEntity = this->_engine->getEntityFromNetworkId(playerId, true);
    if (!this->_engine->isAlive(playerEntity)) {
        LOG_WARN_CAT("Coordinator", "createPacketInputClient: player entity {} is not alive", playerId);
        return false;
    }

    auto& inputOpt = this->_engine->getComponentEntity<InputComponent>(playerEntity);
    if (!inputOpt.has_value()) {
        LOG_WARN_CAT("Coordinator", "createPacketInputClient: player {} has no input component", playerId);
        return false;
    }

    InputComponent& inputComp = inputOpt.value();
    uint32_t actualPlayerId = inputComp.playerId;

    LOG_DEBUG_CAT("Coordinator", "createPacketInputClient: retrieved actualPlayerId={} from InputComponent", actualPlayerId);

    uint16_t inputState = 0;

    auto actionIt = inputComp.activeActions.find(GameAction::MOVE_UP);
    if (actionIt != inputComp.activeActions.end() && actionIt->second) {
        inputState |= static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_UP);
    }
    actionIt = inputComp.activeActions.find(GameAction::MOVE_DOWN);
    if (actionIt != inputComp.activeActions.end() && actionIt->second) {
        inputState |= static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_DOWN);
    }
    actionIt = inputComp.activeActions.find(GameAction::MOVE_LEFT);
    if (actionIt != inputComp.activeActions.end() && actionIt->second) {
        inputState |= static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_LEFT);
    }
    actionIt = inputComp.activeActions.find(GameAction::MOVE_RIGHT);
    if (actionIt != inputComp.activeActions.end() && actionIt->second) {
        inputState |= static_cast<uint16_t>(protocol::InputFlags::INPUT_MOVE_RIGHT);
    }
    actionIt = inputComp.activeActions.find(GameAction::SHOOT);
    if (actionIt != inputComp.activeActions.end() && actionIt->second) {
        inputState |= static_cast<uint16_t>(protocol::InputFlags::INPUT_FIRE_PRIMARY);
    }

    // Get client's current position from Transform component
    int16_t clientPosX = 0;
    int16_t clientPosY = 0;
    auto& transformOpt = this->_engine->getComponentEntity<Transform>(playerEntity);
    if (transformOpt.has_value()) {
        clientPosX = static_cast<int16_t>(transformOpt->x);
        clientPosY = static_cast<int16_t>(transformOpt->y);
    }

    // Format: [flags_count(1) + flags(1) + sequence_number(4) + timestamp(4) + player_id(4) + input_state(2) + aim_dir_x(2) + aim_dir_y(2) + client_pos_x(2) + client_pos_y(2)]
    std::vector<uint8_t> inputArgs;
    inputArgs.resize(24);  // 1 + 1 + 4 + 4 + 4 + 2 + 2 + 2 + 2 + 2
    uint8_t* ptr = inputArgs.data();

    // flags_count (1 byte) - 1 flag (RELIABLE)
    uint8_t flagsCount = 1;
    std::memcpy(ptr, &flagsCount, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    // flags (1 byte) - FLAG_RELIABLE
    uint8_t flags = 0x01;  // FLAG_RELIABLE
    std::memcpy(ptr, &flags, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    // TODO: sequence_number should be based on tick rate, not timestamp
    // sequence_number (4 bytes) - should be incremented based on tick rate
    uint32_t sequenceNumber = static_cast<uint32_t>(std::time(nullptr));
    std::memcpy(ptr, &sequenceNumber, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    // timestamp (4 bytes) - timestamp since epoch
    uint32_t timestamp = static_cast<uint32_t>(std::time(nullptr));
    std::memcpy(ptr, &timestamp, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    // player_id (4 bytes)
    std::memcpy(ptr, &actualPlayerId, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    // input_state (2 bytes)
    std::memcpy(ptr, &inputState, sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    // aim_direction_x (2 bytes) - hardcoded to 0 for now
    int16_t aimX = 0;
    std::memcpy(ptr, &aimX, sizeof(int16_t));
    ptr += sizeof(int16_t);

    // aim_direction_y (2 bytes) - hardcoded to 0 for now
    int16_t aimY = 0;
    std::memcpy(ptr, &aimY, sizeof(int16_t));
    ptr += sizeof(int16_t);

    // client_pos_x (2 bytes) - client's current X position for shoot sync
    std::memcpy(ptr, &clientPosX, sizeof(int16_t));
    ptr += sizeof(int16_t);

    // client_pos_y (2 bytes) - client's current Y position for shoot sync
    std::memcpy(ptr, &clientPosY, sizeof(int16_t));

    auto playerInputPacket = PacketManager::createPlayerInput(inputArgs);
    if (!playerInputPacket.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketInputClient: failed to create player input packet");
        return false;
    }

    *packet = playerInputPacket.value();
    LOG_DEBUG_CAT("Coordinator", "createPacketInputClient: packet created for player {} at pos ({}, {})", actualPlayerId, clientPosX, clientPosY);
    return true;
}

void Coordinator::handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs)
{
    // Parse the player input packet
    auto parsed = PacketManager::parsePlayerInput(packet);
    if (!parsed.has_value()) {
        LOG_ERROR_CAT("Coordinator", "handlePlayerInputPacket: failed to parse input packet");
        return;  // Invalid packet, ignore
    }

    LOG_INFO_CAT("Coordinator", "handlePlayerInputPacket: playerId={} actionCount={} clientPos=({}, {})",
                  parsed->playerId, parsed->actions.size(), parsed->clientPosX, parsed->clientPosY);

    // Find the entity for this player by iterating over networked entities only
    auto& inputComponents = this->_engine->getComponents<InputComponent>();
    const auto& networkedEntities = this->_engine->getNetworkedEntities();
    LOG_DEBUG_CAT("Coordinator", "handlePlayerInputPacket: checking {} networked entities", networkedEntities.size());

    bool foundPlayer = false;
    for (size_t entityId : networkedEntities) {
        if (entityId < inputComponents.size()) {
            auto& input = inputComponents[entityId];
            if (input.has_value() && input->playerId == parsed->playerId) {
                LOG_DEBUG_CAT("Coordinator", "handlePlayerInputPacket: found player {} at entity {}", parsed->playerId, entityId);
                Entity entity = Entity::fromId(entityId);
                // Update all input actions
                for (const auto& [action, isPressed] : parsed->actions) {
                    this->_engine->setPlayerInputAction(entity, parsed->playerId, action, isPressed);
                    LOG_DEBUG_CAT("Coordinator", "handlePlayerInputPacket: Player {} action {} set to {}", parsed->playerId, static_cast<int>(action), isPressed);
                }
                // Store client's reported position for shoot synchronization
                input->clientPosX = static_cast<float>(parsed->clientPosX);
                input->clientPosY = static_cast<float>(parsed->clientPosY);
                foundPlayer = true;
                break;
            }
        }
    }

    if (!foundPlayer) {
        LOG_WARN_CAT("Coordinator", "handlePlayerInputPacket: player {} not found in ECS", parsed->playerId);
    }
}

void Coordinator::handleRelayedPlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs)
{
    auto parsed = PacketManager::parsePlayerInput(packet);
    if (!parsed.has_value()) {
        LOG_ERROR_CAT("Coordinator", "handleRelayedPlayerInputPacket: failed to parse input packet");
        return;  // Invalid packet, ignore
    }

    LOG_DEBUG_CAT("Coordinator", "handleRelayedPlayerInputPacket: playerId={} actionCount={}",
                  parsed->playerId, parsed->actions.size());

    auto& inputComponents = this->_engine->getComponents<InputComponent>();
    const auto& networkedEntities = this->_engine->getNetworkedEntities();

    for (size_t entityId : networkedEntities) {
        if (entityId < inputComponents.size()) {
            auto& input = inputComponents[entityId];
            if (input.has_value() && input->playerId == parsed->playerId) {
                LOG_DEBUG_CAT("Coordinator", "handleRelayedPlayerInputPacket: found player {} at entity {}", parsed->playerId, entityId);
                Entity entity = Entity::fromId(entityId);

                for (const auto& [action, isPressed] : parsed->actions) {
                    input->activeActions[action] = isPressed;
                    LOG_DEBUG_CAT("Coordinator", "handleRelayedPlayerInputPacket: Directly updated Player {} action {} to {}",
                                 parsed->playerId, static_cast<int>(action), isPressed);
                }
                break;
            }
        }
    }
}

void Coordinator::handlePacketCreateEntity(const common::protocol::Packet& packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != ENTITY_SPAWN_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketCreateEntity: invalid packet size {}, expected {}", packet.data.size(), ENTITY_SPAWN_PAYLOAD_SIZE);
        return;
    }

    // Parse the ENTITY_SPAWN payload in one memcpy
    protocol::EntitySpawnPayload payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "Entity created: id={} type={} pos=({}, {}) health={} is_playable={}",
        payload.entity_id, payload.entity_type, static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), payload.initial_health, payload.is_playable);

    // Check if entity already exists in NETWORKED list (to avoid duplicate spawning)
    // Only check NETWORKED entities, not LOCAL entities (which can have the same IDs)
    const auto& networkedEntities = this->_engine->getNetworkedEntities();
    if (networkedEntities.find(payload.entity_id) != networkedEntities.end()) {
        LOG_WARN_CAT("Coordinator", "Entity with ID {} already exists in NETWORKED list, skipping spawn", payload.entity_id);
        return;
    }

    // Create the entity with the specific ID from the server
    Entity newEntity = this->_engine->createEntityWithId(payload.entity_id, "Entity_" + std::to_string(payload.entity_id));

    // Add type-specific components
    switch (payload.entity_type) {
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER): {
            // Convert entity_id to player_id (network-relative ID without offset)
            uint32_t playerId = EntityManager::toNetworkRelativeId(payload.entity_id);
            this->setupPlayerEntity(
                newEntity,
                playerId,
                static_cast<float>(payload.position_x),
                static_cast<float>(payload.position_y),
                static_cast<float>(payload.initial_velocity_x),
                static_cast<float>(payload.initial_velocity_y),
                payload.initial_health,
                payload.is_playable,
                /*withRenderComponents=*/true
            );
            break;
        }
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_ENEMY): {
            // Convert velocity from network format (int16_t representing velocity * 1000)
            float velX = static_cast<float>(static_cast<int16_t>(payload.initial_velocity_x)) / 1000.0f;
            float velY = static_cast<float>(static_cast<int16_t>(payload.initial_velocity_y)) / 1000.0f;
            
            // Decode enemy type from mob_variant (BASIC=0, FAST=1, TANK=2, BOSS=3)
            EnemyType enemyType = EnemyType::BASIC;  // default
            if (payload.mob_variant < 4) {
                enemyType = static_cast<EnemyType>(payload.mob_variant);
            }
            
            this->setupEnemyEntity(
                newEntity,
                payload.entity_id,
                static_cast<float>(payload.position_x),
                static_cast<float>(payload.position_y),
                velX,
                velY,
                payload.initial_health,
                enemyType,  // Use the enemy type from server
                /*withRenderComponents=*/true
            );
            LOG_INFO_CAT("Coordinator", "Enemy created with ID {} (type={}) at ({}, {}) with velocity ({}, {})", 
                        payload.entity_id, static_cast<int>(enemyType), static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), velX, velY);
            break;
        }
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_ENEMY_BOSS):
            // TODO: Initialize boss-specific components
            break;
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PROJECTILE_PLAYER):
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PROJECTILE_ENEMY): {
            const bool isPlayerProjectile =
                payload.entity_type == static_cast<uint8_t>(
                    protocol::EntityTypes::ENTITY_TYPE_PROJECTILE_PLAYER
                );

            // Note: shooterId is unknown from ENTITY_SPAWN - projectiles should be created via WEAPON_FIRE
            this->setupProjectileEntity(
                newEntity,
                Entity::fromId(0),  // Unknown shooter from ENTITY_SPAWN
                static_cast<float>(payload.position_x),
                static_cast<float>(payload.position_y),
                static_cast<float>(payload.initial_velocity_x),
                static_cast<float>(payload.initial_velocity_y),
                isPlayerProjectile,
                10,
                /*withRenderComponents=*/true
            );
            break;
        }
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_POWERUP):
            // TODO: Initialize powerup-specific components
            break;
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_OBSTACLE):
            // TODO: Initialize obstacle-specific components
            break;
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_BG_ELEMENT):
            // TODO: Initialize background element-specific components
            break;
        default:
            LOG_WARN_CAT("Coordinator", "Unknown entity type {}", payload.entity_type);
            break;
    }
}



void Coordinator::handlePacketDestroyEntity(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != ENTITY_DESTROY_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketDestroyEntity: invalid packet size {}, expected {}", packet.data.size(), ENTITY_DESTROY_PAYLOAD_SIZE);
        return;
    }

    // Parse the DESTROY_ENTITY payload in one memcpy
    protocol::EntityDestroyPayload payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "Entity destroyed: id={} reason={} final_pos=({}, {})",
        payload.entity_id, payload.destroy_reason, static_cast<float>(payload.final_position_x), static_cast<float>(payload.final_position_y));

    // Destroy the entity
    this->_engine->destroyEntity(payload.entity_id);
}


void Coordinator::handlePacketTransformSnapshot(const common::protocol::Packet& packet)
{
    constexpr std::size_t BASE_SIZE  = sizeof(uint16_t);                                            // 2 (entity_count)
    constexpr std::size_t ENTRY_SIZE = sizeof(uint32_t) + sizeof(protocol::ComponentTransform);     // 12

    const std::size_t size = packet.data.size();
    if (size < BASE_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketTransformSnapshot: payload too small ({}), expected >= {}",size, BASE_SIZE);
        return;
    }
    if ((size - BASE_SIZE) % ENTRY_SIZE != 0) {
        LOG_ERROR_CAT("Coordinator", "handlePacketTransformSnapshot: invalid payload size {} , expected {} + {}*N", size, BASE_SIZE, ENTRY_SIZE);
        return;
    }

    const std::uint8_t* const data =
        reinterpret_cast<const std::uint8_t*>(packet.data.data());

    // world_tick is in packet.header.timestamp
    uint32_t world_tick = packet.header.timestamp;
    uint16_t entity_count = 0;
    std::memcpy(&entity_count, data, sizeof(entity_count));

    const std::size_t computed_count = (size - BASE_SIZE) / ENTRY_SIZE;
    if (static_cast<std::size_t>(entity_count) != computed_count) {
        LOG_ERROR_CAT("Coordinator", "handlePacketTransformSnapshot: entity_count mismatch (packet={}, computed={})", entity_count, computed_count);
        return;
    }

    LOG_INFO_CAT("Coordinator", "[CLIENT] TransformSnapshot received: world_tick={} entity_count={}", world_tick, entity_count);

    std::size_t offset = BASE_SIZE;

    for (uint16_t i = 0; i < entity_count; ++i) {
        uint32_t entity_id = 0;
        protocol::ComponentTransform net{};

        std::memcpy(&entity_id, packet.data.data() + offset, sizeof(entity_id));
        offset += sizeof(entity_id);

        std::memcpy(&net, packet.data.data() + offset, sizeof(net));
        offset += sizeof(net);

        Entity entity = this->_engine->getEntityFromId(entity_id);
        
        // Check if entity exists before updating (it might not have been spawned yet via ENTITY_SPAWN)
        if (!this->_engine->isAlive(entity)) {
            LOG_WARN_CAT("Coordinator", "[CLIENT] Received transform for non-existent entity {}, skipping", entity_id);
            continue;
        }

        const Transform tf(
            static_cast<float>(net.pos_x),
            static_cast<float>(net.pos_y),
            (static_cast<float>(net.rotation) * 360.0f) / 65535.0f,
            static_cast<float>(net.scale) / 1000.0f
        );

        try {
            this->_engine->updateComponent<Transform>(entity, tf);
            LOG_DEBUG_CAT("Coordinator", "[CLIENT] Updated entity {} position to ({}, {})", entity_id, tf.x, tf.y);
        } catch (const std::exception& e) {
            LOG_WARN_CAT("Coordinator", "[CLIENT] Failed to update transform for entity {}: {}", entity_id, e.what());
            try {
                this->_engine->emplaceComponent<Transform>(entity, tf);
                LOG_DEBUG_CAT("Coordinator", "[CLIENT] Emplaced entity {} position to ({}, {})", entity_id, tf.x, tf.y);
            } catch (const std::exception& e2) {
                LOG_ERROR_CAT("Coordinator", "[CLIENT] Failed to emplace transform for entity {}: {}", entity_id, e2.what());
            }
        }
    }
}

void Coordinator::handlePacketHealthSnapshot(const common::protocol::Packet &packet)
{
    // Minimum size check: must have at least entity_count (2 bytes)
    if (packet.data.size() < 2) {
        LOG_ERROR_CAT("Coordinator", "handlePacketHealthSnapshot: packet too small, size={}", packet.data.size());
        return;
    }

    // Parse entity_count first
    uint16_t entity_count;
    std::memcpy(&entity_count, packet.data.data(), sizeof(entity_count));

    // Each health entry: entity_id(4) + current_health(1) + max_health(1) + current_shield(1) + max_shield(1) = 8 bytes
    const size_t HEALTH_ENTRY_SIZE = 8;
    const size_t expected_size = 2 + (entity_count * HEALTH_ENTRY_SIZE);
    
    if (packet.data.size() != expected_size) {
        LOG_ERROR_CAT("Coordinator", "handlePacketHealthSnapshot: invalid packet size {}, expected {} for {} entities",
            packet.data.size(), expected_size, entity_count);
        return;
    }

    LOG_INFO_CAT("Coordinator", "HealthSnapshot: entity_count={}", entity_count);

    size_t offset = 2; // Skip entity_count
    for (uint16_t i = 0; i < entity_count; i++) {
        uint32_t entity_id;
        uint8_t current_health, max_health, current_shield, max_shield;

        std::memcpy(&entity_id, packet.data.data() + offset, sizeof(entity_id));
        offset += sizeof(entity_id);

        std::memcpy(&current_health, packet.data.data() + offset, sizeof(current_health));
        offset += sizeof(current_health);

        std::memcpy(&max_health, packet.data.data() + offset, sizeof(max_health));
        offset += sizeof(max_health);

        std::memcpy(&current_shield, packet.data.data() + offset, sizeof(current_shield));
        offset += sizeof(current_shield);

        std::memcpy(&max_shield, packet.data.data() + offset, sizeof(max_shield));
        offset += sizeof(max_shield);

        LOG_DEBUG_CAT("Coordinator", "[CLIENT] Received health for entity {}: currentHealth={}, maxHp={}", 
                     entity_id, static_cast<int>(current_health), static_cast<int>(max_health));

        // Convert uint8_t network format to int component format
        Health health(static_cast<int>(current_health), static_cast<int>(max_health));
        // Note: shields are not currently in the Health component, so we only update health values

        Entity entity = this->_engine->getEntityFromId(entity_id);
        
        // Check if entity exists before updating
        if (!this->_engine->isAlive(entity)) {
            LOG_WARN_CAT("Coordinator", "[CLIENT] Received health for non-existent entity {}, skipping", entity_id);
            continue;
        }

        try {
            this->_engine->updateComponent<Health>(entity, health);
        } catch (const std::exception& e) {
            // if the entity does not have the component, set the component
            try {
                this->_engine->emplaceComponent<Health>(entity, health);
            } catch (const std::exception& e2) {
                LOG_ERROR_CAT("Coordinator", "[CLIENT] Failed to emplace health for entity {}: {}", entity_id, e2.what());
            }
        }
    }
}

void Coordinator::handlePacketWeaponSnapshot(const common::protocol::Packet &packet)
{
    // Minimum size check: must have at least entity_count (2 bytes)
    if (packet.data.size() < 2) {
        LOG_ERROR_CAT("Coordinator", "handlePacketWeaponSnapshot: packet too small, size={}", packet.data.size());
        return;
    }

    // Parse entity_count first
    uint16_t entity_count;
    std::memcpy(&entity_count, packet.data.data(), sizeof(entity_count));

    // Each weapon entry: entity_id(4) + fire_rate(2) + damage(1) + projectile_type(1) + ammo(1) = 9 bytes
    const size_t WEAPON_ENTRY_SIZE = 9;
    const size_t expected_size = 2 + (entity_count * WEAPON_ENTRY_SIZE);

    if (packet.data.size() != expected_size) {
        LOG_ERROR_CAT("Coordinator", "handlePacketWeaponSnapshot: invalid packet size {}, expected {} for {} entities",
            packet.data.size(), expected_size, entity_count);
        return;
    }

    LOG_INFO_CAT("Coordinator", "WeaponSnapshot: entity_count={}", entity_count);

    size_t offset = 2; // Skip entity_count
    for (uint16_t i = 0; i < entity_count; i++) {
        uint32_t entity_id;
        uint16_t fire_rate;
        uint8_t damage, projectile_type, ammo;

        std::memcpy(&entity_id, packet.data.data() + offset, sizeof(entity_id));
        offset += sizeof(entity_id);

        std::memcpy(&fire_rate, packet.data.data() + offset, sizeof(fire_rate));
        offset += sizeof(fire_rate);

        std::memcpy(&damage, packet.data.data() + offset, sizeof(damage));
        offset += sizeof(damage);

        std::memcpy(&projectile_type, packet.data.data() + offset, sizeof(projectile_type));
        offset += sizeof(projectile_type);

        std::memcpy(&ammo, packet.data.data() + offset, sizeof(ammo));
        offset += sizeof(ammo);

        // Convert network format to Weapon component format
        // Note: lastShotTime is not sent over network, keep existing value or set to 0
        Weapon weapon(
            static_cast<uint32_t>(fire_rate),  // fireRateMs
            0,                                   // lastShotTime (not synced)
            static_cast<int>(damage),           // damage
            static_cast<ProjectileType>(projectile_type)  // projectileType
        );
        // Note: ammo is not currently in the Weapon component

        Entity entity = this->_engine->getEntityFromId(entity_id);
        
        // Check if entity exists before updating
        if (!this->_engine->isAlive(entity)) {
            LOG_WARN_CAT("Coordinator", "[CLIENT] Received weapon for non-existent entity {}, skipping", entity_id);
            continue;
        }

        try {
            this->_engine->updateComponent<Weapon>(entity, weapon);
        } catch (const std::exception& e) {
            // if the entity does not have the component, set the component
            try {
                this->_engine->emplaceComponent<Weapon>(entity, weapon);
            } catch (const std::exception& e2) {
                LOG_ERROR_CAT("Coordinator", "[CLIENT] Failed to emplace weapon for entity {}: {}", entity_id, e2.what());
            }
        }
    }
}

void Coordinator::handlePacketAnimationSnapshot(const common::protocol::Packet &packet)
{
    // packet.data format:
    // - world_tick (4 bytes)
    // - entity_count (2 bytes)
    // - [entity_id (4 bytes) + ComponentAnimation (7 bytes)] * entity_count

    const auto& data = packet.data;

    // Minimum size: 6 bytes (world_tick + entity_count)
    if (data.size() < 6) {
        LOG_ERROR_CAT("Coordinator", "handlePacketAnimationSnapshot: packet too small ({} bytes)", data.size());
        return;
    }

    const uint8_t* ptr = data.data();

    uint32_t world_tick = 0; 
    std::memcpy(&world_tick, ptr, sizeof(world_tick));
    ptr += sizeof(world_tick);

    uint16_t entity_count = 0;
    std::memcpy(&entity_count, ptr, sizeof(entity_count));
    ptr += sizeof(entity_count);

    // Validate total size: 6 bytes base + (entity_count × 11 bytes)
    // Each entity has: entity_id(4) + ComponentAnimation(7) = 11 bytes
    if (data.size() != 6 + (entity_count * 11)) {
        LOG_ERROR_CAT("Coordinator", "handlePacketAnimationSnapshot: invalid packet size {}, expected {}", 
            data.size(), 6 + (entity_count * 11));
        return;
    }

    LOG_INFO_CAT("Coordinator", "AnimationSnapshot: world_tick={} entity_count={}", world_tick, entity_count);

    for (uint16_t i = 0; i < entity_count; ++i) {
        uint32_t entity_id = 0;
        protocol::ComponentAnimation netAnim;

        std::memcpy(&entity_id, ptr, sizeof(entity_id));
        ptr += sizeof(entity_id);

        std::memcpy(&netAnim, ptr, sizeof(netAnim));
        ptr += sizeof(netAnim);

        Entity entity = Entity::fromId(entity_id);
        if (!this->_engine->isAlive(entity))
            continue;

        auto& opt = this->_engine->getComponentEntity<Animation>(entity);
        if (!opt.has_value())
            continue;

        Animation& anim = opt.value();

        // Update animation state from network data
        // Note: ComponentAnimation uses animation_id, frame_index, frame_duration, loop_mode
        // We map these to the Animation component fields
        anim.currentFrame = netAnim.frame_index;
        anim.frameDuration = static_cast<float>(netAnim.frame_duration) / 1000.0f; // Convert ms to seconds
        anim.loop = (netAnim.loop_mode == 1); // 0=once, 1=loop, 2=pingpong

        // Reset elapsed time when receiving new animation state
        anim.elapsedTime = 0.0f;
    }
}

void Coordinator::handlePacketComponentRemove(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != COMPONENT_REMOVE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketComponentRemove: invalid packet size {} , expected {}", packet.data.size(), COMPONENT_REMOVE_PAYLOAD_SIZE);
        return;
    }

    // Parse the HEALTH_SNAPSHOT snapshot in one memcpy
    protocol::ComponentRemove payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "ComponentRemove: component_type={} entity_id={}",
        payload.component_type, payload.entity_id);

    Entity entity = this->_engine->getEntityFromId(payload.entity_id);


    this->_engine->removeComponentByType(payload.component_type, entity);
}

void Coordinator::handlePacketTransformSnapshotDelta(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketHealthSnapshotDelta(const common::protocol::Packet &packet)
{
    // Validate snapshot size using the protocol define
    if (packet.data.size() != HEALTH_SNAPSHOT_DELTA_BASE_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketHealthSnapshotDelta: invalid packet size {}, expected {}", packet.data.size(), HEALTH_SNAPSHOT_DELTA_BASE_SIZE);
        return;
    }

    // Parse the HEALTH_SNAPSHOT_DELTA snapshot in one memcpy
    protocol::HealthSnapshotDelta snapshot;
    std::memcpy(&snapshot, packet.data.data(), sizeof(snapshot));

    LOG_INFO_CAT("Coordinator", "HealthSnaphot: world_tick={} entity_count={}",
        snapshot.world_tick, snapshot.entity_count);

    size_t offset = sizeof(protocol::HealthSnapshotDelta);
    for (uint16_t i = 0; i < snapshot.entity_count; i++) {
        uint32_t entity_id;
        Health health(0, 0);

        std::memcpy(&entity_id, packet.data.data() + offset, sizeof(entity_id));
        offset += sizeof(entity_id);

        std::memcpy(&health, packet.data.data() + offset, sizeof(health));
        offset += sizeof(health);

        Entity entity = this->_engine->getEntityFromId(entity_id);

        try {
            this->_engine->updateComponent<Health>(entity, health);
        } catch (const std::exception& e) {
            // if the entity does not have the component, set the component
            this->_engine->emplaceComponent<Health>(entity, health);
        }
    }
}

void Coordinator::handlePacketPlayerHit(const common::protocol::Packet &packet)
{
     // Validate payload size using the protocol define
    if (packet.data.size() != PLAYER_HIT_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketPlayerHit: invalid packet size {}, expected {}", packet.data.size(), PLAYER_HIT_PAYLOAD_SIZE);
        return;
    }

    // Parse the PLAYER_HIT_PAYLOAD_SIZE payload in one memcpy
    protocol::PlayerHit payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "attacker_id: id={} damage={} hit_pos=({}, {}) player_id={} remaining_health={} remaining_shield={}",
        payload.attacker_id, payload.damage, static_cast<float>(payload.hit_pos_x), static_cast<float>(payload.hit_pos_y), payload.player_id,
        payload.remaining_health, payload.remaining_shield);


    // Hit player
    Entity playerHit = this->_engine->getEntityFromId(payload.player_id);

    try
    {
        auto& health = this->_engine->getComponentEntity<Health>(playerHit);
        health->currentHealth = payload.remaining_health;
        // TODO: handle the shied here, add a component and add the remaining_shield

        LOG_INFO_CAT("Coordinator", "Player {} health updated: hp={} shield={}",
            payload.player_id, payload.remaining_health, payload.remaining_shield);


        //TODO: add sound effects
        // this->_engine->playHitEffect(playerHit, payload.hit_pos_x, payload.hit_pos_y);
        // this->_engine->playHitSound(payload.damage);

    }
    catch(const Error &e)
    {
        LOG_ERROR_CAT("Coordinator", "Failed to update player {} health: {}",
            payload.player_id, e.what());
        std::cerr << "Error: " << e.what() << std::endl;
        if (e.getType() == ErrorType::CorruptedData) {
            std::cerr << "Problem update player " << payload.player_id << "health when hit" << std::endl;
        }
    }
}

void Coordinator::handlePacketPlayerDeath(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != PLAYER_DEATH_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketPlayerDeath: invalid packet size {}, expected {}", packet.data.size(), PLAYER_DEATH_PAYLOAD_SIZE);
        return;
    }

    // Parse the PLAYER_DEATH_PAYLOAD_SIZE payload in one memcpy
    protocol::PlayerDeath payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "killer_id: id={} death_pos=({}, {}) player_id={} score_before_death={}",
        payload.killer_id, static_cast<float>(payload.death_pos_x), static_cast<float>(payload.death_pos_y), payload.player_id,
        payload.score_before_death);

    // kill player
    Entity playerDead = this->_engine->getEntityFromId(payload.player_id);

    // add this component and the DeadPlayerSystem has to do the job (if all is fine), need to be test absolutely
    this->_engine->addComponent<DeadPlayer>(playerDead, DeadPlayer());
}

void Coordinator::handlePacketScoreUpdate(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != SCORE_UPDATE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketScoreUpdate: invalid packet size {}, expected {}", packet.data.size(), SCORE_UPDATE_PAYLOAD_SIZE);
        return;
    }

    // Parse the SCORE_UPDATE_PAYLOAD_SIZE payload in one memcpy
    protocol::ScoreUpdate payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "player_id={} new_score={} reason={} score_delta={}",
                payload.player_id, payload.new_score, payload.reason, payload.score_delta);

    // get the player
    Entity player = this->_engine->getEntityFromId(payload.player_id);


    // set the score to the player
    Score score;

    score.score = payload.new_score;

    try {
        this->_engine->updateComponent<Score>(player, score);
    } catch (const std::exception& e) {
        // if the player does not have the component, set the component
        this->_engine->emplaceComponent<Score>(player, score);
    }
}

void Coordinator::handlePacketPowerupPickup(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != POWER_PICKUP_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketPowerupPickup: invalid packet size {}, expected {}", packet.data.size(), POWER_PICKUP_PAYLOAD_SIZE);
        return;
    }

    // Parse the POWER_PICKUP_PAYLOAD_SIZE payload in one memcpy
    protocol::PowerupPickup payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "player_id={} powerup_id={} powerup_type={} duration={}",
                payload.player_id, payload.powerup_id, payload.powerup_type, payload.duration);

    // get the player
    Entity player = this->_engine->getEntityFromId(payload.player_id);

    Powerup powerup(PowerupType::UNKOWN, 0);

    powerup.duration = payload.duration;

    switch (powerup.powerupType) {
        case 0x00:
            powerup.powerupType = PowerupType::SPEED_BOOST;
            break;

        case 0x01:
            powerup.powerupType = PowerupType::WEAPON_UPGRADE;
            break;

        case 0x02:
            powerup.powerupType = PowerupType::FORCE;
            break;

        case 0x03:
            powerup.powerupType = PowerupType::SHIELD;
            break;

        case 0x04:
            powerup.powerupType = PowerupType::EXTRA_LIFE;
            break;

        case 0x05:
            powerup.powerupType = PowerupType::INVINCIBILITY;
            break;

        case 0x06:
            powerup.powerupType = PowerupType::HEAL;
            break;

        default:
            break;
    }

    try {
        this->_engine->updateComponent<Powerup>(player, powerup);
    } catch (const std::exception& e) {
        // if the player does not have the component, set the component
        this->_engine->emplaceComponent<Powerup>(player, powerup);
    }
}

void Coordinator::handlePacketWeaponFire(const common::protocol::Packet &packet)
{
    // Only clients process WEAPON_FIRE packets from server
    // Servers generate their own projectiles through queueWeaponFire()
    if (!_isServer) {
        // Parse the weapon fire packet
        auto parsed = PacketManager::parseWeaponFire(packet);
        if (!parsed.has_value()) {
            LOG_ERROR_CAT("Coordinator", "handlePacketWeaponFire: failed to parse weapon fire packet");
            return;
        }

        const auto& payload = parsed.value();

        LOG_INFO_CAT("Coordinator", "shooter_id={} projectile_id={} weapon_type={} origin_pos=({}, {}), direction_pos=({}, {})",
                    payload.shooterId, payload.projectileId, payload.weaponType, 
                    payload.originX, payload.originY, payload.directionX, payload.directionY);

        // Check if projectile already exists (duplicate packet or not yet cleaned up)
        Entity existingProjectile = this->_engine->getEntityFromId(payload.projectileId);
        if (this->_engine->isAlive(existingProjectile)) {
            LOG_DEBUG_CAT("Coordinator", "handlePacketWeaponFire: projectile {} already exists, skipping spawn", payload.projectileId);
            return;
        }

        // get the shooter
        Entity shooter = this->_engine->getEntityFromId(payload.shooterId);

        // Convert from int16_t to float
        float origin_x = static_cast<float>(payload.originX);
        float origin_y = static_cast<float>(payload.originY);

        // Convert direction from int16_t (normalized*1000) back to float
        float direction_x = static_cast<float>(payload.directionX) / 1000.0f;
        float direction_y = static_cast<float>(payload.directionY) / 1000.0f;

        try {
            LOG_DEBUG_CAT("Coordinator", "handlePacketWeaponFire: About to spawn projectile {}", payload.projectileId);
            Entity projectile = spawnProjectile(shooter, payload.projectileId, payload.weaponType, 
                                                origin_x, origin_y, direction_x, direction_y);
            LOG_DEBUG_CAT("Coordinator", "handlePacketWeaponFire: Successfully spawned projectile {}", payload.projectileId);
        } catch (const Error& e) {
            LOG_ERROR_CAT("Coordinator", "handlePacketWeaponFire: Failed to spawn projectile {}: {}", payload.projectileId, e.what());
            return;
        } catch (const std::exception& e) {
            LOG_ERROR_CAT("Coordinator", "handlePacketWeaponFire: Failed to spawn projectile {} with unexpected error: {}", payload.projectileId, e.what());
            return;
        }
    }
}

void Coordinator::handlePacketVisualEffect(const common::protocol::Packet &packet)
{
    if (packet.data.size() != VISUAL_EFFECT_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketVisualEffect: invalid packet size {}, expected {}", packet.data.size(), VISUAL_EFFECT_PAYLOAD_SIZE);
        return;
    }

    // Parse the VISUAL_EFFECT_PAYLOAD_SIZE payload in one memcpy
    protocol::VisualEffect payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "effect_type={} scale={} duration_ms={} color_tint_r={} color_tint_g={} color_tint_b={} pos=({}, {})",
                payload.effect_type, payload.scale, payload.duration_ms,
                payload.color_tint_r, payload.color_tint_g, payload.color_tint_b,
                payload.pos_x, payload.pos_y);

    // Convert network data to usable format
    float pos_x = static_cast<float>(payload.pos_x);
    float pos_y = static_cast<float>(payload.pos_y);
    float scale = static_cast<float>(payload.scale) / 100.0f; // 100 = 1.0x
    float duration = static_cast<float>(payload.duration_ms) / 1000.0f; // Convert to seconds

    // Convert color from 0-255 to 0.0-1.0 if needed for your graphics system
    float color_r = static_cast<float>(payload.color_tint_r) / 255.0f;
    float color_g = static_cast<float>(payload.color_tint_g) / 255.0f;
    float color_b = static_cast<float>(payload.color_tint_b) / 255.0f;

    // Spawn the visual effect based on effect_type
    this->spawnVisualEffect(
        static_cast<protocol::VisualEffectType>(payload.effect_type),
        pos_x, pos_y,
        scale,
        duration,
        color_r, color_g, color_b
    );

    LOG_INFO_CAT("Coordinator", "Visual effect {} spawned at ({}, {}) with scale {}x for {}s", 
                 payload.effect_type, pos_x, pos_y, scale, duration);

}

void Coordinator::handlePacketAudioEffect(const common::protocol::Packet &packet)
{
    if (packet.data.size() != AUDIO_EFFECT_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketAudioEffect: invalid packet size {}, expected {}", 
                      packet.data.size(), AUDIO_EFFECT_PAYLOAD_SIZE);
        return;
    }

    protocol::AudioEffect payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "audio_effect_type={} volume={} pitch={} pos=({}, {})",
                 payload.effect_type, payload.volume, payload.pitch,
                 payload.pos_x, payload.pos_y);

    // Convert network data
    float pos_x = static_cast<float>(payload.pos_x);
    float pos_y = static_cast<float>(payload.pos_y);
    float volume = static_cast<float>(payload.volume) / 255.0f; // 0-255 → 0.0-1.0
    float pitch = static_cast<float>(payload.pitch) / 100.0f;   // 100 = normal pitch

    // Play the audio effect with 3D positioning
    this->playAudioEffect(
        static_cast<protocol::AudioEffectType>(payload.effect_type),
        pos_x, pos_y,
        volume,
        pitch
    );

}

void Coordinator::handlePacketParticleSpawn(const common::protocol::Packet &packet)
{
    if (packet.data.size() != PARTICLE_SPAWN_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketParticleSpawn: invalid packet size {}, expected {}", 
                      packet.data.size(), PARTICLE_SPAWN_PAYLOAD_SIZE);
        return;
    }

    // Parse the PARTICLE_SPAWN payload in one memcpy
    protocol::ParticleSpawn payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "particle_system_id={} particle_count={} lifetime_ms={} pos=({}, {}) velocity=({}, {}) color_start=({}, {}, {}) color_end=({}, {}, {})",
                 payload.particle_system_id, payload.particle_count, payload.lifetime_ms,
                 static_cast<float>(payload.pos_x), static_cast<float>(payload.pos_y),
                 static_cast<float>(payload.velocity_x), static_cast<float>(payload.velocity_y),
                 payload.color_start_r, payload.color_start_g, payload.color_start_b,
                 payload.color_end_r, payload.color_end_g, payload.color_end_b);

    // Convert network data to usable format
    float pos_x = static_cast<float>(payload.pos_x);
    float pos_y = static_cast<float>(payload.pos_y);
    float vel_x = static_cast<float>(payload.velocity_x);
    float vel_y = static_cast<float>(payload.velocity_y);
    float lifetime = static_cast<float>(payload.lifetime_ms) / 1000.0f; // Convert to seconds

    // Convert colors from 0-255 to 0.0-1.0
    sf::Color colorStart(payload.color_start_r, payload.color_start_g, payload.color_start_b);
    sf::Color colorEnd(payload.color_end_r, payload.color_end_g, payload.color_end_b);

    // Spawn the particle system
    // this->_engine->spawnParticleSystem(
    //     static_cast<ParticleSystemType>(payload.particle_system_id),
    //     pos_x, pos_y,
    //     vel_x, vel_y,
    //     payload.particle_count,
    //     lifetime,
    //     colorStart,
    //     colorEnd
    // );

    LOG_INFO_CAT("Coordinator", "Particle system {} spawned with {} particles at ({}, {})", 
                 payload.particle_system_id, payload.particle_count, pos_x, pos_y);
}

void Coordinator::handlePacketPlayerIsReady(const common::protocol::Packet& packet)
{
    // Validate payload size
    if (packet.data.size() != PLAYER_READY_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketPlayerIsReady: invalid packet size {}, expected {}", 
                     packet.data.size(), PLAYER_READY_PAYLOAD_SIZE);
        return;
    }

    // Parse player_id
    uint32_t playerId;
    std::memcpy(&playerId, packet.data.data(), sizeof(uint32_t));

    LOG_INFO_CAT("Coordinator", "Player {} is READY", playerId);
    
    // Notify Game if callback is set
    if (_gameNotificationCallback) {
        _gameNotificationCallback(playerId, true);
    }
}

void Coordinator::handlePacketPlayerNotReady(const common::protocol::Packet& packet)
{
    // Validate payload size
    if (packet.data.size() != PLAYER_READY_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketPlayerNotReady: invalid packet size {}, expected {}", 
                     packet.data.size(), PLAYER_READY_PAYLOAD_SIZE);
        return;
    }

    // Parse player_id
    uint32_t playerId;
    std::memcpy(&playerId, packet.data.data(), sizeof(uint32_t));

    LOG_INFO_CAT("Coordinator", "Player {} is NOT READY", playerId);
    
    // Notify Game if callback is set
    if (_gameNotificationCallback) {
        _gameNotificationCallback(playerId, false);
    }
}

bool Coordinator::areAllPlayersReady(
    const std::vector<uint32_t>& connectedPlayers,
    uint32_t maxPlayers,
    const std::unordered_map<uint32_t, bool>& playerReadyStatus
) const
{
    // If no players are connected, not all players are ready
    if (connectedPlayers.empty()) {
        return false;
    }

    // Check if we have the right number of players
    if (connectedPlayers.size() < maxPlayers) {
        return false;
    }

    // Check if all connected players are ready
    for (uint32_t playerId : connectedPlayers) {
        auto it = playerReadyStatus.find(playerId);
        if (it == playerReadyStatus.end() || !it->second) {
            return false;
        }
    }

    return true;
}

void Coordinator::queuePlayerIsReady(uint32_t playerId)
{
    PlayerReadyEvent event;
    event.playerId = playerId;
    event.isReady = true;
    _pendingPlayerReadyEvents.push_back(event);
    LOG_INFO_CAT("Coordinator", "Queued PLAYER_IS_READY event for player {}", playerId);
}

void Coordinator::queuePlayerNotReady(uint32_t playerId)
{
    PlayerReadyEvent event;
    event.playerId = playerId;
    event.isReady = false;
    _pendingPlayerReadyEvents.push_back(event);
    LOG_INFO_CAT("Coordinator", "Queued PLAYER_NOT_READY event for player {}", playerId);
}

void Coordinator::handleGameStart(const common::protocol::Packet& packet)
{
    if (packet.data.size() != GAME_START_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "GameStart: invalid size {}", packet.data.size());
        return;
    }

    _gameRunning = true;
}

void Coordinator::handleGameEnd(const common::protocol::Packet& packet)
{
    if (packet.data.size() != GAME_END_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "GameEnd: invalid size {}", packet.data.size());
        return;
    }

    _gameRunning = false;
}

void Coordinator::handlePacketLevelComplete(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != LEVEL_COMPLETE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "Invalid LEVEL_COMPLETE payload size: expected {}, got {}",
            LEVEL_COMPLETE_PAYLOAD_SIZE, packet.data.size());
        return;
    }

    // Parse the LEVEL_COMPLETE payload in one memcpy
    const uint8_t* ptr = packet.data.data();

    uint8_t completed_level = 0;
    std::memcpy(&completed_level, ptr, sizeof(completed_level));
    ptr += sizeof(completed_level);

    uint8_t next_level = 0;
    std::memcpy(&next_level, ptr, sizeof(next_level));
    ptr += sizeof(next_level);

    uint32_t bonus_score = 0;
    std::memcpy(&bonus_score, ptr, sizeof(bonus_score));
    ptr += sizeof(bonus_score);

    uint16_t completion_time = 0;
    std::memcpy(&completion_time, ptr, sizeof(completion_time));

    LOG_INFO_CAT("Coordinator", "Level completed: level={} next={} bonus_score={} time={} seconds",
        completed_level, next_level, bonus_score, completion_time);

    // Display level completion message
    std::string completionMessage;
    if (next_level == 0xFF) {
        // Game is complete
        completionMessage = "GAME COMPLETE! Final Score Bonus: " + std::to_string(bonus_score);
        LOG_INFO_CAT("Coordinator", "Game completed! Final bonus: {}", bonus_score);
    } else {
        // Level complete, more levels ahead
        completionMessage = "LEVEL " + std::to_string(completed_level) + " COMPLETE! Bonus: " + std::to_string(bonus_score);
        LOG_INFO_CAT("Coordinator", "Level {} completed, preparing level {}", completed_level, next_level);
    }

    // Create a UI text entity to display the completion message
    Entity messageEntity = this->_engine->createEntity("LevelCompleteMessage");
    this->_engine->addComponent<Transform>(messageEntity, Transform(400.f, 300.f, 0.f, 1.5f));
    this->_engine->addComponent<Text>(messageEntity, Text(completionMessage.c_str()));
    this->_engine->addComponent<Sprite>(messageEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));

    // Update game state based on completion
    if (next_level == 0xFF) {
        // Game is finished - stop running state
        _gameRunning = false;
        stopMusic();  // Stop level music
        LOG_INFO_CAT("Coordinator", "Game ended - all levels completed, music stopped");

        // Display final game stats
        std::string timeMessage = "Completion Time: " + std::to_string(completion_time) + " seconds";
        Entity timeEntity = this->_engine->createEntity("CompletionTime");
        this->_engine->addComponent<Transform>(timeEntity, Transform(400.f, 350.f, 0.f, 1.0f));
        this->_engine->addComponent<Text>(timeEntity, Text(timeMessage.c_str()));
        this->_engine->addComponent<Sprite>(timeEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));
    } else {
        // More levels to play - keep game running
        LOG_INFO_CAT("Coordinator", "Waiting for server to start level {}", next_level);

        // Display "Next Level" message
        std::string nextLevelMessage = "Next Level: " + std::to_string(next_level);
        Entity nextEntity = this->_engine->createEntity("NextLevelMessage");
        this->_engine->addComponent<Transform>(nextEntity, Transform(400.f, 350.f, 0.f, 1.0f));
        this->_engine->addComponent<Text>(nextEntity, Text(nextLevelMessage.c_str()));
        this->_engine->addComponent<Sprite>(nextEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));
    }
}

void Coordinator::handlePacketLevelStart(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != LEVEL_START_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "Invalid LEVEL_START payload size: expected {}, got {}",
            LEVEL_START_PAYLOAD_SIZE, packet.data.size());
        return;
    }

    // Parse the LEVEL_START payload
    const uint8_t* ptr = packet.data.data();

    uint8_t level_id = 0;
    std::memcpy(&level_id, ptr, sizeof(level_id));
    ptr += sizeof(level_id);

    char level_name[32] = {0};
    std::memcpy(level_name, ptr, LEVEL_START_LEVEL_NAME_SIZE);
    ptr += LEVEL_START_LEVEL_NAME_SIZE;

    uint16_t estimated_duration = 0;
    std::memcpy(&estimated_duration, ptr, sizeof(estimated_duration));

    // Ensure level name is null-terminated
    level_name[31] = '\0';
    std::string levelNameStr(level_name);

    LOG_INFO_CAT("Coordinator", "Level started: id={} name=\"{}\"   estimated_duration={} seconds",
        level_id, levelNameStr.c_str(), estimated_duration);

    // Set game to running state
    _gameRunning = true;
    
    // Notify Game to clear the menu (client-side)
    if (_levelStartCallback) {
        _levelStartCallback();
    }

    // Create scrolling background for the level (client-side rendering)
    Entity backgroundEntity = this->_engine->createEntity("LevelBackground");
    this->_engine->addComponent<Transform>(backgroundEntity, Transform(0.f, 0.f, 0.f, 1.0f));
    // Use a background asset - for now using a placeholder, should be based on level
    // The actual asset would come from level data sent by server or configured per level
    this->_engine->addComponent<Sprite>(backgroundEntity, 
    Sprite(Assets::GAME_BG, ZIndex::IS_BACKGROUND, sf::IntRect(0, 0, GAME_BG_SPRITE_WIDTH, GAME_BG_SPRITE_HEIGHT)));
    this->_engine->addComponent<ScrollingBackground>(backgroundEntity, ScrollingBackground(LEVEL_BACKGROUND_SCROLL_SPEED, true, true));
    this->_engine->addComponent<Drawable>(backgroundEntity, Drawable());
    LOG_INFO_CAT("Coordinator", "Created scrolling background for level {}", level_id);

    // Start level music (client-side only)
    // The music type should ideally come from the level data sent by server
    // For now, using a default music type
    playMusic(protocol::AudioEffectType::FIRST_LEVEL_MUSIC);
    LOG_INFO_CAT("Coordinator", "Started level music");

    //// Create UI entity to display level start information
    //std::string startMessage = "LEVEL " + std::to_string(level_id) + ": " + levelNameStr;
    //Entity levelStartEntity = this->_engine->createEntity("LevelStartMessage");
    //this->_engine->addComponent<Transform>(levelStartEntity, Transform(640.f, 100.f, 0.f, 1.0f));
    //this->_engine->addComponent<Text>(levelStartEntity, Text(startMessage.c_str(), sf::Color::White, 48, ZIndex::IS_UI_HUD));
    //this->_engine->addComponent<Sprite>(levelStartEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));  // Dummy sprite for rendering system
    //this->_engine->addComponent<Drawable>(levelStartEntity, Drawable());

    // Display timer countdown if provided
    //if (estimated_duration > 0) {
    //    std::string timerMessage = "Time: " + std::to_string(static_cast<int>(estimated_duration)) + "s";
    //    Entity timerEntity = this->_engine->createEntity("LevelTimer");
    //    this->_engine->addComponent<Transform>(timerEntity, Transform(640.f, 50.f, 0.f, 1.0f));
    //    this->_engine->addComponent<Text>(timerEntity, Text(timerMessage.c_str(), sf::Color::Yellow, 36, ZIndex::IS_UI_HUD));
    //    this->_engine->addComponent<Sprite>(timerEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));  // Dummy sprite for rendering system
    //    this->_engine->addComponent<TimerUI>(timerEntity, TimerUI());
    //    this->_engine->addComponent<Drawable>(timerEntity, Drawable());
    //}

/*     // Create UI entity to display level start information
    std::string startMessage = "LEVEL " + std::to_string(level_id) + ": " + levelNameStr;
    Entity levelStartEntity = this->_engine->createEntity("LevelStartMessage");
    this->_engine->addComponent<Transform>(levelStartEntity, Transform(400.f, 200.f, 0.f, 2.0f));
    this->_engine->addComponent<Text>(levelStartEntity, Text(startMessage.c_str()));
    //this->_engine->addComponent<Sprite>(levelStartEntity, Sprite(DEFAULT_BULLET, ZIndex::IS_UI_HUD));

    // Display estimated duration if provided
    if (estimated_duration > 0) {
        std::string durationMessage = "Estimated Time: " + std::to_string(estimated_duration) + "s";
        Entity durationEntity = this->_engine->createEntity("LevelDuration");
        this->_engine->addComponent<Transform>(durationEntity, Transform(400.f, 250.f, 0.f, 1.0f));
        this->_engine->addComponent<Text>(durationEntity, Text(durationMessage.c_str()));
        //this->_engine->addComponent<Sprite>(durationEntity, Sprite(DEFAULT_BULLET, ZIndex::IS_UI_HUD));
    } */

    // Initialize level component if needed
    // Note: The server will spawn level entities and waves through TYPE_ENTITY_SPAWN packets
    LOG_INFO_CAT("Coordinator", "Level {} ({}) ready - waiting for entity spawns", 
        level_id, levelNameStr.c_str());
}

void Coordinator::handlePacketForceState(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != FORCE_STATE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "Invalid FORCE_STATE payload size: expected {}, got {}",
            FORCE_STATE_PAYLOAD_SIZE, packet.data.size());
        return;
    }

    // Parse the FORCE_STATE payload
    const uint8_t* ptr = packet.data.data();

    uint32_t force_entity_id = 0;
    std::memcpy(&force_entity_id, ptr, sizeof(force_entity_id));
    ptr += sizeof(force_entity_id);

    uint32_t parent_ship_id = 0;
    std::memcpy(&parent_ship_id, ptr, sizeof(parent_ship_id));
    ptr += sizeof(parent_ship_id);

    uint8_t attachment_point = 0;
    std::memcpy(&attachment_point, ptr, sizeof(attachment_point));
    ptr += sizeof(attachment_point);

    uint8_t power_level = 0;
    std::memcpy(&power_level, ptr, sizeof(power_level));
    ptr += sizeof(power_level);

    uint8_t charge_percentage = 0;
    std::memcpy(&charge_percentage, ptr, sizeof(charge_percentage));
    ptr += sizeof(charge_percentage);

    uint8_t is_firing = 0;
    std::memcpy(&is_firing, ptr, sizeof(is_firing));

    // Log force state update
    const char* attachment_str = "DETACHED";
    switch (attachment_point) {
        case 0x00: attachment_str = "DETACHED"; break;
        case 0x01: attachment_str = "FRONT"; break;
        case 0x02: attachment_str = "BACK"; break;
        case 0x03: attachment_str = "TOP"; break;
        case 0x04: attachment_str = "BOTTOM"; break;
        default: attachment_str = "UNKNOWN"; break;
    }

    LOG_INFO_CAT("Coordinator", "Force state: entity={} parent={} attach={} power={} charge={}%% firing={}",
        force_entity_id, parent_ship_id, attachment_str, power_level, charge_percentage,
        is_firing ? "YES" : "NO");

    // Get the Force entity
    Entity forceEntity = Entity::fromId(force_entity_id);
    if (!this->_engine->isAlive(forceEntity)) {
        LOG_WARN_CAT("Coordinator", "Force entity {} does not exist in engine", force_entity_id);
        return;
    }

    // Get or add Force component
    auto& forceComponent = this->_engine->getComponentEntity<Force>(forceEntity);
    if (!forceComponent.has_value()) {
        // Force component doesn't exist, create it
        this->_engine->addComponent<Force>(forceEntity, 
            Force(parent_ship_id, static_cast<ForceAttachmentPoint>(attachment_point), 
                  power_level, charge_percentage, is_firing != 0));
        LOG_DEBUG_CAT("Coordinator", "Created Force component for entity {}", force_entity_id);
    } else {
        // Update existing Force component
        forceComponent->parentShipId = parent_ship_id;
        forceComponent->attachmentPoint = static_cast<ForceAttachmentPoint>(attachment_point);
        forceComponent->powerLevel = power_level;
        forceComponent->chargePercentage = charge_percentage;
        forceComponent->isFiring = (is_firing != 0);
    }

    // Update Force position based on attachment
    if (parent_ship_id == 0) {
        // Force is detached - it should move independently
        LOG_DEBUG_CAT("Coordinator", "Force {} is detached and moving independently", force_entity_id);
    } else {
        // Force is attached to a parent ship - update its position relative to parent
        Entity parentEntity = Entity::fromId(parent_ship_id);
        if (this->_engine->isAlive(parentEntity)) {
            auto& parentTransform = this->_engine->getComponentEntity<Transform>(parentEntity);
            auto& forceTransform = this->_engine->getComponentEntity<Transform>(forceEntity);

            if (parentTransform.has_value() && forceTransform.has_value()) {
                // Adjust Force position based on attachment point
                float offsetX = 0.f;
                float offsetY = 0.f;

                switch (attachment_point) {
                    case 0x01: // FRONT
                        offsetX = 30.f;
                        offsetY = 0.f;
                        break;
                    case 0x02: // BACK
                        offsetX = -30.f;
                        offsetY = 0.f;
                        break;
                    case 0x03: // TOP
                        offsetX = 0.f;
                        offsetY = -20.f;
                        break;
                    case 0x04: // BOTTOM
                        offsetX = 0.f;

                        offsetY = 20.f;
                        break;
                    default:

                    break;
                }

                forceTransform->x = parentTransform->x + offsetX;
                forceTransform->y = parentTransform->y + offsetY;

                LOG_DEBUG_CAT("Coordinator", "Force {} attached to parent {} at ({:.1f}, {:.1f})",
                    force_entity_id, parent_ship_id, forceTransform->x, forceTransform->y);
            }
        } else {
            LOG_WARN_CAT("Coordinator", "Parent ship {} does not exist for Force {}", 
                parent_ship_id, force_entity_id);
        }
    }
}

void Coordinator::handlePacketAIState(const common::protocol::Packet &packet)
{
    // Validate payload size
    if (packet.data.size() != AI_STATE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "AIState: invalid packet size {}, expected {}",
            packet.data.size(), AI_STATE_PAYLOAD_SIZE);
        return;
    }

    // Parse the AI_STATE payload
    const uint8_t* ptr = packet.data.data();

    uint32_t entity_id = 0;
    std::memcpy(&entity_id, ptr, sizeof(entity_id));
    ptr += sizeof(entity_id);

    uint8_t current_state = 0;
    std::memcpy(&current_state, ptr, sizeof(current_state));
    ptr += sizeof(current_state);

    uint8_t behavior_type = 0;
    std::memcpy(&behavior_type, ptr, sizeof(behavior_type));
    ptr += sizeof(behavior_type);

    uint32_t target_entity_id = 0;
    std::memcpy(&target_entity_id, ptr, sizeof(target_entity_id));
    ptr += sizeof(target_entity_id);

    int16_t waypoint_x = 0;
    std::memcpy(&waypoint_x, ptr, sizeof(waypoint_x));
    ptr += sizeof(waypoint_x);

    int16_t waypoint_y = 0;
    std::memcpy(&waypoint_y, ptr, sizeof(waypoint_y));
    ptr += sizeof(waypoint_y);

    uint16_t state_timer = 0;
    std::memcpy(&state_timer, ptr, sizeof(state_timer));
    ptr += sizeof(state_timer);

    // Get the entity
    Entity entity = Entity::fromId(entity_id);
    if (!this->_engine->isAlive(entity)) {
        LOG_WARN_CAT("Coordinator", "AIState: entity {} is not alive", entity_id);
        return;
    }

    // Get the AI component
    auto& optAI = this->_engine->getComponentEntity<AI>(entity);
    if (!optAI.has_value()) {
        LOG_WARN_CAT("Coordinator", "AIState: entity {} has no AI component", entity_id);
        return;
    }

    // Update the AI component with new state information
    AI& ai = optAI.value();
    ai.internalTime = static_cast<float>(state_timer);

    LOG_DEBUG_CAT("Coordinator", "AIState updated: entity={} state={} behavior={} target={} waypoint=({}, {}) timer={}",
        entity_id, current_state, behavior_type, target_entity_id,
        static_cast<float>(waypoint_x), static_cast<float>(waypoint_y), state_timer);
}

// ==============================================================
//                  CREATE PACKET METHODS
// ==============================================================

bool Coordinator::createPacketEntitySpawn(common::protocol::Packet* packet, uint32_t entityId, uint32_t sequence_number)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: null packet pointer");
        return false;
    }

    Entity entity = Entity::fromId(entityId);
    if (!this->_engine->isAlive(entity)) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: entity {} is not alive", entityId);
        return false;
    }

    // Retrieve components from the engine
    auto transformOpt = this->_engine->getComponentEntity<Transform>(entity);
    auto velocityOpt = this->_engine->getComponentEntity<Velocity>(entity);
    auto healthOpt = this->_engine->getComponentEntity<Health>(entity);
    auto networkIdOpt = this->_engine->getComponentEntity<NetworkId>(entity);

    if (!transformOpt.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: entity {} has no Transform component", entityId);
        return false;
    }

    Transform& transform = transformOpt.value();

    // Build args vector for PacketManager
    std::vector<uint8_t> args;

    // flags_count (1 for FLAG_RELIABLE)
    uint8_t flags_count = 1;
    args.push_back(flags_count);
    
    // flags (FLAG_RELIABLE = 0x01)
    uint8_t flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    args.push_back(flags);

    // sequence_number
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&sequence_number), 
                reinterpret_cast<uint8_t*>(&sequence_number) + sizeof(sequence_number));

                // timestamp
    uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), 
                reinterpret_cast<uint8_t*>(&timestamp) + sizeof(timestamp));

                // entity_id
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&entityId), 
                reinterpret_cast<uint8_t*>(&entityId) + sizeof(entityId));

                // entity_type (default to ENEMY if no specific type found)
    uint8_t entity_type = static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_ENEMY);
    args.push_back(entity_type);

    // position_x, position_y
    uint16_t position_x = static_cast<uint16_t>(transform.x);
    uint16_t position_y = static_cast<uint16_t>(transform.y);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&position_x), 
                reinterpret_cast<uint8_t*>(&position_x) + sizeof(position_x));
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&position_y), 
                reinterpret_cast<uint8_t*>(&position_y) + sizeof(position_y));

                // mob_variant: encode the enemy type if this is an enemy entity
    uint8_t mob_variant = 0;  // default for non-enemy entities
    auto enemyOpt = this->_engine->getComponentEntity<Enemy>(entity);
    if (enemyOpt.has_value()) {
        // For enemy entities, encode the type (BASIC=0, FAST=1, TANK=2, BOSS=3)
        mob_variant = static_cast<uint8_t>(enemyOpt.value().type);
    }
    args.push_back(mob_variant);

    // initial_health
    uint8_t initial_health = healthOpt.has_value() ? static_cast<uint8_t>(healthOpt.value().currentHealth) : 100;
    args.push_back(initial_health);

    // initial_velocity_x, initial_velocity_y (multiply by 1000 for network encoding)
    int16_t velocity_x = velocityOpt.has_value() ? static_cast<int16_t>(velocityOpt.value().vx * 1000.0f) : 0;
    int16_t velocity_y = velocityOpt.has_value() ? static_cast<int16_t>(velocityOpt.value().vy * 1000.0f) : 0;
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&velocity_x), 
                reinterpret_cast<uint8_t*>(&velocity_x) + sizeof(velocity_x));
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&velocity_y), 
                reinterpret_cast<uint8_t*>(&velocity_y) + sizeof(velocity_y));

                // is_playable (check for InputComponent)
    uint8_t is_playable = this->_engine->getComponentEntity<InputComponent>(entity).has_value() ? 1 : 0;
    args.push_back(is_playable);

    // Create the packet using PacketManager
    auto result = PacketManager::createEntitySpawn(args);
    if (!result.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: PacketManager failed to create packet");
        return false;
    }

    // Assert the packet
    if (!PacketManager::assertEntitySpawn(result.value())) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: packet assertion failed");
        return false;
    }

    // Copy to output packet
    *packet = result.value();

    LOG_DEBUG_CAT("Coordinator", "createPacketEntitySpawn: created packet for entity {}", entityId);
    return true;
}

bool Coordinator::createPacketTransformSnapshot(common::protocol::Packet* packet, const std::vector<uint32_t>& entityIds, uint32_t sequence_number)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketTransformSnapshot: null packet pointer");
        return false;
    }

    // Build args vector
    std::vector<uint8_t> args;

    // flags_count (0 for now)
    uint8_t flags_count = 0;
    args.push_back(flags_count);

    // sequence_numbe
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&sequence_number), 
                reinterpret_cast<uint8_t*>(&sequence_number) + sizeof(sequence_number));

                // timestamp
    uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), 
                reinterpret_cast<uint8_t*>(&timestamp) + sizeof(timestamp));

                // entity_count
    uint16_t entity_count = static_cast<uint16_t>(entityIds.size());
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&entity_count), 
                reinterpret_cast<uint8_t*>(&entity_count) + sizeof(entity_count));

                // For each entity, add transform data
    for (uint32_t entityId : entityIds) {
        Entity entity = Entity::fromId(entityId);
        if (!this->_engine->isAlive(entity)) {
            LOG_WARN_CAT("Coordinator", "createPacketTransformSnapshot: skipping dead entity {}", entityId);
            continue;
        }

        auto transformOpt = this->_engine->getComponentEntity<Transform>(entity);
        if (!transformOpt.has_value()) {
            LOG_WARN_CAT("Coordinator", "createPacketTransformSnapshot: entity {} has no Transform", entityId);
            continue;
        }

        Transform& transform = transformOpt.value();

        // entity_id (4 bytes)
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&entityId), 
                    reinterpret_cast<uint8_t*>(&entityId) + sizeof(entityId));

                    // position_x (2 bytes)
        uint16_t position_x = static_cast<uint16_t>(transform.x);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&position_x), 
                    reinterpret_cast<uint8_t*>(&position_x) + sizeof(position_x));

                    // position_y (2 bytes)
        uint16_t position_y = static_cast<uint16_t>(transform.y);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&position_y), 
                    reinterpret_cast<uint8_t*>(&position_y) + sizeof(position_y));

                    // rotation (2 bytes, scaled to uint16_t)
        uint16_t rotation = static_cast<uint16_t>(transform.rotation * 100.0f);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&rotation), 
                    reinterpret_cast<uint8_t*>(&rotation) + sizeof(rotation));

                    // scale (2 bytes, scaled to uint16_t)
        uint16_t scale = static_cast<uint16_t>(transform.scale * 1000.0f);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&scale), 
                    reinterpret_cast<uint8_t*>(&scale) + sizeof(scale));
    }

    // Create the packet
    auto result = PacketManager::createTransformSnapshot(args);
    if (!result.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketTransformSnapshot: PacketManager failed");
        return false;
    }

    // Assert the packet
    if (!PacketManager::assertTransformSnapshot(result.value())) {
        LOG_ERROR_CAT("Coordinator", "createPacketTransformSnapshot: packet assertion failed");
        return false;
    }

    *packet = result.value();
    LOG_DEBUG_CAT("Coordinator", "createPacketTransformSnapshot: created packet for {} entities", entityIds.size());
    return true;
}

bool Coordinator::createPacketHealthSnapshot(common::protocol::Packet* packet, const std::vector<uint32_t>& entityIds, uint32_t sequence_number)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketHealthSnapshot: null packet pointer");
        return false;
    }

    std::vector<uint8_t> args;

    // flags_count
    uint8_t flags_count = 0;
    args.push_back(flags_count);

    // sequence_number
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&sequence_number), 
                reinterpret_cast<uint8_t*>(&sequence_number) + sizeof(sequence_number));

                // timestamp
    uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), 
                reinterpret_cast<uint8_t*>(&timestamp) + sizeof(timestamp));

                // entity_count
    uint16_t entity_count = static_cast<uint16_t>(entityIds.size());
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&entity_count), 
                reinterpret_cast<uint8_t*>(&entity_count) + sizeof(entity_count));

                // For each entity, add health data
    for (uint32_t entityId : entityIds) {
        Entity entity = Entity::fromId(entityId);
        if (!this->_engine->isAlive(entity)) {
            continue;
        }

        auto healthOpt = this->_engine->getComponentEntity<Health>(entity);
        if (!healthOpt.has_value()) {
            continue;
        }

        Health& health = healthOpt.value();
        
        LOG_DEBUG_CAT("Coordinator", "[SERVER] Entity {} Health component BEFORE conversion: currentHealth={}, maxHp={}", 
                     entityId, health.currentHealth, health.maxHp);
        
        // Skip entities with invalid health values
        if (health.maxHp <= 0) {
            LOG_DEBUG_CAT("Coordinator", "Skipping entity {} with invalid maxHp: {}", entityId, health.maxHp);
            continue;
        }

        // entity_id (4 bytes)
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&entityId), 
                    reinterpret_cast<uint8_t*>(&entityId) + sizeof(entityId));

        // ComponentHealth (4 bytes): current_health(1) + max_health(1) + current_shield(1) + max_shield(1)
        // current_health (1 byte)
        uint8_t current_health = static_cast<uint8_t>(std::min(255, std::max(0, static_cast<int>(health.currentHealth))));
        args.push_back(current_health);

        // max_health (1 byte) - must be > 0
        uint8_t max_health = static_cast<uint8_t>(std::min(255, std::max(1, static_cast<int>(health.maxHp))));
        args.push_back(max_health);
        
        LOG_DEBUG_CAT("Coordinator", "[SERVER] Sending health for entity {}: currentHealth={}, maxHp={}", 
                     entityId, static_cast<int>(current_health), static_cast<int>(max_health));

        // current_shield (1 byte) - 0 for now
        args.push_back(0);

        // max_shield (1 byte) - 0 for now
        args.push_back(0);
    }

    auto result = PacketManager::createHealthSnapshot(args);
    if (!result.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketHealthSnapshot: PacketManager failed");
        return false;
    }

    if (!PacketManager::assertHealthSnapshot(result.value())) {
        LOG_ERROR_CAT("Coordinator", "createPacketHealthSnapshot: packet assertion failed");
        return false;
    }

    *packet = result.value();
    LOG_DEBUG_CAT("Coordinator", "createPacketHealthSnapshot: created packet for {} entities", entityIds.size());
    return true;
}

bool Coordinator::createPacketWeaponSnapshot(common::protocol::Packet* packet, const std::vector<uint32_t>& entityIds, uint32_t sequence_number)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketWeaponSnapshot: null packet pointer");
        return false;
    }

    std::vector<uint8_t> args;

    // flags_count
    uint8_t flags_count = 0;
    args.push_back(flags_count);

    // sequence_number
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&sequence_number), 
                reinterpret_cast<uint8_t*>(&sequence_number) + sizeof(sequence_number));

                // timestamp
    uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), 
                reinterpret_cast<uint8_t*>(&timestamp) + sizeof(timestamp));

                // entity_count
    uint16_t entity_count = static_cast<uint16_t>(entityIds.size());
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&entity_count), 
                reinterpret_cast<uint8_t*>(&entity_count) + sizeof(entity_count));

                // For each entity, add weapon data
    for (uint32_t entityId : entityIds) {
        Entity entity = Entity::fromId(entityId);
        if (!this->_engine->isAlive(entity)) {
            continue;
        }

        auto weaponOpt = this->_engine->getComponentEntity<Weapon>(entity);
        if (!weaponOpt.has_value()) {
            continue;
        }

        Weapon& weapon = weaponOpt.value();

        // entity_id (4 bytes)
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&entityId), 
                    reinterpret_cast<uint8_t*>(&entityId) + sizeof(entityId));

        // ComponentWeapon (5 bytes): fire_rate(2) + damage(1) + projectile_type(1) + ammo(1)
        // fire_rate (2 bytes)
        uint16_t fire_rate = static_cast<uint16_t>(weapon.fireRateMs);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&fire_rate), 
                    reinterpret_cast<uint8_t*>(&fire_rate) + sizeof(fire_rate));

        // damage (1 byte)
        uint8_t damage = static_cast<uint8_t>(std::min(255, static_cast<int>(weapon.damage)));
        args.push_back(damage);

        // projectile_type (1 byte)
        uint8_t projectile_type = static_cast<uint8_t>(weapon.projectileType);
        args.push_back(projectile_type);

        // ammo (1 byte) - 255 for infinite
        args.push_back(255);
    }

    auto result = PacketManager::createWeaponSnapshot(args);
    if (!result.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketWeaponSnapshot: PacketManager failed");
        return false;
    }

    if (!PacketManager::assertWeaponSnapshot(result.value())) {
        LOG_ERROR_CAT("Coordinator", "createPacketWeaponSnapshot: packet assertion failed");
        return false;
    }

    *packet = result.value();
    LOG_DEBUG_CAT("Coordinator", "createPacketWeaponSnapshot: created packet for {} entities", entityIds.size());
    return true;
}

bool Coordinator::createPacketEntityDestroy(common::protocol::Packet* packet, uint32_t entityId, uint8_t reason, uint32_t sequence_number)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntityDestroy: null packet pointer");
        return false;
    }

    Entity entity = Entity::fromId(entityId);

    // Get final position if entity still exists
    uint16_t final_x = 0;
    uint16_t final_y = 0;

    if (this->_engine->isAlive(entity)) {
        auto transformOpt = this->_engine->getComponentEntity<Transform>(entity);
        if (transformOpt.has_value()) {
            final_x = static_cast<uint16_t>(transformOpt.value().x);
            final_y = static_cast<uint16_t>(transformOpt.value().y);
        }
    }

    std::vector<uint8_t> args;

    // flags_count
    uint8_t flags_count = 0;
    args.push_back(flags_count);

    // sequence_number
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&sequence_number), 
                reinterpret_cast<uint8_t*>(&sequence_number) + sizeof(sequence_number));

                // timestamp
    uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP);
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&timestamp), 
                reinterpret_cast<uint8_t*>(&timestamp) + sizeof(timestamp));

                // entity_id
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&entityId), 
                reinterpret_cast<uint8_t*>(&entityId) + sizeof(entityId));

                // destroy_reason
    args.push_back(reason);

    // final_position_x
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&final_x), 
                reinterpret_cast<uint8_t*>(&final_x) + sizeof(final_x));

                // final_position_y
    args.insert(args.end(), reinterpret_cast<uint8_t*>(&final_y), 
                reinterpret_cast<uint8_t*>(&final_y) + sizeof(final_y));

    auto result = PacketManager::createEntityDestroy(args);
    if (!result.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntityDestroy: PacketManager failed");
        return false;
    }

    if (!PacketManager::assertEntityDestroy(result.value())) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntityDestroy: packet assertion failed");
        return false;
    }

    *packet = result.value();
    LOG_DEBUG_CAT("Coordinator", "createPacketEntityDestroy: created packet for entity {}", entityId);
    return true;
}


std::shared_ptr<gameEngine::GameEngine> Coordinator::getEngine() const
{
    return this->_engine;
}

Entity Coordinator::spawnProjectile(Entity shooter, uint32_t projectile_id, uint8_t weapon_type, float origin_x, float origin_y, float dir_x, float dir_y)
{
    LOG_DEBUG_CAT("Coordinator", "spawnProjectile: START - projectile_id={} weapon_type={}", projectile_id, weapon_type);
    
    bool isFromPlayable = false;
    std::string projectileName = "projectile_" + std::to_string(projectile_id);
    
    // Get damage from shooter's weapon if available, otherwise use default
    uint16_t projectileDamage = 10; // Default damage
    
    // Only access shooter components if entity exists, is alive, AND has all required components
    // Use try-catch to handle any unexpected component access failures
    try {
        if (this->_engine->isAlive(shooter) && 
            this->_engine->hasComponent<Transform>(shooter) &&
            this->_engine->hasComponent<Weapon>(shooter)) {
            
            if (this->_engine->hasComponent<InputComponent>(shooter)) {
                isFromPlayable = true;
            }
            
            auto& shooterWeapon = this->_engine->getComponentEntity<Weapon>(shooter);
            projectileDamage = shooterWeapon->damage;
        }
    } catch (...) {
        // If any component access fails, just use default values
        LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Failed to access shooter components, using defaults");
    }

    LOG_DEBUG_CAT("Coordinator", "spawnProjectile: About to createEntityWithId {}", projectile_id);
    Entity projectile = this->_engine->createEntityWithId(projectile_id, projectileName);
    LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Entity created successfully");
    
    float projectileSpeed = BULLET_SPEED;  // tuned for visible travel with dt in ms

    switch (weapon_type) {
        case 0x00: // WEAPON_TYPE_BASIC
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding Transform component");
            this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, DEFAULT_BULLET_ROTATION, DEFAULT_BULLET_SCALE));
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding Velocity component");
            this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding Projectile component");
            this->_engine->addComponent<Projectile>(projectile, Projectile(shooter, isFromPlayable, projectileDamage));
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding Sprite component");
            this->_engine->addComponent<Sprite>(projectile, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_GAME,
                sf::IntRect(0, 0, DEFAULT_BULLET_SPRITE_WIDTH, DEFAULT_BULLET_SPRITE_HEIGHT)));
            this->_engine->addComponent<Animation>(projectile, Animation(DEFAULT_BULLET_ANIMATION_WIDTH,
                DEFAULT_BULLET_ANIMATION_HEIGHT, DEFAULT_BULLET_ANIMATION_CURRENT, DEFAULT_BULLET_ANIMATION_ELAPSED_TIME, DEFAULT_BULLET_ANIMATION_DURATION,
                DEFAULT_BULLET_ANIMATION_START, DEFAULT_BULLET_ANIMATION_END, DEFAULT_BULLET_ANIMATION_LOOPING));
                sf::IntRect(0, 0, DEFAULT_BULLET_SPRITE_WIDTH, DEFAULT_BULLET_SPRITE_HEIGHT);
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding Animation component");
            this->_engine->addComponent<Animation>(projectile, Animation(DEFAULT_BULLET_ANIMATION_WIDTH,
                DEFAULT_BULLET_ANIMATION_HEIGHT, DEFAULT_BULLET_ANIMATION_CURRENT, DEFAULT_BULLET_ANIMATION_ELAPSED_TIME, DEFAULT_BULLET_ANIMATION_DURATION,
                DEFAULT_BULLET_ANIMATION_START, DEFAULT_BULLET_ANIMATION_END, DEFAULT_BULLET_ANIMATION_LOOPING));
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding HitBox component");
            this->_engine->addComponent<HitBox>(projectile, HitBox());
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding Team component");
            this->_engine->addComponent<Team>(projectile, isFromPlayable ? TeamType::PLAYER : TeamType::ENEMY);
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: Adding AudioSource component for basic projectile");
            this->_engine->addComponent<AudioSource>(projectile, AudioSource(AudioAssets::SFX_SHOOT_BASIC, AUDIO_BASIC_PROJECTILE_LOOP, AUDIO_BASIC_PROJECTILE_MIN_DISTANCE, AUDIO_BASIC_PROJECTILE_ATTENUATION, false, AUDIO_SHOOT_BASIC_DURATION));
            LOG_DEBUG_CAT("Coordinator", "spawnProjectile: All components added successfully");
            break;

        case 0x01: // WEAPON_TYPE_CHARGED
            this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, CHARGED_BULLET_ROTATION, CHARGED_BULLET_SCALE));
            this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
            this->_engine->addComponent<Projectile>(projectile, Projectile(shooter, isFromPlayable, projectileDamage));
            this->_engine->addComponent<Sprite>(projectile, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_GAME,
                sf::IntRect(0, 0, CHARGED_BULLET_SPRITE_WIDTH, CHARGED_BULLET_SPRITE_HEIGHT)));
            this->_engine->addComponent<Animation>(projectile, Animation(CHARGED_BULLET_ANIMATION_WIDTH,
                CHARGED_BULLET_ANIMATION_HEIGHT, CHARGED_BULLET_ANIMATION_CURRENT, CHARGED_BULLET_ANIMATION_ELAPSED_TIME, CHARGED_BULLET_ANIMATION_DURATION,
                CHARGED_BULLET_ANIMATION_START, CHARGED_BULLET_ANIMATION_END, CHARGED_BULLET_ANIMATION_LOOPING));
            this->_engine->addComponent<HitBox>(projectile, HitBox());
            this->_engine->addComponent<Team>(projectile, isFromPlayable ? TeamType::PLAYER : TeamType::ENEMY);
            this->_engine->addComponent<AudioSource>(projectile, AudioSource(AudioAssets::SFX_SHOOT_CHARGED, AUDIO_CHARGED_PROJECTILE_LOOP, AUDIO_CHARGED_PROJECTILE_MIN_DISTANCE, AUDIO_CHARGED_PROJECTILE_ATTENUATION, false, AUDIO_SHOOT_CHARGED_DURATION));
            break;

        // case 0x02: // WEAPON_TYPE_SPREAD
        //     this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, 0, ));
        //     this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
        //     break;

        // case 0x03: // WEAPON_TYPE_LASER
        //     this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, 0, ));
        //     this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
        //     break;

        // case 0x04: // WEAPON_TYPE_MISSILE
        //     this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, 0, ));
        //     this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
        //     break;

        // case 0x05: // WEAPON_TYPE_FORCE_SHOT
        //     this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, 0, ));
        //     this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
        //     break;

        default:
            break;
    }


    return projectile;
}


void Coordinator::spawnVisualEffect(protocol::VisualEffectType type, float x, float y,
    float scale, float duration, float color_r, float color_g, float color_b)
{
    Entity visualEffectEntity = this->_engine->createEntity("VisualEffect");

    this->_engine->addComponent<Transform>(visualEffectEntity, Transform(x, y, 0, scale));
    this->_engine->addComponent<VisualEffect>(visualEffectEntity,
        VisualEffect(type, scale, duration, color_r, color_g, color_b));

    // set the sprites and animations
    switch(type) {
        case protocol::VisualEffectType::VFX_EXPLOSION_SMALL:
            this->_engine->addComponent<Sprite>(visualEffectEntity,
                Sprite(Assets::SMALL_EXPLOSION, ZIndex::IS_GAME,
                sf::IntRect(0, 0, SMALL_EXPLOSION_SPRITE_WIDTH, SMALL_EXPLOSION_SPRITE_HEIGHT)));

            this->_engine->addComponent<Animation>(visualEffectEntity,
                Animation(SMALL_EXPLOSION_ANIMATION_WIDTH, SMALL_EXPLOSION_ANIMATION_HEIGHT,
                    SMALL_EXPLOSION_ANIMATION_CURRENT, SMALL_EXPLOSION_ANIMATION_ELAPSED_TIME, SMALL_EXPLOSION_ANIMATION_DURATION,
                SMALL_EXPLOSION_ANIMATION_START, SMALL_EXPLOSION_ANIMATION_END, SMALL_EXPLOSION_ANIMATION_LOOPING));
            break;

        case protocol::VisualEffectType::VFX_EXPLOSION_MEDIUM:
            this->_engine->addComponent<Sprite>(visualEffectEntity,
                Sprite(Assets::MEDIUM_EXPLOSION, ZIndex::IS_GAME,
                sf::IntRect(0, 0, MEDIUM_EXPLOSION_SPRITE_WIDTH, MEDIUM_EXPLOSION_SPRITE_HEIGHT)));

            this->_engine->addComponent<Animation>(visualEffectEntity,
                Animation(MEDIUM_EXPLOSION_ANIMATION_WIDTH, MEDIUM_EXPLOSION_ANIMATION_HEIGHT,
                    MEDIUM_EXPLOSION_ANIMATION_CURRENT, MEDIUM_EXPLOSION_ANIMATION_ELAPSED_TIME, MEDIUM_EXPLOSION_ANIMATION_DURATION,
                MEDIUM_EXPLOSION_ANIMATION_START, MEDIUM_EXPLOSION_ANIMATION_END, MEDIUM_EXPLOSION_ANIMATION_LOOPING));
            break;

        case protocol::VisualEffectType::VFX_EXPLOSION_LARGE:
            this->_engine->addComponent<Sprite>(visualEffectEntity,
                Sprite(Assets::BIG_EXPLOSION, ZIndex::IS_GAME,
                sf::IntRect(0, 0, BIG_EXPLOSION_SPRITE_WIDTH, BIG_EXPLOSION_SPRITE_HEIGHT)));

            this->_engine->addComponent<Animation>(visualEffectEntity,
                Animation(BIG_EXPLOSION_ANIMATION_WIDTH, BIG_EXPLOSION_ANIMATION_HEIGHT,
                    BIG_EXPLOSION_ANIMATION_CURRENT, BIG_EXPLOSION_ANIMATION_ELAPSED_TIME, BIG_EXPLOSION_ANIMATION_DURATION,
                BIG_EXPLOSION_ANIMATION_START, BIG_EXPLOSION_ANIMATION_END, BIG_EXPLOSION_ANIMATION_LOOPING));
            break;

        case protocol::VisualEffectType::VFX_MUZZLE_FLASH:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_IMPACT_SPARK:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_POWERUP_GLOW:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_SHIELD_HIT:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_WARP_IN:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_WARP_OUT:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_CHARGE_BEAM:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_FORCE_DETACH:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_PLAYER_SPAWN:
            //TODO:
            break;

        case protocol::VisualEffectType::VFX_BOSS_INTRO:
            //TODO:
            break;

        // Add more as needed
    }

    // TODO: add a LifeTime Component to destroy it when it is ends
    this->_engine->addComponent<Lifetime>(visualEffectEntity, Lifetime(duration));
}

void Coordinator::playAudioEffect(protocol::AudioEffectType type, float x, float y, float volume, float pitch)
{
    Entity audioEffectEntity = this->_engine->createEntity("AudioEffect");
    this->_engine->addComponent<Transform>(audioEffectEntity, Transform(x, y, 0, 0));
    this->_engine->addComponent<AudioEffect>(audioEffectEntity, AudioEffect(type, volume, pitch));

    switch(type) {
        // WEAPONS
        case protocol::AudioEffectType::SFX_SHOOT_BASIC:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_SHOOT_BASIC, AUDIO_BASIC_PROJECTILE_LOOP, AUDIO_BASIC_PROJECTILE_MIN_DISTANCE, AUDIO_BASIC_PROJECTILE_ATTENUATION, false, AUDIO_SHOOT_BASIC_DURATION));
            break;

        case protocol::AudioEffectType::SFX_SHOOT_CHARGED:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_SHOOT_CHARGED, AUDIO_CHARGED_PROJECTILE_LOOP, AUDIO_CHARGED_PROJECTILE_MIN_DISTANCE, AUDIO_CHARGED_PROJECTILE_ATTENUATION, false, AUDIO_SHOOT_CHARGED_DURATION));
            break;

        case protocol::AudioEffectType::SFX_SHOOT_LASER:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_SHOOT_LASER, AUDIO_LASER_PROJECTILE_LOOP, AUDIO_LASER_PROJECTILE_MIN_DISTANCE, AUDIO_LASER_PROJECTILE_ATTENUATION, false, AUDIO_SHOOT_LASER_DURATION));
            break;

        // EXPLOSIONS
        case protocol::AudioEffectType::SFX_EXPLOSION_SMALL_1:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_EXPLOSION_SMALL_1, false, 200.0f, 0.2f));
            break;

        case protocol::AudioEffectType::SFX_EXPLOSION_SMALL_2:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_EXPLOSION_SMALL_2, false, 200.0f, 0.2f));
            break;

        case protocol::AudioEffectType::SFX_EXPLOSION_LARGE_1:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_EXPLOSION_LARGE_1, false, 300.0f, 0.1f));
            break;

        case protocol::AudioEffectType::SFX_EXPLOSION_LARGE_2:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_EXPLOSION_LARGE_2, false, 300.0f, 0.1f));
            break;

        // POWERUPS
        case protocol::AudioEffectType::SFX_POWERUP_COLLECT_1:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_POWERUP_COLLECT_1, false, 80.0f, 1.0f));
            break;

        case protocol::AudioEffectType::SFX_POWERUP_COLLECT_2:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_POWERUP_COLLECT_2, false, 80.0f, 1.0f));
            break;

        // PLAYER
        case protocol::AudioEffectType::SFX_PLAYER_HIT:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_PLAYER_HIT, false, 100.0f, 0.5f));
            break;

        case protocol::AudioEffectType::SFX_PLAYER_DEATH_1:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_PLAYER_DEATH_1, false, 150.0f, 0.3f));
            break;

        case protocol::AudioEffectType::SFX_PLAYER_DEATH_2:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_PLAYER_DEATH_2, false, 150.0f, 0.3f));
            break;

        case protocol::AudioEffectType::SFX_PLAYER_DEATH_3:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_PLAYER_DEATH_3, false, 150.0f, 0.3f));
            break;

        // FORCE
        case protocol::AudioEffectType::SFX_FORCE_ATTACH:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_FORCE_ATTACH, false, 90.0f, 0.6f));
            break;

        case protocol::AudioEffectType::SFX_FORCE_DETACH:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_FORCE_DETACH, false, 90.0f, 0.6f));
            break;

        // BOSS
        case protocol::AudioEffectType::SFX_BOSS_ROAR:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_BOSS_ROAR, false, 400.0f, 0.05f));
            break;

        // UI
        case protocol::AudioEffectType::SFX_MENU_SELECT:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_MENU_SELECT, false, 0.0f, 0.0f, true));
            break;

        case protocol::AudioEffectType::SFX_MENU_ALERT:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_MENU_ALERT, false, 0.0f, 0.0f, true));
            break;

        // MUSIC (Musics does not have to be in this method, that's why playMusic method exist, just below)
        case protocol::AudioEffectType::MAIN_MENU_MUSIC:
        case protocol::AudioEffectType::FIRST_LEVEL_MUSIC:
        case protocol::AudioEffectType::SECOND_LEVEL_MUSIC:
        case protocol::AudioEffectType::THIRD_LEVEL_MUSIC:
        case protocol::AudioEffectType::FOURTH_LEVEL_MUSIC:
        case protocol::AudioEffectType::VICTORY_MUSIC:
        case protocol::AudioEffectType::DEFEAT_MUSIC:
            LOG_WARN_CAT("Coordinator", "Music type %d should use playMusic() instead of playAudioEffect()", std::to_string(static_cast<int>(type)));
            this->_engine->destroyEntity(audioEffectEntity);
            return;

        default:
            LOG_ERROR_CAT("Coordinator", "Unknown audio effect type: %d", static_cast<int>(type));
            this->_engine->destroyEntity(audioEffectEntity);
            return;
    }

    this->_engine->addComponent<Lifetime>(audioEffectEntity, Lifetime(10.0f)); // 10 secondes max
}

void Coordinator::playMusic(protocol::AudioEffectType musicType)
{
    std::string musicPath;

    switch(musicType) {
        case protocol::AudioEffectType::MAIN_MENU_MUSIC:
            musicPath = pathAudioAssets[MAIN_MENU_MUSIC];
            break;
        case protocol::AudioEffectType::FIRST_LEVEL_MUSIC:
            musicPath = pathAudioAssets[FIRST_LEVEL_MUSIC];
            break;
        case protocol::AudioEffectType::SECOND_LEVEL_MUSIC:
            musicPath = pathAudioAssets[SECOND_LEVEL_MUSIC];
            break;
        case protocol::AudioEffectType::THIRD_LEVEL_MUSIC:
            musicPath = pathAudioAssets[THIRD_LEVEL_MUSIC];
            break;
        case protocol::AudioEffectType::FOURTH_LEVEL_MUSIC:
            musicPath = pathAudioAssets[FOURTH_LEVEL_MUSIC];
            break;
        case protocol::AudioEffectType::VICTORY_MUSIC:
            musicPath = pathAudioAssets[VICTORY_MUSIC];
            break;
        case protocol::AudioEffectType::DEFEAT_MUSIC:
            musicPath = pathAudioAssets[DEFEAT_MUSIC];
            break;
        default:
            LOG_ERROR_CAT("Coordinator", "Unknown music type: %d", static_cast<int>(musicType));
            return;
    }

    this->_engine->getAudioManager()->playMusic(musicPath, 0.5f); // 50% volume

    LOG_INFO_CAT("Coordinator", "Playing music: %s", musicPath.c_str());
}

void Coordinator::stopMusic()
{
    this->_engine->getAudioManager()->stopMusic();
}

