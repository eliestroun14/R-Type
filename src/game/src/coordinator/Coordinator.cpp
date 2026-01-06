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
    this->_engine->playWeaponFireSound(payload.weapon_type);

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
    this->_engine->spawnVisualEffect(
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
