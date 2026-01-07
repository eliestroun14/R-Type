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

void Coordinator::buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
{
    // TODO
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
}

void Coordinator::handlePacketLevelStart(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketForceState(const common::protocol::Packet &packet)
{
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

