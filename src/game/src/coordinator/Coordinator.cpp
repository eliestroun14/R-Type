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

void Coordinator::processCLientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
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
                    // TODO: handle entity destroy
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT):
                if (PacketManager::assertTransformSnapshot(packet)) {
                    this->handlePacketTransformSnapshot(packet);
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT):
                if (PacketManager::assertHealthSnapshot(packet)) {
                    // TODO: handle health snapshot
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_SNAPSHOT):
                if (PacketManager::assertWeaponSnapshot(packet)) {
                    // TODO: handle weapon snapshot
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_ANIMATION_SNAPSHOT):
                if (PacketManager::assertAnimationSnapshot(packet)) {
                    // TODO: handle animation snapshot
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_COMPONENT_REMOVE):
                if (PacketManager::assertComponentRemove(packet)) {
                    // TODO: handle component remove
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_TRANSFORM_SNAPSHOT_DELTA):
                if (PacketManager::assertTransformSnapshotDelta(packet)) {
                    // TODO: handle transform snapshot delta
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEALTH_SNAPSHOT_DELTA):
                if (PacketManager::assertHealthSnapshotDelta(packet)) {
                    // TODO: handle health snapshot delta
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_HIT):
                if (PacketManager::assertPlayerHit(packet)) {
                    // TODO: handle player hit
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_DEATH):
                if (PacketManager::assertPlayerDeath(packet)) {
                    // TODO: handle player death
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_SCORE_UPDATE):
                if (PacketManager::assertScoreUpdate(packet)) {
                    // TODO: handle score update
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_POWER_PICKUP):
                if (PacketManager::assertPowerupPickup(packet)) {
                    // TODO: handle powerup pickup
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_WEAPON_FIRE):
                if (PacketManager::assertWeaponFire(packet)) {
                    // TODO: handle weapon fire
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_VISUAL_EFFECT):
                if (PacketManager::assertVisualEffect(packet)) {
                    // TODO: handle visual effect
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_AUDIO_EFFECT):
                if (PacketManager::assertAudioEffect(packet)) {
                    // TODO: handle audio effect
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_PARTICLE_SPAWN):
                if (PacketManager::assertParticleSpawn(packet)) {
                    // TODO: handle particle spawn
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
                    // TODO: handle level complete
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_START):
                if (PacketManager::assertLevelStart(packet)) {
                    // TODO: handle level start
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_FORCE_STATE):
                if (PacketManager::assertForceState(packet)) {
                    // TODO: handle force state
                }
                break;
            case static_cast<uint8_t>(protocol::PacketTypes::TYPE_AI_STATE):
                if (PacketManager::assertAIState(packet)) {
                    // TODO: handle AI state
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


void Coordinator::handlePacketTransformSnapshot(const common::protocol::Packet &packet)
{
    // packet.data format:
    // - world_tick (4 bytes)
    // - entity_count (2 bytes)
    // - [entity_id (4 bytes) + ComponentTransform (8 bytes)] * entity_count
    
    const auto& data = packet.data;
    
    // Minimum size: 6 bytes (world_tick + entity_count)
    if (data.size() < 6 || (data.size() - 6) % 12 != 0) {
        LOG_ERROR_CAT("Coordinator",
            "TransformSnapshot: invalid payload size %zu",
            data.size());
        return;
    }
    
    const uint8_t* ptr = data.data();
    
    uint32_t world_tick = 0;
    std::memcpy(&world_tick, ptr, sizeof(world_tick));
    ptr += sizeof(world_tick);
    
    uint16_t entity_count = 0;
    std::memcpy(&entity_count, ptr, sizeof(entity_count));
    ptr += sizeof(entity_count);
    
    // Validate total size
    if (data.size() != 6 + (entity_count * 12)) {
        LOG_ERROR_CAT("Coordinator",
            "TransformSnapshot: size mismatch (%zu != %zu)",
            data.size(), 6 + (entity_count * 12));
        return;
    }

    for (uint16_t i = 0; i < entity_count; ++i) {
        uint32_t entity_id = 0;
        protocol::ComponentTransform net;

        std::memcpy(&entity_id, ptr, sizeof(entity_id));
        ptr += sizeof(entity_id);

        std::memcpy(&net, ptr, sizeof(net));
        ptr += sizeof(net);

        Entity entity = Entity::fromId(entity_id);
        if (!this->_engine->isAlive(entity))
            continue;

        auto& opt = this->_engine->getComponentEntity<Transform>(entity);
        if (!opt.has_value())
            continue;

        Transform& tr = opt.value();

        tr.x = static_cast<float>(net.pos_x);
        tr.y = static_cast<float>(net.pos_y);

        tr.rotation = (static_cast<float>(net.rotation) / 65535.f) * 360.f;

        tr.scale = static_cast<float>(net.scale) / 1000.f;
    }
}

void Coordinator::handlePacketHealthSnapshot(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketWeaponSnapshot(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketAnimationSnapshot(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketComponentRemove(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketTransformSnapshotDelta(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketHealthSnapshotDelta(const common::protocol::Packet &packet)
{
}

void Coordinator::handlePacketPlayerHit(const common::protocol::Packet &packet)
{
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
}
