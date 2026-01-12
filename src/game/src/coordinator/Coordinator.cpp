/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Coordinator
*/

#include "game/coordinator/Coordinator.hpp"

void Coordinator::initEngine()
{
    this->_engine = std::make_shared<gameEngine::GameEngine>();
    this->_engine->init();
}

void Coordinator::initEngineRender()  // Nouvelle méthode
{
    this->_engine->initRender();
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
    Entity entity = this->_engine->createEntity("Entity_" + std::to_string(playerId));
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

Entity Coordinator::createEnemyEntity(
    uint32_t enemyId,
    float posX,
    float posY,
    float velX,
    float velY,
    uint16_t initialHealth,
    bool withRenderComponents
)
{
    Entity entity = this->_engine->createEntity("Entity_" + std::to_string(enemyId));
    this->setupEnemyEntity(
        entity,
        enemyId,
        posX,
        posY,
        velX,
        velY,
        initialHealth,
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
    Entity entity = this->_engine->createEntity("Entity_" + std::to_string(projectileId));
    this->setupProjectileEntity(
        entity,
        projectileId,
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
    if (withRenderComponents) {
        Assets spriteAsset = _playerSpriteAllocator.allocate(playerId);
        this->_engine->addComponent<Sprite>(entity, Sprite(spriteAsset, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 15)));
        this->_engine->addComponent<Animation>(entity, Animation(33, 15, 2, 0.f, 0.1f, 2, 2, true));
    }

    this->_engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, 2.5f));
    this->_engine->addComponent<Velocity>(entity, Velocity(velX, velY));
    this->_engine->addComponent<Health>(entity, Health(initialHealth, initialHealth));
    this->_engine->addComponent<HitBox>(entity, HitBox());
    this->_engine->addComponent<Weapon>(entity, Weapon(200, 0, 10, ProjectileType::MISSILE));
    this->_engine->addComponent<InputComponent>(entity, InputComponent(playerId));

    // If this is the playable player, set it as local player
    if (isPlayable) {
        this->_engine->addComponent<Playable>(entity, Playable());

        //FIXME: fix here the method in game engine
        // this->_engine->setLocalPlayerEntity(entity, playerId);
        LOG_INFO_CAT("Coordinator", "Local player created with ID %u", playerId);
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
    bool withRenderComponents
)
{
    (void)enemyId;

    this->_engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, 2.0f));
    this->_engine->addComponent<Velocity>(entity, Velocity(velX, velY));
    this->_engine->addComponent<Health>(entity, Health(initialHealth, initialHealth));
    if (withRenderComponents) {
        this->_engine->addComponent<Sprite>(entity, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, BASE_ENEMY_SPRITE_WIDTH, BASE_ENEMY_SPRITE_HEIGHT)));
    }
    this->_engine->addComponent<HitBox>(entity, HitBox());
    this->_engine->addComponent<Weapon>(entity, Weapon(BASE_ENEMY_WEAPON_FIRE_RATE, 0, BASE_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
}

void Coordinator::setupProjectileEntity(
    Entity entity,
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
    this->_engine->addComponent<Transform>(entity, Transform(posX, posY, 0.f, 1.f));
    this->_engine->addComponent<Velocity>(entity, Velocity(velX, velY));
    if (withRenderComponents) {
        this->_engine->addComponent<Sprite>(entity, Sprite(DEFAULT_BULLET, ZIndex::IS_GAME, sf::IntRect(0, 0, 16, 16)));
    }
    this->_engine->addComponent<HitBox>(entity, HitBox());
    this->_engine->addComponent<Projectile>(entity, Projectile(Entity::fromId(projectileId), isPlayerProjectile, damage));
}

void Coordinator::processServerPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
{
    // TODO
    for (const auto& packet : packetsToProcess) {

        if (PacketManager::assertPlayerInput(packet)) {

            handlePlayerInputPacket(packet, elapsedMs);
        }
    }
}

void Coordinator::processClientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
{
    for (const auto& packet : packetsToProcess) {
        // Check packet type first, then validate
        uint8_t packetType = packet.header.packet_type;

        switch (packetType) {
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

void Coordinator::buildSeverPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
{
    // TODO
}

std::vector<uint32_t> Coordinator::getPlayablePlayerIds()
{
    std::vector<uint32_t> playerIds;
    auto playableEntities = this->_engine->getComponents<Playable>();

    for (size_t i = 0; i < playableEntities.size(); ++i) {
        if (playableEntities[i].has_value()) {
            playerIds.push_back(static_cast<uint32_t>(i));
        }
    }

    return playerIds;
}

void Coordinator::buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
{
    auto playerIds = getPlayablePlayerIds();

    // TODO: add if for the elapsed time to limit packet sending rate
    // Should only have one playable per client
    if (!playerIds.empty()) {
        common::protocol::Packet packet;
        if (createPacketInputClient(&packet, playerIds[0])) {
            outgoingPackets.push_back(packet);
        }
    }

    LOG_DEBUG("buildClientPacketBasedOnStatus: sent {} packets", outgoingPackets.size());
}

bool Coordinator::createPacketInputClient(common::protocol::Packet* packet, uint32_t playerId)
{
    if (!packet) {
        LOG_ERROR_CAT("Coordinator", "createPacketInputClient: packet pointer is null");
        return false;
    }

    // Get the entity for the player ID
    Entity playerEntity = this->_engine->getEntityFromId(playerId);
    if (!this->_engine->isAlive(playerEntity)) {
        LOG_WARN_CAT("Coordinator", "createPacketInputClient: player entity %u is not alive", playerId);
        return false;
    }

    auto& inputOpt = this->_engine->getComponentEntity<InputComponent>(playerEntity);
    if (!inputOpt.has_value()) {
        LOG_WARN_CAT("Coordinator", "createPacketInputClient: player %u has no input component", playerId);
        return false;
    }

    InputComponent& inputComp = inputOpt.value();
    uint32_t actualPlayerId = inputComp.playerId;
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

    // Format: [flags_count(1) + flags(1) + sequence_number(4) + timestamp(4) + player_id(4) + input_state(2) + aim_dir_x(2) + aim_dir_y(2)]
    std::vector<uint8_t> inputArgs;
    inputArgs.resize(20);  // 1 + 1 + 4 + 4 + 4 + 2 + 2 + 2
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

    auto playerInputPacket = PacketManager::createPlayerInput(inputArgs);
    if (!playerInputPacket.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketInputClient: failed to create player input packet");
        return false;
    }

    *packet = playerInputPacket.value();
    LOG_DEBUG_CAT("Coordinator", "createPacketInputClient: packet created for player %u", actualPlayerId);
    return true;
}

void Coordinator::handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs)
{
    // Parse the player input packet
    auto parsed = PacketManager::parsePlayerInput(packet);
    if (!parsed.has_value()) {
        return;  // Invalid packet, ignore
    }

    // Find the entity for this player by iterating over all InputComponents
    auto& inputComponents = this->_engine->getComponents<InputComponent>();
    for (size_t entityId = 0; entityId < inputComponents.size(); ++entityId) {
        auto& input = inputComponents[entityId];
        if (input.has_value() && input->playerId == parsed->playerId) {
            Entity entity = Entity::fromId(entityId);
            // Update all input actions
            for (const auto& [action, isPressed] : parsed->actions) {
                this->_engine->setPlayerInputAction(entity, parsed->playerId, action, isPressed);
            }
            break;
        }
    }
}

void Coordinator::handlePacketCreateEntity(const common::protocol::Packet& packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != ENTITY_SPAWN_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketCreateEntity: invalid packet size %zu, expected %d", packet.data.size(), ENTITY_SPAWN_PAYLOAD_SIZE);
        return;
    }

    // Parse the ENTITY_SPAWN payload in one memcpy
    protocol::EntitySpawnPayload payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "Entity created: id=%u type=%u pos=(%.1f, %.1f) health=%u",
        payload.entity_id, payload.entity_type, static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), payload.initial_health);

    // Create the entity
    Entity newEntity = this->_engine->createEntity("Entity_" + std::to_string(payload.entity_id));

    // Add type-specific components
    switch (payload.entity_type) {
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER): {
            this->setupPlayerEntity(
                newEntity,
                payload.entity_id,
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
            this->setupEnemyEntity(
                newEntity,
                payload.entity_id,
                static_cast<float>(payload.position_x),
                static_cast<float>(payload.position_y),
                static_cast<float>(payload.initial_velocity_x),
                static_cast<float>(payload.initial_velocity_y),
                payload.initial_health,
                /*withRenderComponents=*/true
            );
            LOG_INFO_CAT("Coordinator", "Enemy created with ID %u at (%.1f, %.1f)", payload.entity_id, static_cast<float>(payload.position_x), static_cast<float>(payload.position_y));
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

            this->setupProjectileEntity(
                newEntity,
                payload.entity_id,
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
            LOG_WARN_CAT("Coordinator", "Unknown entity type %u", payload.entity_type);
            break;
    }
}



void Coordinator::handlePacketDestroyEntity(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != ENTITY_DESTROY_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketDestroyEntity: invalid packet size %zu, expected %d", packet.data.size(), ENTITY_DESTROY_PAYLOAD_SIZE);
        return;
    }

    // Parse the DESTROY_ENTITY payload in one memcpy
    protocol::EntityDestroyPayload payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "Entity destroyed: id=%u reason=%u final_pos=(%.1f, %.1f)",
        payload.entity_id, payload.destroy_reason, static_cast<float>(payload.final_position_x), static_cast<float>(payload.final_position_y));

    // Destroy the entity
    this->_engine->destroyEntity(payload.entity_id);
}


void Coordinator::handlePacketTransformSnapshot(const common::protocol::Packet& packet)
{
    constexpr std::size_t BASE_SIZE  = sizeof(uint32_t) + sizeof(uint16_t);                         // 6
    constexpr std::size_t ENTRY_SIZE = sizeof(uint32_t) + sizeof(protocol::ComponentTransform);     // 12

    const std::size_t size = packet.data.size();
    if (size < BASE_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketTransformSnapshot: payload too small (%zu), expected >= %zu",size, BASE_SIZE);
        return;
    }
    if ((size - BASE_SIZE) % ENTRY_SIZE != 0) {
        LOG_ERROR_CAT("Coordinator", "handlePacketTransformSnapshot: invalid payload size %zu, expected %zu + %zu*N", size, BASE_SIZE, ENTRY_SIZE);
        return;
    }

    const std::uint8_t* const data =
        reinterpret_cast<const std::uint8_t*>(packet.data.data());

    uint32_t world_tick = 0;
    uint16_t entity_count = 0;
    std::memcpy(&world_tick, data, sizeof(world_tick));
    std::memcpy(&entity_count, data + sizeof(world_tick), sizeof(entity_count));

    const std::size_t computed_count = (size - BASE_SIZE) / ENTRY_SIZE;
    if (static_cast<std::size_t>(entity_count) != computed_count) {
        LOG_ERROR_CAT("Coordinator", "handlePacketTransformSnapshot: entity_count mismatch (packet=%u computed=%zu)", entity_count, computed_count);
        return;
    }

    LOG_INFO_CAT("Coordinator", "TransformSnapshot: world_tick=%u entity_count=%u", world_tick, entity_count);

    std::size_t offset = BASE_SIZE;

    for (uint16_t i = 0; i < entity_count; ++i) {
        uint32_t entity_id = 0;
        protocol::ComponentTransform net{};

        std::memcpy(&entity_id, packet.data.data() + offset, sizeof(entity_id));
        offset += sizeof(entity_id);

        std::memcpy(&net, packet.data.data() + offset, sizeof(net));
        offset += sizeof(net);

        Entity entity = this->_engine->getEntityFromId(entity_id);

        const Transform tf(
            static_cast<float>(net.pos_x),
            static_cast<float>(net.pos_y),
            (static_cast<float>(net.rotation) * 360.0f) / 65535.0f,
            static_cast<float>(net.scale) / 1000.0f
        );

        try {
            this->_engine->updateComponent<Transform>(entity, tf);
        } catch (const std::exception&) {
            this->_engine->emplaceComponent<Transform>(entity, tf);
        }
    }
}

void Coordinator::handlePacketHealthSnapshot(const common::protocol::Packet &packet)
{
    // Validate snapshot size using the protocol define
    if (packet.data.size() != HEALTH_SNAPSHOT_BASE_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketHealthSnapshot: invalid packet size %zu, expected %d", packet.data.size(), HEALTH_SNAPSHOT_BASE_SIZE);
        return;
    }

    // Parse the HEALTH_SNAPSHOT snapshot in one memcpy
    protocol::HealthSnapshot snapshot;
    std::memcpy(&snapshot, packet.data.data(), sizeof(snapshot));

    LOG_INFO_CAT("Coordinator", "HealthSnaphot: world_tick=%u entity_count=%u",
        snapshot.world_tick, snapshot.entity_count);

    size_t offset = sizeof(protocol::HealthSnapshot);
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

void Coordinator::handlePacketWeaponSnapshot(const common::protocol::Packet &packet)
{
    // Validate snapshot size using the protocol define
    if (packet.data.size() != WEAPON_SNAPSHOT_BASE_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketWeaponSnapthot: invalid packet size %zu, expected %d", packet.data.size(), WEAPON_SNAPSHOT_BASE_SIZE);
        return;
    }

    // Parse the HEALTH_SNAPSHOT snapshot in one memcpy
    protocol::WeaponSnapshot snapshot;
    std::memcpy(&snapshot, packet.data.data(), sizeof(snapshot));

    LOG_INFO_CAT("Coordinator", "WeaponSnapshot: world_tick=%u entity_count=%u",
        snapshot.world_tick, snapshot.entity_count);

    size_t offset = sizeof(protocol::WeaponSnapshot);
    for (uint16_t i = 0; i < snapshot.entity_count; i++) {
        uint32_t entity_id;
        Weapon weapon(0, 0, 0, ProjectileType::UNKNOWN);

        std::memcpy(&entity_id, packet.data.data() + offset, sizeof(entity_id));
        offset += sizeof(entity_id);

        std::memcpy(&weapon, packet.data.data() + offset, sizeof(weapon));
        offset += sizeof(weapon);

        Entity entity = this->_engine->getEntityFromId(entity_id);

        try {
            this->_engine->updateComponent<Weapon>(entity, weapon);
        } catch (const std::exception& e) {
            // if the entity does not have the component, set the component
            this->_engine->emplaceComponent<Weapon>(entity, weapon);
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
        LOG_ERROR_CAT("Coordinator", "handlePacketAnimationSnapshot: packet too small (%zu bytes)", data.size());
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
        LOG_ERROR_CAT("Coordinator", "handlePacketAnimationSnapshot: invalid packet size %zu, expected %zu", 
            data.size(), 6 + (entity_count * 11));
        return;
    }

    LOG_INFO_CAT("Coordinator", "AnimationSnapshot: world_tick=%u entity_count=%u", world_tick, entity_count);

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
        LOG_ERROR_CAT("Coordinator", "handlePacketComponentRemove: invalid packet size %zu, expected %d", packet.data.size(), COMPONENT_REMOVE_PAYLOAD_SIZE);
        return;
    }

    // Parse the HEALTH_SNAPSHOT snapshot in one memcpy
    protocol::ComponentRemove payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "ComponentRemove: component_type=%u entity_id=%u",
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
        LOG_ERROR_CAT("Coordinator", "handlePacketHealthSnapshotDelta: invalid packet size %zu, expected %d", packet.data.size(), HEALTH_SNAPSHOT_DELTA_BASE_SIZE);
        return;
    }

    // Parse the HEALTH_SNAPSHOT_DELTA snapshot in one memcpy
    protocol::HealthSnapshotDelta snapshot;
    std::memcpy(&snapshot, packet.data.data(), sizeof(snapshot));

    LOG_INFO_CAT("Coordinator", "HealthSnaphot: world_tick=%u entity_count=%u",
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
        LOG_ERROR_CAT("Coordinator", "handlePacketPlayerHit: invalid packet size %zu, expected %d", packet.data.size(), PLAYER_HIT_PAYLOAD_SIZE);
        return;
    }

    // Parse the PLAYER_HIT_PAYLOAD_SIZE payload in one memcpy
    protocol::PlayerHit payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "attacker_id: id=%u damage=%u hit_pos=(%.1f, %.1f) player_id=%u remaining_health=%u remaining_shield=%u",
        payload.attacker_id, payload.damage, static_cast<float>(payload.hit_pos_x), static_cast<float>(payload.hit_pos_y), payload.player_id,
        payload.remaining_health, payload.remaining_shield);


    // Hit player
    Entity playerHit = this->_engine->getEntityFromId(payload.player_id);

    try
    {
        auto& health = this->_engine->getComponentEntity<Health>(playerHit);
        health->currentHealth = payload.remaining_health;
        // TODO: handle the shied here, add a component and add the remaining_shield

        LOG_INFO_CAT("Coordinator", "Player %u health updated: hp=%u shield=%u",
            payload.player_id, payload.remaining_health, payload.remaining_shield);


        //TODO: add sound effects
        // this->_engine->playHitEffect(playerHit, payload.hit_pos_x, payload.hit_pos_y);
        // this->_engine->playHitSound(payload.damage);

    }
    catch(const Error &e)
    {
        LOG_ERROR_CAT("Coordinator", "Failed to update player %u health: %s",
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
        LOG_ERROR_CAT("Coordinator", "handlePacketPlayerDeath: invalid packet size %zu, expected %d", packet.data.size(), PLAYER_DEATH_PAYLOAD_SIZE);
        return;
    }

    // Parse the PLAYER_DEATH_PAYLOAD_SIZE payload in one memcpy
    protocol::PlayerDeath payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "killer_id: id=%u death_pos=(%.1f, %.1f) player_id=%u score_before_death=%u",
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
        LOG_ERROR_CAT("Coordinator", "handlePacketScoreUpdate: invalid packet size %zu, expected %d", packet.data.size(), SCORE_UPDATE_PAYLOAD_SIZE);
        return;
    }

    // Parse the SCORE_UPDATE_PAYLOAD_SIZE payload in one memcpy
    protocol::ScoreUpdate payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "player_id=%u new_score=%u reason=%u score_delta=%u",
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
        LOG_ERROR_CAT("Coordinator", "handlePacketPowerupPickup: invalid packet size %zu, expected %d", packet.data.size(), POWER_PICKUP_PAYLOAD_SIZE);
        return;
    }

    // Parse the POWER_PICKUP_PAYLOAD_SIZE payload in one memcpy
    protocol::PowerupPickup payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "player_id=%u powerup_id=%u powerup_type=%u duration=%u",
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
    // Validate payload size using the protocol define
    if (packet.data.size() != WEAPON_FIRE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketWeaponFire: invalid packet size %zu, expected %d", packet.data.size(), WEAPON_FIRE_PAYLOAD_SIZE);
        return;
    }

    // Parse the WEAPON_FIRE_PAYLOAD_SIZE payload in one memcpy
    protocol::WeaponFire payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "shooter_id=%u projectile_id=%u weapon_type=%u origin_pos=(%.1f, %.1f), direction_pos=(%.1f, %.1f)",
                payload.shooter_id, payload.projectile_id, payload.weapon_type, payload.origin_x, payload.origin_y, payload.direction_x, payload.direction_y);

    // get the shooter
    Entity shooter = this->_engine->getEntityFromId(payload.shooter_id);

    float origin_x = static_cast<float>(payload.origin_x);
    float origin_y = static_cast<float>(payload.origin_y);


    float direction_x = static_cast<float>(payload.direction_x) / 1000.0f;
    float direction_y = static_cast<float>(payload.direction_y) / 1000.0f;


    Entity projectile = spawnProjectile(shooter, payload.projectile_id, payload.weapon_type, payload.origin_x,
        payload.origin_y, payload.direction_x, payload.direction_y);

    // Optional: Play fire effects for the shooter
    //TODO: play sound if necessary
    // this->_engine->playWeaponFireSound(payload.weapon_type);

    LOG_INFO_CAT("Coordinator", "Projectile %u spawned from shooter %u", 
                 payload.projectile_id, payload.shooter_id);
}

void Coordinator::handlePacketVisualEffect(const common::protocol::Packet &packet)
{
    if (packet.data.size() != VISUAL_EFFECT_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketVisualEffect: invalid packet size %zu, expected %d", packet.data.size(), VISUAL_EFFECT_PAYLOAD_SIZE);
        return;
    }

    // Parse the VISUAL_EFFECT_PAYLOAD_SIZE payload in one memcpy
    protocol::VisualEffect payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "effect_type=%u scale=%u duration_ms=%u color_tint_r=%u color_tint_g=%u color_tint_b=%u pos=(%.1f, %.1f)",
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

    LOG_INFO_CAT("Coordinator", "Visual effect %u spawned at (%.1f, %.1f) with scale %.2fx for %.2fs", 
                 payload.effect_type, pos_x, pos_y, scale, duration);

}

void Coordinator::handlePacketAudioEffect(const common::protocol::Packet &packet)
{
    if (packet.data.size() != AUDIO_EFFECT_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "handlePacketAudioEffect: invalid packet size %zu, expected %d", 
                      packet.data.size(), AUDIO_EFFECT_PAYLOAD_SIZE);
        return;
    }

    protocol::AudioEffect payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "audio_effect_type=%u volume=%u pitch=%u pos=(%.1f, %.1f)",
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
        LOG_ERROR_CAT("Coordinator", "handlePacketParticleSpawn: invalid packet size %zu, expected %d", 
                      packet.data.size(), PARTICLE_SPAWN_PAYLOAD_SIZE);
        return;
    }

    // Parse the PARTICLE_SPAWN payload in one memcpy
    protocol::ParticleSpawn payload;
    std::memcpy(&payload, packet.data.data(), sizeof(payload));

    LOG_INFO_CAT("Coordinator", "particle_system_id=%u particle_count=%u lifetime_ms=%u pos=(%.1f, %.1f) velocity=(%.1f, %.1f) color_start=(%u,%u,%u) color_end=(%u,%u,%u)",
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

    LOG_INFO_CAT("Coordinator", "Particle system %u spawned with %u particles at (%.1f, %.1f)", 
                 payload.particle_system_id, payload.particle_count, pos_x, pos_y);
}

void Coordinator::handleGameStart(const common::protocol::Packet& packet)
{
    if (packet.data.size() != GAME_START_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "GameStart: invalid size %zu", packet.data.size());
        return;
    }

    _gameRunning = true;
}

void Coordinator::handleGameEnd(const common::protocol::Packet& packet)
{
    if (packet.data.size() != GAME_END_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "GameEnd: invalid size %zu", packet.data.size());
        return;
    }

    _gameRunning = false;
}

void Coordinator::handlePacketLevelComplete(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != LEVEL_COMPLETE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "Invalid LEVEL_COMPLETE payload size: expected %zu, got %zu",
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

    LOG_INFO_CAT("Coordinator", "Level completed: level=%u next=%u bonus_score=%u time=%u seconds",
        completed_level, next_level, bonus_score, completion_time);

    // Display level completion message
    std::string completionMessage;
    if (next_level == 0xFF) {
        // Game is complete
        completionMessage = "GAME COMPLETE! Final Score Bonus: " + std::to_string(bonus_score);
        LOG_INFO_CAT("Coordinator", "Game completed! Final bonus: %u", bonus_score);
    } else {
        // Level complete, more levels ahead
        completionMessage = "LEVEL " + std::to_string(completed_level) + " COMPLETE! Bonus: " + std::to_string(bonus_score);
        LOG_INFO_CAT("Coordinator", "Level %u completed, preparing level %u", completed_level, next_level);
    }

    // Create a UI text entity to display the completion message
    Entity messageEntity = this->_engine->createEntity("LevelCompleteMessage");
    this->_engine->addComponent<Transform>(messageEntity, Transform(400.f, 300.f, 0.f, 1.5f));
    this->_engine->addComponent<Text>(messageEntity, Text(completionMessage));
    this->_engine->addComponent<Sprite>(messageEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));

    // Update game state based on completion
    if (next_level == 0xFF) {
        // Game is finished - stop running state
        _gameRunning = false;
        LOG_INFO_CAT("Coordinator", "Game ended - all levels completed");
        
        // Display final game stats
        std::string timeMessage = "Completion Time: " + std::to_string(completion_time) + " seconds";
        Entity timeEntity = this->_engine->createEntity("CompletionTime");
        this->_engine->addComponent<Transform>(timeEntity, Transform(400.f, 350.f, 0.f, 1.0f));
        this->_engine->addComponent<Text>(timeEntity, Text(timeMessage));
        this->_engine->addComponent<Sprite>(timeEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));
    } else {
        // More levels to play - keep game running
        LOG_INFO_CAT("Coordinator", "Waiting for server to start level %u", next_level);
        
        // Display "Next Level" message
        std::string nextLevelMessage = "Next Level: " + std::to_string(next_level);
        Entity nextEntity = this->_engine->createEntity("NextLevelMessage");
        this->_engine->addComponent<Transform>(nextEntity, Transform(400.f, 350.f, 0.f, 1.0f));
        this->_engine->addComponent<Text>(nextEntity, Text(nextLevelMessage));
        this->_engine->addComponent<Sprite>(nextEntity, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_UI_HUD));
    }
}

void Coordinator::handlePacketLevelStart(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != LEVEL_START_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "Invalid LEVEL_START payload size: expected %zu, got %zu",
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

    LOG_INFO_CAT("Coordinator", "Level started: id=%u name=\"%s\" estimated_duration=%u seconds",
        level_id, levelNameStr.c_str(), estimated_duration);

    // Set game to running state
    _gameRunning = true;

    // Create UI entity to display level start information
    std::string startMessage = "LEVEL " + std::to_string(level_id) + ": " + levelNameStr;
    Entity levelStartEntity = this->_engine->createEntity("LevelStartMessage");
    this->_engine->addComponent<Transform>(levelStartEntity, Transform(400.f, 200.f, 0.f, 2.0f));
    this->_engine->addComponent<Text>(levelStartEntity, Text(startMessage));
    this->_engine->addComponent<Sprite>(levelStartEntity, Sprite(DEFAULT_BULLET, ZIndex::IS_UI_HUD));

    // Display estimated duration if provided
    if (estimated_duration > 0) {
        std::string durationMessage = "Estimated Time: " + std::to_string(estimated_duration) + "s";
        Entity durationEntity = this->_engine->createEntity("LevelDuration");
        this->_engine->addComponent<Transform>(durationEntity, Transform(400.f, 250.f, 0.f, 1.0f));
        this->_engine->addComponent<Text>(durationEntity, Text(durationMessage));
        this->_engine->addComponent<Sprite>(durationEntity, Sprite(DEFAULT_BULLET, ZIndex::IS_UI_HUD));
    }

    // Initialize level component if needed
    // Note: The server will spawn level entities and waves through TYPE_ENTITY_SPAWN packets
    LOG_INFO_CAT("Coordinator", "Level %u (%s) ready - waiting for entity spawns", 
        level_id, levelNameStr.c_str());
}

void Coordinator::handlePacketForceState(const common::protocol::Packet &packet)
{
    // Validate payload size using the protocol define
    if (packet.data.size() != FORCE_STATE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "Invalid FORCE_STATE payload size: expected %zu, got %zu",
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

    LOG_INFO_CAT("Coordinator", "Force state: entity=%u parent=%u attach=%s power=%u charge=%u%% firing=%s",
        force_entity_id, parent_ship_id, attachment_str, power_level, charge_percentage,
        is_firing ? "YES" : "NO");

    // Get the Force entity
    Entity forceEntity = Entity::fromId(force_entity_id);
    if (!this->_engine->isAlive(forceEntity)) {
        LOG_WARN_CAT("Coordinator", "Force entity %u does not exist in engine", force_entity_id);
        return;
    }

    // Get or add Force component
    auto& forceComponent = this->_engine->getComponentEntity<Force>(forceEntity);
    if (!forceComponent.has_value()) {
        // Force component doesn't exist, create it
        this->_engine->addComponent<Force>(forceEntity, 
            Force(parent_ship_id, static_cast<ForceAttachmentPoint>(attachment_point), 
                  power_level, charge_percentage, is_firing != 0));
        LOG_DEBUG_CAT("Coordinator", "Created Force component for entity %u", force_entity_id);
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
        LOG_DEBUG_CAT("Coordinator", "Force %u is detached and moving independently", force_entity_id);
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
                
                LOG_DEBUG_CAT("Coordinator", "Force %u attached to parent %u at (%.1f, %.1f)",
                    force_entity_id, parent_ship_id, forceTransform->x, forceTransform->y);
            }
        } else {
            LOG_WARN_CAT("Coordinator", "Parent ship %u does not exist for Force %u", 
                parent_ship_id, force_entity_id);
        }
    }
}

void Coordinator::handlePacketAIState(const common::protocol::Packet &packet)
{
    // Validate payload size
    if (packet.data.size() != AI_STATE_PAYLOAD_SIZE) {
        LOG_ERROR_CAT("Coordinator", "AIState: invalid packet size %zu, expected %d",
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
        LOG_WARN_CAT("Coordinator", "AIState: entity %u is not alive", entity_id);
        return;
    }

    // Get the AI component
    auto& optAI = this->_engine->getComponentEntity<AI>(entity);
    if (!optAI.has_value()) {
        LOG_WARN_CAT("Coordinator", "AIState: entity %u has no AI component", entity_id);
        return;
    }

    // Update the AI component with new state information
    AI& ai = optAI.value();
    ai.internalTime = static_cast<float>(state_timer);

    LOG_DEBUG_CAT("Coordinator", "AIState updated: entity=%u state=%u behavior=%u target=%u waypoint=(%.0f, %.0f) timer=%u",
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
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: entity %u is not alive", entityId);
        return false;
    }

    // Retrieve components from the engine
    auto transformOpt = this->_engine->getComponentEntity<Transform>(entity);
    auto velocityOpt = this->_engine->getComponentEntity<Velocity>(entity);
    auto healthOpt = this->_engine->getComponentEntity<Health>(entity);
    auto networkIdOpt = this->_engine->getComponentEntity<NetworkId>(entity);

    if (!transformOpt.has_value()) {
        LOG_ERROR_CAT("Coordinator", "createPacketEntitySpawn: entity %u has no Transform component", entityId);
        return false;
    }

    Transform& transform = transformOpt.value();
    
    // Build args vector for PacketManager
    std::vector<uint8_t> args;
    
    // flags_count (0 for now)
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
    
    // mob_variant (0 for default)
    uint8_t mob_variant = 0;
    args.push_back(mob_variant);
    
    // initial_health
    uint8_t initial_health = healthOpt.has_value() ? static_cast<uint8_t>(healthOpt.value().currentHealth) : 100;
    args.push_back(initial_health);
    
    // initial_velocity_x, initial_velocity_y
    int16_t velocity_x = velocityOpt.has_value() ? static_cast<int16_t>(velocityOpt.value().vx) : 0;
    int16_t velocity_y = velocityOpt.has_value() ? static_cast<int16_t>(velocityOpt.value().vy) : 0;
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
    
    LOG_DEBUG_CAT("Coordinator", "createPacketEntitySpawn: created packet for entity %u", entityId);
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
            LOG_WARN_CAT("Coordinator", "createPacketTransformSnapshot: skipping dead entity %u", entityId);
            continue;
        }

        auto transformOpt = this->_engine->getComponentEntity<Transform>(entity);
        if (!transformOpt.has_value()) {
            LOG_WARN_CAT("Coordinator", "createPacketTransformSnapshot: entity %u has no Transform", entityId);
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
        uint16_t scale = static_cast<uint16_t>(transform.scale * 100.0f);
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
    LOG_DEBUG_CAT("Coordinator", "createPacketTransformSnapshot: created packet for %zu entities", entityIds.size());
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

        // entity_id (4 bytes)
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&entityId), 
                    reinterpret_cast<uint8_t*>(&entityId) + sizeof(entityId));

                    // current_health (2 bytes)
        uint16_t current_health = static_cast<uint16_t>(health.currentHealth);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&current_health), 
                    reinterpret_cast<uint8_t*>(&current_health) + sizeof(current_health));

                    // max_health (2 bytes)
        uint16_t max_health = static_cast<uint16_t>(health.maxHp);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&max_health), 
                    reinterpret_cast<uint8_t*>(&max_health) + sizeof(max_health));
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
    LOG_DEBUG_CAT("Coordinator", "createPacketHealthSnapshot: created packet for %zu entities", entityIds.size());
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

                    // fire_rate (2 bytes)
        uint16_t fire_rate = static_cast<uint16_t>(weapon.fireRateMs);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&fire_rate), 
                    reinterpret_cast<uint8_t*>(&fire_rate) + sizeof(fire_rate));

                    // damage (2 bytes)
        uint16_t damage = static_cast<uint16_t>(weapon.damage);
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&damage), 
                    reinterpret_cast<uint8_t*>(&damage) + sizeof(damage));

                    // projectile_type (1 byte)
        uint8_t projectile_type = static_cast<uint8_t>(weapon.projectileType);
        args.push_back(projectile_type);

        // last_shot_time (4 bytes)
        uint32_t last_shot_time = weapon.lastShotTime;
        args.insert(args.end(), reinterpret_cast<uint8_t*>(&last_shot_time), 
                    reinterpret_cast<uint8_t*>(&last_shot_time) + sizeof(last_shot_time));
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
    LOG_DEBUG_CAT("Coordinator", "createPacketWeaponSnapshot: created packet for %zu entities", entityIds.size());
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
    LOG_DEBUG_CAT("Coordinator", "createPacketEntityDestroy: created packet for entity %u", entityId);
    return true;
}

std::shared_ptr<gameEngine::GameEngine> Coordinator::getEngine() const
{
    return this->_engine;
Entity Coordinator::spawnProjectile(Entity shooter, uint32_t projectile_id, uint8_t weapon_type, float origin_x, float origin_y, float dir_x, float dir_y)
{
    bool isFromPlayable = false;
    std::string projectileName = this->_engine->getEntityName(shooter) + " projectile";

    auto& shooterWeapon = this->_engine->getComponentEntity<Weapon>(shooter);

    if (this->_engine->hasComponent<InputComponent>(shooter))
        isFromPlayable = true;

    Entity projectile = this->_engine->createEntityWithId(projectile_id, projectileName);
    float projectileSpeed = 1.5f;  // tuned for visible travel with dt in ms

    switch (weapon_type) {
        case 0x00: // WEAPON_TYPE_BASIC
            this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, DEFAULT_PROJ_ROTATION, DEFAULT_PROJ_SCALE));
            this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
            this->_engine->addComponent<Projectile>(projectile, Projectile(shooter, isFromPlayable, shooterWeapon->damage));
            this->_engine->addComponent<Sprite>(projectile, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_GAME,
                sf::IntRect(0, 0, DEFAULT_PROJ_SPRITE_WIDTH, DEFAULT_PROJ_SPRITE_HEIGHT)));
            this->_engine->addComponent<Animation>(projectile, Animation(DEFAULT_PROJ_ANIMATION_WIDTH,
                DEFAULT_PROJ_ANIMATION_HEIGHT, DEFAULT_PROJ_ANIMATION_CURRENT, DEFAULT_PROJ_ANIMATION_ELAPSED_TIME, DEFAULT_PROJ_ANIMATION_DURATION,
                DEFAULT_PROJ_ANIMATION_START, DEFAULT_PROJ_ANIMATION_END, DEFAULT_PROJ_ANIMATION_LOOPING));
            this->_engine->addComponent<HitBox>(projectile, HitBox());
            break;

        case 0x01: // WEAPON_TYPE_CHARGED
            this->_engine->addComponent<Transform>(projectile, Transform(origin_x, origin_y, CHARGED_PROJ_ROTATION, CHARGED_PROJ_SCALE));
            this->_engine->addComponent<Velocity>(projectile, Velocity(dir_x * projectileSpeed, dir_y * projectileSpeed));
            this->_engine->addComponent<Projectile>(projectile, Projectile(shooter, isFromPlayable, shooterWeapon->damage));
            this->_engine->addComponent<Sprite>(projectile, Sprite(Assets::DEFAULT_BULLET, ZIndex::IS_GAME,
                sf::IntRect(0, 0, CHARGED_PROJ_SPRITE_WIDTH, CHARGED_PROJ_SPRITE_HEIGHT)));
            this->_engine->addComponent<Animation>(projectile, Animation(CHARGED_PROJ_ANIMATION_WIDTH,
                CHARGED_PROJ_ANIMATION_HEIGHT, CHARGED_PROJ_ANIMATION_CURRENT, CHARGED_PROJ_ANIMATION_ELAPSED_TIME, CHARGED_PROJ_ANIMATION_DURATION,
                CHARGED_PROJ_ANIMATION_START, CHARGED_PROJ_ANIMATION_END, CHARGED_PROJ_ANIMATION_LOOPING));
            this->_engine->addComponent<HitBox>(projectile, HitBox());
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
                AudioSource(AudioAssets::SFX_SHOOT_BASIC, false, 100.0f, 0.5f));
            break;

        case protocol::AudioEffectType::SFX_SHOOT_CHARGED:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_SHOOT_CHARGED, false, 150.0f, 0.3f));
            break;

        case protocol::AudioEffectType::SFX_SHOOT_LASER:
            this->_engine->addComponent<AudioSource>(audioEffectEntity,
                AudioSource(AudioAssets::SFX_SHOOT_LASER, false, 120.0f, 0.4f));
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

std::shared_ptr<gameEngine::GameEngine> Coordinator::getEngine() const
{
    return this->_engine;
}

