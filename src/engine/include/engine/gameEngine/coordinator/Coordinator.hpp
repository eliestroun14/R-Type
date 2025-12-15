/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Coordinator
*/

#ifndef COORDINATOR_HPP_
#define COORDINATOR_HPP_

#include <memory>
#include <string>

#include <engine/gameEngine/coordinator/ecs/system/SystemManager.hpp>
#include <engine/gameEngine/coordinator/ecs/entity/EntityManager.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/render/RenderManager.hpp>
#include <engine/gameEngine/coordinator/network/PacketManager.hpp>
#include <common/protocol/Protocol.hpp>
#include <cstring>
#include <common/constants/defines.hpp>
#include <common/constants/render/Assets.hpp>
#include <engine/utils/Logger.hpp>
#include <common/protocol/Payload.hpp>
#include <engine/gameEngine/coordinator/render/SpriteAllocator.hpp>


class Coordinator {
    public:
        struct ProjectileInfo {
            uint32_t id;
            float x;
            float y;
            float velX;
            float velY;
        };

        Coordinator() {
            std::cout << "Coordinator constructor START" << std::endl;
            std::cout << "Coordinator constructor END" << std::endl;
        }
        ~Coordinator() = default;

        // ==============================================================
        //                          Initialization
        // ==============================================================

        void init()
        {
            this->_entityManager = std::make_unique<EntityManager>();
            this->_systemManager = std::make_unique<SystemManager>();
            this->_entityManager->setSystemManager(this->_systemManager.get());
        }

        void initRender()  // Nouvelle méthode
        {
            this->_renderManager = std::make_unique<RenderManager>();
            this->_renderManager->init();
        }

        // ==============================================================
        //                              Entity
        // ==============================================================

        Entity createEntity(const std::string &entityName)
        {
            return this->_entityManager->spawnEntity(entityName);
        }

        /**
         * @brief Create a player entity (local or remote)
         * @param playerId The network ID of the player
         * @param isPlayable Whether this is the local playable player (true) or remote (false)
         * @return The created entity
         */
        Entity createPlayerEntity(uint32_t playerId, bool isPlayable)
        {
            Entity player = this->createEntity("Player_" + std::to_string(playerId));
            
            // All players have InputComponent
            this->addComponent<InputComponent>(player, InputComponent(playerId));
            
            // Only the local player has Playable
            if (isPlayable) {
                this->addComponent<Playable>(player, Playable());
            }
            return player;
        }

        void destroyEntity(Entity &e)
        {
            this->_entityManager->killEntity(e);
        }

        bool isAlive(Entity const &entity) const
        {
            return this->_entityManager->isAlive(entity);
        }

        std::string getEntityName(Entity const &entity) const
        {
            return this->_entityManager->getEntityName(entity);
        }

        // ==============================================================
        //                           Components
        // ==============================================================

        template<class Component>
        ComponentManager<Component> &registerComponent()
        {
            return this->_entityManager->registerComponent<Component>();
        }

        template <class Component>
        typename ComponentManager<Component>::referenceType
        addComponent(Entity const &entity, Component component)
        {
            return this->_entityManager->template addComponent<Component>(entity, std::move(component));
        }

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType
        emplaceComponent(Entity const &entity, Params&&... params)
        {
            return this->_entityManager->template emplaceComponent<Component>(entity, std::forward<Params>(params)...);
        }

        template<class Component>
        void removeComponent(Entity const &entity)
        {
            this->_entityManager->removeComponent<Component>(entity);
        }

        template<typename Component>
        ComponentManager<Component> &getComponents() const
        {
            return this->_entityManager->getComponents<Component>();
        }

        template<typename Component>
        std::optional<Component> &getComponentEntity(Entity const &entity) const
        {
            return this->_entityManager->getComponent<Component>(entity);
        }

        // ==============================================================
        //                              Systems
        // ==============================================================

        template<class System, class... Params>
        System &registerSystem(Params &&... params)
        {
            return this->_systemManager->addSystem<System>(std::forward<Params>(params)...);
        }

        template<class System>
        System &getSystem() const
        {
            return this->_systemManager->getSystem<System>();
        }

        template<class System>
        void removeSystem()
        {
            this->_systemManager->deleteSystem<System>();
        }

        void updateSystems(float dt)
        {
            this->_systemManager->updateAll(dt);
        }

        void onCreateSystems()
        {
            _systemManager->onCreateAll();
        }

        void onDestroySystems()
        {
            _systemManager->onDestroyAll();
        }

        template <class S, class... Components>
        void setSystemSignature()
        {
            Signature sig{};
            (sig.set(_entityManager->template getComponentTypeId<Components>()), ...);
            _systemManager->setSignature<S>(sig);
        }

        // ==============================================================
        //                              Input
        // ==============================================================

        /**
         * @brief Sets the local player entity for input handling (Client-side only).
         * * Should be called after the local player entity is created.
         * * On the server, this does nothing (no RenderManager).
         *
         * @param localPlayerEntity The entity ID of the local player
         * @param playerId The player ID
         */
        void setLocalPlayerEntity(Entity localPlayerEntity, uint32_t playerId = 0)
        {
            if (this->_renderManager) {
                this->_renderManager->setLocalPlayer(*this, localPlayerEntity);
            }
        }

        /**
         * @brief Updates input for a specific player entity.
         * * Used on the server to inject player inputs from network packets.
         * * Used on the client to update the local player's input component.
         *
         * @param entity The player entity to update
         * @param playerId The player ID
         * @param action The game action to set
         * @param isActive Whether the action is active (pressed) or not
         */
        void setPlayerInputAction(Entity entity, uint32_t playerId, GameAction action, bool isActive)
        {
            auto& inputComp = this->_entityManager->getComponent<InputComponent>(entity);
            if (inputComp.has_value()) {
                inputComp.value().activeActions[action] = isActive;
            }
        }

        /**
         * @brief Checks if a specific action is active for a player entity.
         *
         * @param entity The player entity
         * @param action The game action to check
         * @return true If the action is active for this player
         */
        bool isPlayerActionActive(Entity entity, GameAction action) const
        {
            const auto& inputComp = this->_entityManager->getComponent<InputComponent>(entity);
            if (!inputComp.has_value()) return false;
            auto it = inputComp.value().activeActions.find(action);
            return it != inputComp.value().activeActions.end() && it->second;
        }

        /**
         * @brief Retrieves the active actions map for a player entity.
         *
         * @param entity The player entity
         * @return Reference to the active actions map for this player
         */
        std::map<GameAction, bool>& getPlayerActiveActions(Entity entity)
        {
            auto& inputComp = this->_entityManager->getComponent<InputComponent>(entity);
            return inputComp.value().activeActions;  // Will throw if empty, that's intentional
        }

        // ==============================================================
        //                              Render
        // ==============================================================

        /**
         * @brief Polls all pending input events from the window and updates InputComponent.
         * * Retrieves events (keyboard, mouse, window close) from SFML
         * and updates the InputComponent of the local player entity.
         * Updates the mouse position.
         */
        void processInput()
        {
            this->_renderManager->processInput();
        }

        /**
         * @brief Clears the window buffer to prepare for new frame drawing.
         * * Should be called at the start of each frame before any drawing.
         */
        void beginFrame()
        {
            this->_renderManager->beginFrame();
        }

        /**
         * @brief Displays the rendered frame.
         * * Should be called at the end of the frame after all drawing is done.
         */
        void render()
        {
            this->_renderManager->render();
        }

        /**
         * @brief Checks if a specific game action is currently active (pressed).
         * * @param action The logical action to check (e.g., GameAction::SHOOT).
         * @return true If the key corresponding to the action is currently held down.
         * @return false Otherwise.
         */
        bool isActionActive(GameAction action) const
        {
            return this->_renderManager->isActionActive(action);
        }

        /**
         * @brief Checks if a specific game action was just pressed (edge detection).
         * * @param action The logical action to check.
         * @return true If the action is currently pressed AND was not pressed last frame.
         * @return false Otherwise.
         */
        bool isActionJustPressed(GameAction action) const
        {
            return this->_renderManager->isActionJustPressed(action);
        }

        /**
         * @brief Get the full map of active actions.
         * * @return const std::map<GameAction, bool>& Reference to the internal action map.
         */
        std::map<GameAction, bool>& getActiveActions()
        {
            return this->_renderManager->getActiveActions();
        }

        /**
         * @brief Checks if the game window is currently open.
         * * @return true If the window is open and running.
         * @return false If the window has been closed.
         */
        bool isOpen()
        {
            return this->_renderManager->isOpen();
        }

        /**
         * @brief Get the sf::Texture of sprite in function of his id.
         * * @return const std::shared_ptr<sf::Texture> to the sf::Texture.
         * * @param id The id reference to the enum Assets
         */
        std::shared_ptr<sf::Texture> getTexture(Assets id) const {
            return this->_renderManager->getTexture(id);
        }

        /**
         * @brief Get the sf::RenderWindow of the game.
         * * @return const sf::RenderWindow& Reference to the window.
         */
        sf::RenderWindow& getWindow() {
            return this->_renderManager->getWindow();
        }

        /**
         * @brief Get the Factor scale for adjust velocity in function of the window size.
         * * @return const float of the factor.
         */
        float getScaleFactor() const
        {
            return this->_renderManager->getScaleFactor();
        }

        /**
         * @brief Retrieves the current mouse position relative to the window.
         * * @return sf::Vector2i Coordinates (x, y) of the mouse.
         */
        sf::Vector2i getMousePosition() const
        {
            return this->_renderManager->getMousePosition();
        }

        void processServerPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
        {
            // TODO
            for (const auto& packet : packetsToProcess) {

                if (PacketManager::assertPlayerInput(packet)) {

                    handlePlayerInputPacket(packet, elapsedMs);
                }
            }
        }

        void processCLientPackets(const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
        {
            for (const auto& packet : packetsToProcess) {
                // Check packet type first, then validate
                uint8_t packetType = packet.header.packet_type;

                switch (packetType) {
                    case static_cast<uint8_t>(protocol::PacketTypes::TYPE_FULL_GAME_STATE):
                        // Handle full game state snapshot
                        handleFullGameStatePacket(packet, elapsedMs);
                        break;
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

        void buildSeverPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
        {
            // Build full game state snapshot every tick
            // This contains ALL entities and their Transform, Velocity, Health components
            buildFullGameStatePacket(outgoingPackets, elapsedMs);
            LOG_DEBUG_CAT("Coordinator", "Server built %zu outgoing packets", outgoingPackets.size());
        }

        void buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
        {
            // Client sends input packets (already handled by PlayerSystem)
            // For MVP, we keep this simple - no client-side prediction
        }

        void handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs)
        {
            // Parse the player input packet
            auto parsed = PacketManager::parsePlayerInput(packet);
            if (!parsed.has_value()) {
                return;  // Invalid packet, ignore
            }

            // Find the entity for this player by iterating over all InputComponents
            auto& inputComponents = this->_entityManager->getComponents<InputComponent>();
            for (size_t entityId = 0; entityId < inputComponents.size(); ++entityId) {
                auto& input = inputComponents[entityId];
                if (input.has_value() && input->playerId == parsed->playerId) {
                    Entity entity = Entity::fromId(entityId);
                    // Update all input actions
                    for (const auto& [action, isPressed] : parsed->actions) {
                        this->setPlayerInputAction(entity, parsed->playerId, action, isPressed);
                    }
                    break;
                }
            }
        }

        void handlePacketCreateEntity(const common::protocol::Packet& packet)
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
            Entity newEntity = this->createEntity("Entity_" + std::to_string(payload.entity_id));

            // Add type-specific components
            switch (payload.entity_type) {
                case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER): {
                    Assets spriteAsset = _playerSpriteAllocator.allocate(payload.entity_id);

                    addComponent<Transform>(newEntity, Transform(static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), 0.f, 2.5f));
                    addComponent<Velocity>(newEntity, Velocity(static_cast<float>(payload.initial_velocity_x), static_cast<float>(payload.initial_velocity_y)));
                    addComponent<Health>(newEntity, Health(payload.initial_health, payload.initial_health));
                    addComponent<Sprite>(newEntity, Sprite(spriteAsset, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 15)));
                    addComponent<Animation>(newEntity, Animation(33, 15, 2, 0.f, 0.1f, 2, 2, true));
                    addComponent<HitBox>(newEntity, HitBox());
                    addComponent<Weapon>(newEntity, Weapon(200, 0, 10, ProjectileType::MISSILE));
                    addComponent<InputComponent>(newEntity, InputComponent(payload.entity_id));

                    // If this is the playable player, set it as local player
                    if (payload.is_playable) {
                        addComponent<Playable>(newEntity, Playable());
                        this->setLocalPlayerEntity(newEntity, payload.entity_id);
                        LOG_INFO_CAT("Coordinator", "Local player created with ID %u", payload.entity_id);
                    }
                    break;
                }
                case static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_ENEMY): {
                    addComponent<Transform>(newEntity, Transform(static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), 0.f, 2.0f));
                    addComponent<Velocity>(newEntity, Velocity(static_cast<float>(payload.initial_velocity_x), static_cast<float>(payload.initial_velocity_y)));
                    addComponent<Health>(newEntity, Health(payload.initial_health, payload.initial_health));
                    addComponent<Sprite>(newEntity, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, BASE_ENEMY_SPRITE_WIDTH, BASE_ENEMY_SPRITE_HEIGHT)));
                    addComponent<HitBox>(newEntity, HitBox());
                    addComponent<Weapon>(newEntity, Weapon(BASE_ENEMY_WEAPON_FIRE_RATE, 0, BASE_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
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
                    addComponent<Transform>(newEntity, Transform(static_cast<float>(payload.position_x), static_cast<float>(payload.position_y), 0.f, 1.f));
                    addComponent<Velocity>(newEntity, Velocity(static_cast<float>(payload.initial_velocity_x), static_cast<float>(payload.initial_velocity_y)));
                    addComponent<Sprite>(newEntity, Sprite(DEFAULT_BULLET, ZIndex::IS_GAME, sf::IntRect(0, 0, 16, 16)));
                    addComponent<HitBox>(newEntity, HitBox());
                    addComponent<Projectile>(newEntity, Projectile(Entity::fromId(payload.entity_id), isPlayerProjectile, 10));

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

    void handlePacketTransformSnapshot(const common::protocol::Packet& packet)
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
            if (!this->isAlive(entity))
                continue;

            auto& opt = this->getComponentEntity<Transform>(entity);
            if (!opt.has_value())
                continue;

            Transform& tr = opt.value();

            tr.x = static_cast<float>(net.pos_x);
            tr.y = static_cast<float>(net.pos_y);

            tr.rotation = (static_cast<float>(net.rotation) / 65535.f) * 360.f;

            tr.scale = static_cast<float>(net.scale) / 1000.f;
        }
    }

    void handleGameStart(const common::protocol::Packet& packet)
    {
        if (packet.data.size() != GAME_START_PAYLOAD_SIZE) {
            LOG_ERROR_CAT("Coordinator", "GameStart: invalid size %zu", packet.data.size());
            return;
        }

        _gameRunning = true;
    }

    void handleGameEnd(const common::protocol::Packet& packet)
    {
        if (packet.data.size() != GAME_END_PAYLOAD_SIZE) {
            LOG_ERROR_CAT("Coordinator", "GameEnd: invalid size %zu", packet.data.size());
            return;
        }

        _gameRunning = false;
    }

    /**
     * @brief Builds a full game state snapshot packet containing all entities and components
     * 
     * This creates a TYPE_FULL_GAME_STATE packet with all active entities and their components.
     * The packet contains Transform, Velocity, Health, Sprite, and Animation data for each entity.
     * 
     * @param outgoingPackets Vector to append the packet to
     * @param elapsedMs Current elapsed time in milliseconds (used as world tick)
     */
    void buildFullGameStatePacket(std::vector<common::protocol::Packet>& outgoingPackets, uint64_t elapsedMs)
    {
        common::protocol::Packet packet;
        packet.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_FULL_GAME_STATE);
        packet.header.flags = 0;  // Not reliable, sent every frame
        packet.header.sequence_number = 0;  // Will be set by network manager
        packet.header.timestamp = static_cast<uint32_t>(elapsedMs);

        // Build payload: world_tick + entity_count + entity_data[]
        std::vector<uint8_t> payload;
        
        // World tick (4 bytes)
        uint32_t world_tick = static_cast<uint32_t>(elapsedMs);
        payload.insert(payload.end(), 
            reinterpret_cast<uint8_t*>(&world_tick),
            reinterpret_cast<uint8_t*>(&world_tick) + sizeof(world_tick));
        
        // Count alive entities with Transform component
        auto& transforms = _entityManager->getComponents<Transform>();
        uint16_t entity_count = 0;
        for (size_t i = 0; i < transforms.size(); ++i) {
            if (transforms[i].has_value()) {
                entity_count++;
            }
        }
        
        // Entity count (2 bytes)
        payload.insert(payload.end(),
            reinterpret_cast<uint8_t*>(&entity_count),
            reinterpret_cast<uint8_t*>(&entity_count) + sizeof(entity_count));
        
        // For each entity, serialize its components
        auto& velocities = _entityManager->getComponents<Velocity>();
        auto& healths = _entityManager->getComponents<Health>();
        auto& sprites = _entityManager->getComponents<Sprite>();
        auto& animations = _entityManager->getComponents<Animation>();
        
        for (size_t entityId = 0; entityId < transforms.size(); ++entityId) {
            if (!transforms[entityId].has_value()) continue;
            
            // Entity ID (4 bytes)
            uint32_t eid = static_cast<uint32_t>(entityId);
            payload.insert(payload.end(),
                reinterpret_cast<uint8_t*>(&eid),
                reinterpret_cast<uint8_t*>(&eid) + sizeof(eid));
            
            // Entity type (1 byte) - default to PLAYER for now, TODO: store entity type
            uint8_t entity_type = static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER);
            payload.push_back(entity_type);
            
            // Component flags (1 byte)
            uint8_t flags = 0x01;  // Always has Transform
            if (velocities[entityId].has_value()) flags |= 0x02;
            if (healths[entityId].has_value()) flags |= 0x04;
            if (sprites[entityId].has_value()) flags |= 0x08;
            if (animations[entityId].has_value()) flags |= 0x10;
            payload.push_back(flags);
            
            // Transform data (8 bytes)
            auto& transform = transforms[entityId].value();
            int16_t pos_x = static_cast<int16_t>(transform.x);
            int16_t pos_y = static_cast<int16_t>(transform.y);
            uint16_t rotation = static_cast<uint16_t>((transform.rotation / 360.0f) * 65535.0f);
            uint16_t scale = static_cast<uint16_t>(transform.scale * 1000.0f);
            
            payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&pos_x), reinterpret_cast<uint8_t*>(&pos_x) + 2);
            payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&pos_y), reinterpret_cast<uint8_t*>(&pos_y) + 2);
            payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&rotation), reinterpret_cast<uint8_t*>(&rotation) + 2);
            payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&scale), reinterpret_cast<uint8_t*>(&scale) + 2);
            
            // Velocity data (4 bytes) if present
            if (velocities[entityId].has_value()) {
                auto& velocity = velocities[entityId].value();
                int16_t vel_x = static_cast<int16_t>(velocity.vx);
                int16_t vel_y = static_cast<int16_t>(velocity.vy);
                payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&vel_x), reinterpret_cast<uint8_t*>(&vel_x) + 2);
                payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&vel_y), reinterpret_cast<uint8_t*>(&vel_y) + 2);
            }
            
            // Health data (2 bytes) if present
            if (healths[entityId].has_value()) {
                auto& health = healths[entityId].value();
                uint8_t current_health = static_cast<uint8_t>(health.currentHealth);
                uint8_t max_health = static_cast<uint8_t>(health.maxHp);
                payload.push_back(current_health);
                payload.push_back(max_health);
            }
            
            // Sprite data (2 bytes) if present
            if (sprites[entityId].has_value()) {
                auto& sprite = sprites[entityId].value();
                uint16_t sprite_id = static_cast<uint16_t>(sprite.assetId);
                payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&sprite_id), reinterpret_cast<uint8_t*>(&sprite_id) + 2);
            }
            
            // Animation data (4 bytes) if present
            if (animations[entityId].has_value()) {
                auto& animation = animations[entityId].value();
                uint16_t animation_frame = static_cast<uint16_t>(animation.currentFrame);
                uint16_t animation_id = 0;  // TODO: store animation ID
                payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&animation_frame), reinterpret_cast<uint8_t*>(&animation_frame) + 2);
                payload.insert(payload.end(), reinterpret_cast<uint8_t*>(&animation_id), reinterpret_cast<uint8_t*>(&animation_id) + 2);
            }
        }
        
        // Set packet data
        packet.data = std::move(payload);
        
        outgoingPackets.push_back(std::move(packet));
        
        LOG_DEBUG_CAT("Coordinator", "Built full game state packet: %u entities, %zu bytes", 
            entity_count, packet.data.size());
    }

    /**
     * @brief Handles a full game state snapshot packet from the server
     * 
     * This updates all entities on the client with the data received from the server.
     * The client does NOT simulate - it only applies the received state and renders.
     * 
     * @param packet The full game state packet
     * @param elapsedMs Current elapsed time in milliseconds
     */
    void handleFullGameStatePacket(const common::protocol::Packet& packet, uint64_t elapsedMs)
    {
        LOG_DEBUG_CAT("Coordinator", "Handling full game state packet (%zu bytes)", packet.data.size());

        const auto& data = packet.data;
        
        // Minimum size: 6 bytes (world_tick + entity_count)
        if (data.size() < 6) {
            LOG_ERROR_CAT("Coordinator", "FullGameState: packet too small (%zu bytes)", data.size());
            return;
        }
        
        const uint8_t* ptr = data.data();
        
        // Read world tick
        uint32_t world_tick = 0;
        std::memcpy(&world_tick, ptr, sizeof(world_tick));
        ptr += sizeof(world_tick);
        
        // Read entity count
        uint16_t entity_count = 0;
        std::memcpy(&entity_count, ptr, sizeof(entity_count));
        ptr += sizeof(entity_count);
        
        LOG_DEBUG_CAT("Coordinator", "Received full game state: tick=%u, entities=%u", world_tick, entity_count);
        
        // Process each entity
        for (uint16_t i = 0; i < entity_count; ++i) {
            // Check if we have enough data
            if (ptr + 6 > data.data() + data.size()) {
                LOG_ERROR_CAT("Coordinator", "FullGameState: truncated entity data at entity %u", i);
                break;
            }
            
            // Read entity ID
            uint32_t entity_id = 0;
            std::memcpy(&entity_id, ptr, sizeof(entity_id));
            ptr += sizeof(entity_id);
            
            // Read entity type
            uint8_t entity_type = *ptr++;
            
            // Read component flags
            uint8_t flags = *ptr++;
            
            Entity entity = Entity::fromId(entity_id);
            
            // If entity doesn't exist yet, skip (it will be created via ENTITY_SPAWN)
            if (!this->isAlive(entity)) {
                // Skip this entity's data based on flags
                if (flags & 0x01) ptr += 8;  // Transform
                if (flags & 0x02) ptr += 4;  // Velocity
                if (flags & 0x04) ptr += 2;  // Health
                if (flags & 0x08) ptr += 2;  // Sprite
                if (flags & 0x10) ptr += 4;  // Animation
                continue;
            }
            
            // Update Transform (always present if flag set)
            if (flags & 0x01) {
                if (ptr + 8 > data.data() + data.size()) break;
                
                int16_t pos_x, pos_y;
                uint16_t rotation, scale;
                std::memcpy(&pos_x, ptr, 2); ptr += 2;
                std::memcpy(&pos_y, ptr, 2); ptr += 2;
                std::memcpy(&rotation, ptr, 2); ptr += 2;
                std::memcpy(&scale, ptr, 2); ptr += 2;
                
                auto& transformOpt = this->getComponentEntity<Transform>(entity);
                if (transformOpt.has_value()) {
                    Transform& transform = transformOpt.value();
                    transform.x = static_cast<float>(pos_x);
                    transform.y = static_cast<float>(pos_y);
                    transform.rotation = (static_cast<float>(rotation) / 65535.0f) * 360.0f;
                    transform.scale = static_cast<float>(scale) / 1000.0f;
                }
            }
            
            // Update Velocity if present
            if (flags & 0x02) {
                if (ptr + 4 > data.data() + data.size()) break;
                
                int16_t vel_x, vel_y;
                std::memcpy(&vel_x, ptr, 2); ptr += 2;
                std::memcpy(&vel_y, ptr, 2); ptr += 2;
                
                auto& velocityOpt = this->getComponentEntity<Velocity>(entity);
                if (velocityOpt.has_value()) {
                    Velocity& velocity = velocityOpt.value();
                    velocity.vx = static_cast<float>(vel_x);
                    velocity.vy = static_cast<float>(vel_y);
                }
            }
            
            // Update Health if present
            if (flags & 0x04) {
                if (ptr + 2 > data.data() + data.size()) break;
                
                uint8_t current_health = *ptr++;
                uint8_t max_health = *ptr++;
                
                auto& healthOpt = this->getComponentEntity<Health>(entity);
                if (healthOpt.has_value()) {
                    Health& health = healthOpt.value();
                    health.currentHealth = current_health;
                    health.maxHp = max_health;
                }
            }
            
            // Sprite ID (skip for now, already set on entity creation)
            if (flags & 0x08) {
                if (ptr + 2 > data.data() + data.size()) break;
                ptr += 2;  // Skip sprite_id
            }
            
            // Animation frame (update if present)
            if (flags & 0x10) {
                if (ptr + 4 > data.data() + data.size()) break;
                
                uint16_t animation_frame, animation_id;
                std::memcpy(&animation_frame, ptr, 2); ptr += 2;
                std::memcpy(&animation_id, ptr, 2); ptr += 2;
                
                auto& animationOpt = this->getComponentEntity<Animation>(entity);
                if (animationOpt.has_value()) {
                    Animation& animation = animationOpt.value();
                    animation.currentFrame = animation_frame;
                }
            }
        }
    }

    public:
        bool _gameRunning = false;

    private:
        uint32_t _lastProjectileCount = 0;
        ProjectileInfo _lastSpawnedProjectileInfo{};
        
        PlayerSpriteAllocator _playerSpriteAllocator;
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
        std::unique_ptr<RenderManager> _renderManager;
};

#endif /* !COORDINATOR_HPP_ */
