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

// ==============================================================
//                     CREATE PACKET METHODS
// ==============================================================

void Coordinator::createEntitySpawnPacket(
    common::protocol::Packet* packet,
    uint32_t entityId,
    uint8_t entityType,
    uint16_t posX,
    uint16_t posY,
    uint8_t mobVariant,
    uint8_t initialHealth,
    int16_t initialVelX,
    int16_t initialVelY,
    uint8_t isPlayable)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare payload
    protocol::EntitySpawnPayload payload;
    payload.entity_id = entityId;
    payload.entity_type = entityType;
    payload.position_x = posX;
    payload.position_y = posY;
    payload.mob_variant = mobVariant;
    payload.initial_health = initialHealth;
    payload.initial_velocity_x = initialVelX;
    payload.initial_velocity_y = initialVelY;
    payload.is_playable = isPlayable;
    
    // Copy payload to packet data
    packet->data.resize(ENTITY_SPAWN_PAYLOAD_SIZE);
    std::memcpy(packet->data.data(), &payload, ENTITY_SPAWN_PAYLOAD_SIZE);
}

void Coordinator::createEntityDestroyPacket(
    common::protocol::Packet* packet,
    uint32_t entityId,
    uint8_t destroyReason,
    uint16_t finalPosX,
    uint16_t finalPosY)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_DESTROY);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare payload
    protocol::EntityDestroyPayload payload;
    payload.entity_id = entityId;
    payload.destroy_reason = destroyReason;
    payload.final_position_x = finalPosX;
    payload.final_position_y = finalPosY;
    
    // Copy payload to packet data
    packet->data.resize(ENTITY_DESTROY_PAYLOAD_SIZE);
    std::memcpy(packet->data.data(), &payload, ENTITY_DESTROY_PAYLOAD_SIZE);
}

void Coordinator::createTransformSnapshotPacket(
    common::protocol::Packet* packet,
    uint32_t worldTick,
    const std::vector<std::pair<uint32_t, protocol::ComponentTransform>>& transforms)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT);
    packet->header.flags = 0;
    
    // Calculate total size: world_tick (4) + entity_count (2) + entities data
    const uint16_t entityCount = static_cast<uint16_t>(transforms.size());
    const size_t dataSize = sizeof(uint32_t) + sizeof(uint16_t) + 
                            (entityCount * (sizeof(uint32_t) + sizeof(protocol::ComponentTransform)));
    
    packet->data.resize(dataSize);
    uint8_t* ptr = packet->data.data();
    
    // Write world tick
    std::memcpy(ptr, &worldTick, sizeof(worldTick));
    ptr += sizeof(worldTick);
    
    // Write entity count
    std::memcpy(ptr, &entityCount, sizeof(entityCount));
    ptr += sizeof(entityCount);
    
    // Write each entity's data
    for (const auto& [entityId, transform] : transforms) {
        std::memcpy(ptr, &entityId, sizeof(entityId));
        ptr += sizeof(entityId);
        std::memcpy(ptr, &transform, sizeof(transform));
        ptr += sizeof(transform);
    }
}

void Coordinator::createHealthSnapshotPacket(
    common::protocol::Packet* packet,
    uint32_t worldTick,
    const std::vector<std::pair<uint32_t, protocol::ComponentHealth>>& healthData)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT);
    packet->header.flags = 0;
    
    // Calculate total size: world_tick (4) + entity_count (2) + entities data
    const uint16_t entityCount = static_cast<uint16_t>(healthData.size());
    const size_t dataSize = sizeof(uint32_t) + sizeof(uint16_t) + 
                            (entityCount * (sizeof(uint32_t) + sizeof(protocol::ComponentHealth)));
    
    packet->data.resize(dataSize);
    uint8_t* ptr = packet->data.data();
    
    // Write world tick
    std::memcpy(ptr, &worldTick, sizeof(worldTick));
    ptr += sizeof(worldTick);
    
    // Write entity count
    std::memcpy(ptr, &entityCount, sizeof(entityCount));
    ptr += sizeof(entityCount);
    
    // Write each entity's data
    for (const auto& [entityId, health] : healthData) {
        std::memcpy(ptr, &entityId, sizeof(entityId));
        ptr += sizeof(entityId);
        std::memcpy(ptr, &health, sizeof(health));
        ptr += sizeof(health);
    }
}

void Coordinator::createWeaponSnapshotPacket(
    common::protocol::Packet* packet,
    uint32_t worldTick,
    const std::vector<std::pair<uint32_t, protocol::ComponentWeapon>>& weaponData)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT);
    packet->header.flags = 0;
    
    // Calculate total size: world_tick (4) + entity_count (2) + entities data
    const uint16_t entityCount = static_cast<uint16_t>(weaponData.size());
    const size_t dataSize = sizeof(uint32_t) + sizeof(uint16_t) + 
                            (entityCount * (sizeof(uint32_t) + sizeof(protocol::ComponentWeapon)));
    
    packet->data.resize(dataSize);
    uint8_t* ptr = packet->data.data();
    
    // Write world tick
    std::memcpy(ptr, &worldTick, sizeof(worldTick));
    ptr += sizeof(worldTick);
    
    // Write entity count
    std::memcpy(ptr, &entityCount, sizeof(entityCount));
    ptr += sizeof(entityCount);
    
    // Write each entity's data
    for (const auto& [entityId, weapon] : weaponData) {
        std::memcpy(ptr, &entityId, sizeof(entityId));
        ptr += sizeof(entityId);
        std::memcpy(ptr, &weapon, sizeof(weapon));
        ptr += sizeof(weapon);
    }
}

void Coordinator::createAnimationSnapshotPacket(
    common::protocol::Packet* packet,
    uint32_t worldTick,
    const std::vector<std::pair<uint32_t, protocol::ComponentAnimation>>& animationData)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT);
    packet->header.flags = 0;
    
    // Calculate total size: world_tick (4) + entity_count (2) + entities data
    const uint16_t entityCount = static_cast<uint16_t>(animationData.size());
    const size_t dataSize = sizeof(uint32_t) + sizeof(uint16_t) + 
                            (entityCount * (sizeof(uint32_t) + sizeof(protocol::ComponentAnimation)));
    
    packet->data.resize(dataSize);
    uint8_t* ptr = packet->data.data();
    
    // Write world tick
    std::memcpy(ptr, &worldTick, sizeof(worldTick));
    ptr += sizeof(worldTick);
    
    // Write entity count
    std::memcpy(ptr, &entityCount, sizeof(entityCount));
    ptr += sizeof(entityCount);
    
    // Write each entity's data
    for (const auto& [entityId, animation] : animationData) {
        std::memcpy(ptr, &entityId, sizeof(entityId));
        ptr += sizeof(entityId);
        std::memcpy(ptr, &animation, sizeof(animation));
        ptr += sizeof(animation);
    }
}

void Coordinator::createPlayerHitPacket(
    common::protocol::Packet* packet,
    uint32_t playerId,
    uint32_t attackerId,
    uint8_t damage,
    uint8_t remainingHealth,
    uint8_t remainingShield,
    int16_t hitPosX,
    int16_t hitPosY)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_HIT);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(PLAYER_HIT_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &playerId, sizeof(playerId));
    ptr += sizeof(playerId);
    std::memcpy(ptr, &attackerId, sizeof(attackerId));
    ptr += sizeof(attackerId);
    std::memcpy(ptr, &damage, sizeof(damage));
    ptr += sizeof(damage);
    std::memcpy(ptr, &remainingHealth, sizeof(remainingHealth));
    ptr += sizeof(remainingHealth);
    std::memcpy(ptr, &remainingShield, sizeof(remainingShield));
    ptr += sizeof(remainingShield);
    std::memcpy(ptr, &hitPosX, sizeof(hitPosX));
    ptr += sizeof(hitPosX);
    std::memcpy(ptr, &hitPosY, sizeof(hitPosY));
}

void Coordinator::createPlayerDeathPacket(
    common::protocol::Packet* packet,
    uint32_t playerId,
    uint32_t killerId,
    uint32_t scoreBeforeDeath,
    int16_t deathPosX,
    int16_t deathPosY)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_DEATH);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(PLAYER_DEATH_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &playerId, sizeof(playerId));
    ptr += sizeof(playerId);
    std::memcpy(ptr, &killerId, sizeof(killerId));
    ptr += sizeof(killerId);
    std::memcpy(ptr, &scoreBeforeDeath, sizeof(scoreBeforeDeath));
    ptr += sizeof(scoreBeforeDeath);
    std::memcpy(ptr, &deathPosX, sizeof(deathPosX));
    ptr += sizeof(deathPosX);
    std::memcpy(ptr, &deathPosY, sizeof(deathPosY));
}

void Coordinator::createScoreUpdatePacket(
    common::protocol::Packet* packet,
    uint32_t playerId,
    uint32_t newScore,
    int16_t scoreDelta,
    uint8_t reason)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_SCORE_UPDATE);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(SCORE_UPDATE_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &playerId, sizeof(playerId));
    ptr += sizeof(playerId);
    std::memcpy(ptr, &newScore, sizeof(newScore));
    ptr += sizeof(newScore);
    std::memcpy(ptr, &scoreDelta, sizeof(scoreDelta));
    ptr += sizeof(scoreDelta);
    std::memcpy(ptr, &reason, sizeof(reason));
}

void Coordinator::createPowerupPickupPacket(
    common::protocol::Packet* packet,
    uint32_t playerId,
    uint32_t powerupId,
    uint8_t powerupType,
    uint8_t duration)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_POWER_PICKUP);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(POWER_PICKUP_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &playerId, sizeof(playerId));
    ptr += sizeof(playerId);
    std::memcpy(ptr, &powerupId, sizeof(powerupId));
    ptr += sizeof(powerupId);
    std::memcpy(ptr, &powerupType, sizeof(powerupType));
    ptr += sizeof(powerupType);
    std::memcpy(ptr, &duration, sizeof(duration));
}

void Coordinator::createWeaponFirePacket(
    common::protocol::Packet* packet,
    uint32_t shooterId,
    uint32_t projectileId,
    int16_t originX,
    int16_t originY,
    int16_t directionX,
    int16_t directionY,
    uint8_t weaponType)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(WEAPON_FIRE_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &shooterId, sizeof(shooterId));
    ptr += sizeof(shooterId);
    std::memcpy(ptr, &projectileId, sizeof(projectileId));
    ptr += sizeof(projectileId);
    std::memcpy(ptr, &originX, sizeof(originX));
    ptr += sizeof(originX);
    std::memcpy(ptr, &originY, sizeof(originY));
    ptr += sizeof(originY);
    std::memcpy(ptr, &directionX, sizeof(directionX));
    ptr += sizeof(directionX);
    std::memcpy(ptr, &directionY, sizeof(directionY));
    ptr += sizeof(directionY);
    std::memcpy(ptr, &weaponType, sizeof(weaponType));
}

void Coordinator::createVisualEffectPacket(
    common::protocol::Packet* packet,
    uint8_t effectType,
    int16_t posX,
    int16_t posY,
    uint16_t durationMs,
    uint8_t scale,
    uint8_t colorR,
    uint8_t colorG,
    uint8_t colorB)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_VISUAL_EFFECT);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(VISUAL_EFFECT_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &effectType, sizeof(effectType));
    ptr += sizeof(effectType);
    std::memcpy(ptr, &posX, sizeof(posX));
    ptr += sizeof(posX);
    std::memcpy(ptr, &posY, sizeof(posY));
    ptr += sizeof(posY);
    std::memcpy(ptr, &durationMs, sizeof(durationMs));
    ptr += sizeof(durationMs);
    std::memcpy(ptr, &scale, sizeof(scale));
    ptr += sizeof(scale);
    std::memcpy(ptr, &colorR, sizeof(colorR));
    ptr += sizeof(colorR);
    std::memcpy(ptr, &colorG, sizeof(colorG));
    ptr += sizeof(colorG);
    std::memcpy(ptr, &colorB, sizeof(colorB));
}

void Coordinator::createAudioEffectPacket(
    common::protocol::Packet* packet,
    uint8_t effectType,
    int16_t posX,
    int16_t posY,
    uint8_t volume,
    uint8_t pitch)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_AUDIO_EFFECT);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(AUDIO_EFFECT_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &effectType, sizeof(effectType));
    ptr += sizeof(effectType);
    std::memcpy(ptr, &posX, sizeof(posX));
    ptr += sizeof(posX);
    std::memcpy(ptr, &posY, sizeof(posY));
    ptr += sizeof(posY);
    std::memcpy(ptr, &volume, sizeof(volume));
    ptr += sizeof(volume);
    std::memcpy(ptr, &pitch, sizeof(pitch));
}

void Coordinator::createParticleSpawnPacket(
    common::protocol::Packet* packet,
    uint16_t particleSystemId,
    int16_t posX,
    int16_t posY,
    int16_t velocityX,
    int16_t velocityY,
    uint16_t particleCount,
    uint16_t lifetimeMs,
    uint8_t colorStartR,
    uint8_t colorStartG,
    uint8_t colorStartB,
    uint8_t colorEndR,
    uint8_t colorEndG,
    uint8_t colorEndB)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_PARTICLE_SPAWN);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(PARTICLE_SPAWN_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &particleSystemId, sizeof(particleSystemId));
    ptr += sizeof(particleSystemId);
    std::memcpy(ptr, &posX, sizeof(posX));
    ptr += sizeof(posX);
    std::memcpy(ptr, &posY, sizeof(posY));
    ptr += sizeof(posY);
    std::memcpy(ptr, &velocityX, sizeof(velocityX));
    ptr += sizeof(velocityX);
    std::memcpy(ptr, &velocityY, sizeof(velocityY));
    ptr += sizeof(velocityY);
    std::memcpy(ptr, &particleCount, sizeof(particleCount));
    ptr += sizeof(particleCount);
    std::memcpy(ptr, &lifetimeMs, sizeof(lifetimeMs));
    ptr += sizeof(lifetimeMs);
    std::memcpy(ptr, &colorStartR, sizeof(colorStartR));
    ptr += sizeof(colorStartR);
    std::memcpy(ptr, &colorStartG, sizeof(colorStartG));
    ptr += sizeof(colorStartG);
    std::memcpy(ptr, &colorStartB, sizeof(colorStartB));
    ptr += sizeof(colorStartB);
    std::memcpy(ptr, &colorEndR, sizeof(colorEndR));
    ptr += sizeof(colorEndR);
    std::memcpy(ptr, &colorEndG, sizeof(colorEndG));
    ptr += sizeof(colorEndG);
    std::memcpy(ptr, &colorEndB, sizeof(colorEndB));
}

void Coordinator::createGameStartPacket(
    common::protocol::Packet* packet,
    uint32_t gameInstanceId,
    uint8_t playerCount,
    const uint32_t playerIds[4],
    uint8_t levelId,
    uint8_t difficulty)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_START);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(GAME_START_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &gameInstanceId, sizeof(gameInstanceId));
    ptr += sizeof(gameInstanceId);
    std::memcpy(ptr, &playerCount, sizeof(playerCount));
    ptr += sizeof(playerCount);
    std::memcpy(ptr, playerIds, sizeof(uint32_t) * 4);
    ptr += sizeof(uint32_t) * 4;
    std::memcpy(ptr, &levelId, sizeof(levelId));
    ptr += sizeof(levelId);
    std::memcpy(ptr, &difficulty, sizeof(difficulty));
}

void Coordinator::createGameEndPacket(
    common::protocol::Packet* packet,
    uint8_t endReason,
    const uint32_t finalScores[4],
    uint8_t winnerId,
    uint32_t playTime)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_END);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(GAME_END_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &endReason, sizeof(endReason));
    ptr += sizeof(endReason);
    std::memcpy(ptr, finalScores, sizeof(uint32_t) * 4);
    ptr += sizeof(uint32_t) * 4;
    std::memcpy(ptr, &winnerId, sizeof(winnerId));
    ptr += sizeof(winnerId);
    std::memcpy(ptr, &playTime, sizeof(playTime));
}

void Coordinator::createLevelCompletePacket(
    common::protocol::Packet* packet,
    uint8_t completedLevel,
    uint8_t nextLevel,
    uint32_t bonusScore,
    uint16_t completionTime)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_COMPLETE);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(LEVEL_COMPLETE_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &completedLevel, sizeof(completedLevel));
    ptr += sizeof(completedLevel);
    std::memcpy(ptr, &nextLevel, sizeof(nextLevel));
    ptr += sizeof(nextLevel);
    std::memcpy(ptr, &bonusScore, sizeof(bonusScore));
    ptr += sizeof(bonusScore);
    std::memcpy(ptr, &completionTime, sizeof(completionTime));
}

void Coordinator::createLevelStartPacket(
    common::protocol::Packet* packet,
    uint8_t levelId,
    const char* levelName,
    uint16_t estimatedDuration)
{
    // Set packet type with reliable flag
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_START);
    packet->header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
    
    // Prepare and write payload
    packet->data.resize(LEVEL_START_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &levelId, sizeof(levelId));
    ptr += sizeof(levelId);
    
    // Copy level name (max 32 chars, null-terminated)
    char levelNameBuffer[32] = {0};
    std::strncpy(levelNameBuffer, levelName, 31);
    std::memcpy(ptr, levelNameBuffer, 32);
    ptr += 32;
    
    std::memcpy(ptr, &estimatedDuration, sizeof(estimatedDuration));
}

void Coordinator::createForceStatePacket(
    common::protocol::Packet* packet,
    uint32_t forceEntityId,
    uint32_t parentShipId,
    uint8_t attachmentPoint,
    uint8_t powerLevel,
    uint8_t chargePercentage,
    uint8_t isFiring)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_FORCE_STATE);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(FORCE_STATE_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &forceEntityId, sizeof(forceEntityId));
    ptr += sizeof(forceEntityId);
    std::memcpy(ptr, &parentShipId, sizeof(parentShipId));
    ptr += sizeof(parentShipId);
    std::memcpy(ptr, &attachmentPoint, sizeof(attachmentPoint));
    ptr += sizeof(attachmentPoint);
    std::memcpy(ptr, &powerLevel, sizeof(powerLevel));
    ptr += sizeof(powerLevel);
    std::memcpy(ptr, &chargePercentage, sizeof(chargePercentage));
    ptr += sizeof(chargePercentage);
    std::memcpy(ptr, &isFiring, sizeof(isFiring));
}

void Coordinator::createAIStatePacket(
    common::protocol::Packet* packet,
    uint32_t entityId,
    uint8_t currentState,
    uint8_t behaviorType,
    uint32_t targetEntityId,
    int16_t waypointX,
    int16_t waypointY,
    uint16_t stateTimer)
{
    // Set packet type
    packet->header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_STATE);
    packet->header.flags = 0;
    
    // Prepare and write payload
    packet->data.resize(AI_STATE_PAYLOAD_SIZE);
    uint8_t* ptr = packet->data.data();
    
    std::memcpy(ptr, &entityId, sizeof(entityId));
    ptr += sizeof(entityId);
    std::memcpy(ptr, &currentState, sizeof(currentState));
    ptr += sizeof(currentState);
    std::memcpy(ptr, &behaviorType, sizeof(behaviorType));
    ptr += sizeof(behaviorType);
    std::memcpy(ptr, &targetEntityId, sizeof(targetEntityId));
    ptr += sizeof(targetEntityId);
    std::memcpy(ptr, &waypointX, sizeof(waypointX));
    ptr += sizeof(waypointX);
    std::memcpy(ptr, &waypointY, sizeof(waypointY));
    ptr += sizeof(waypointY);
    std::memcpy(ptr, &stateTimer, sizeof(stateTimer));
}
