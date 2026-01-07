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
}
