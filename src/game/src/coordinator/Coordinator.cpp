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
            Assets spriteAsset = _playerSpriteAllocator.allocate(payload.entity_id);

            this->_engine->addComponent<Transform>(newEntity, Transform(static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), 0.f, 2.5f));
            this->_engine->addComponent<Velocity>(newEntity, Velocity(static_cast<float>(payload.initial_velocity_x), static_cast<float>(payload.initial_velocity_y)));
            this->_engine->addComponent<Health>(newEntity, Health(payload.initial_health, payload.initial_health));
            this->_engine->addComponent<Sprite>(newEntity, Sprite(spriteAsset, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 15)));
            this->_engine->addComponent<Animation>(newEntity, Animation(33, 15, 2, 0.f, 0.1f, 2, 2, true));
            this->_engine->addComponent<HitBox>(newEntity, HitBox());
            this->_engine->addComponent<Weapon>(newEntity, Weapon(200, 0, 10, ProjectileType::MISSILE));
            this->_engine->addComponent<InputComponent>(newEntity, InputComponent(payload.entity_id));

            // If this is the playable player, set it as local player
            if (payload.is_playable) {
                this->_engine->addComponent<Playable>(newEntity, Playable());

                //FIXME: fix here the method in game engine
                // this->_engine->setLocalPlayerEntity(newEntity, payload.entity_id);
                LOG_INFO_CAT("Coordinator", "Local player created with ID %u", payload.entity_id);
            }
            break;
        }
        case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_ENEMY): {
            this->_engine->addComponent<Transform>(newEntity, Transform(static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), 0.f, 2.0f));
            this->_engine->addComponent<Velocity>(newEntity, Velocity(static_cast<float>(payload.initial_velocity_x), static_cast<float>(payload.initial_velocity_y)));
            this->_engine->addComponent<Health>(newEntity, Health(payload.initial_health, payload.initial_health));
            this->_engine->addComponent<Sprite>(newEntity, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, BASE_ENEMY_SPRITE_WIDTH, BASE_ENEMY_SPRITE_HEIGHT)));
            this->_engine->addComponent<HitBox>(newEntity, HitBox());
            this->_engine->addComponent<Weapon>(newEntity, Weapon(BASE_ENEMY_WEAPON_FIRE_RATE, 0, BASE_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
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
            this->_engine->addComponent<Transform>(newEntity, Transform(static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), 0.f, 1.f));
            this->_engine->addComponent<Velocity>(newEntity, Velocity(static_cast<float>(payload.initial_velocity_x), static_cast<float>(payload.initial_velocity_y)));
            this->_engine->addComponent<Sprite>(newEntity, Sprite(DEFAULT_BULLET, ZIndex::IS_GAME, sf::IntRect(0, 0, 16, 16)));
            this->_engine->addComponent<HitBox>(newEntity, HitBox());
            this->_engine->addComponent<Projectile>(newEntity, Projectile(Entity::fromId(payload.entity_id), isPlayerProjectile, 10));

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
}

void Coordinator::handlePacketScoreUpdate(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketPowerupPickup(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketWeaponFire(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketVisualEffect(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketAudioEffect(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketParticleSpawn(const common::protocol::Packet &packet)
{
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
