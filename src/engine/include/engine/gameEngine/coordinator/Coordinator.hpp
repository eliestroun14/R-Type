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
#include <include/engine/gameEngine/coordinator/render/SpriteAllocator.hpp>


class Coordinator {
    public:

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
                            this->handleGameStart();
                        }
                        break;
                    case static_cast<uint8_t>(protocol::PacketTypes::TYPE_GAME_END):
                        if (PacketManager::assertGameEnd(packet)) {
                            this->handleGameEnd();
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
            // TODO
        }

        void buildClientPacketBasedOnStatus(std::vector<common::protocol::Packet> &outgoingPackets, uint64_t elapsedMs)
        {
            // TODO
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
                    addComponent<Sprite>(newEntity, Sprite(spriteAsset, 1, sf::IntRect(0, 0, 33, 15)));
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
                    addComponent<Sprite>(newEntity, Sprite(BASE_ENEMY, 1, sf::IntRect(0, 0, BASE_ENEMY_SPRITE_WIDTH, BASE_ENEMY_SPRITE_HEIGHT)));
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
                    addComponent<Sprite>(newEntity, Sprite(DEFAULT_BULLET, 1, sf::IntRect(0, 0, 16, 16)));
                    addComponent<HitBox>(newEntity, HitBox());
                    addComponent<Projectile>(newEntity, Projectile(isPlayerProjectile ? ProjectileOwner::PLAYER: ProjectileOwner::ENEMY));

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
        constexpr std::size_t HEADER_SIZE =
            sizeof(protocol::PacketHeader) +
            sizeof(uint32_t) +   // world_tick
            sizeof(uint16_t);    // entity_count

        constexpr std::size_t ENTRY_SIZE =
            sizeof(uint32_t) +   // entity_id
            sizeof(protocol::ComponentTransform); // 8 bytes

        if (packet.data.size() < HEADER_SIZE) {
            LOG_ERROR_CAT("Coordinator",
                "TransformSnapshot: packet too small (%zu)",
                packet.data.size());
            return;
        }

        const uint8_t* ptr = packet.data.data();
        const uint8_t* end = ptr + packet.data.size();

        protocol::PacketHeader header;
        std::memcpy(&header, ptr, sizeof(header));
        ptr += sizeof(header);

        uint32_t world_tick = 0;
        uint16_t entity_count = 0;

        std::memcpy(&world_tick, ptr, sizeof(world_tick));
        ptr += sizeof(world_tick);

        std::memcpy(&entity_count, ptr, sizeof(entity_count));
        ptr += sizeof(entity_count);

        const std::size_t expectedSize =
            HEADER_SIZE + static_cast<std::size_t>(entity_count) * ENTRY_SIZE;

        if (packet.data.size() != expectedSize) {
            LOG_ERROR_CAT("Coordinator",
                "TransformSnapshot: size mismatch (%zu != %zu)",
                packet.data.size(), expectedSize);
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
        if (packet.data.size() != sizeof(protocol::GameStart)) {
            LOG_ERROR_CAT("Coordinator", "GameStart: invalid size %zu", packet.data.size());
            return;
        }

        _gameRunning = true;
    }

    void handleGameEnd(const common::protocol::Packet& packet)
    {
        if (packet.data.size() != sizeof(protocol::GameEnd)) {
            LOG_ERROR_CAT("Coordinator", "GameEnd: invalid size %zu", packet.data.size());
            return;
        }

        _gameRunning = false;
    }

    private:
        _gameRunning = false;
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
        std::unique_ptr<RenderManager> _renderManager;
};

#endif /* !COORDINATOR_HPP_ */
