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
#include <engine/gameEngine/coordinator/render/RenderManager.hpp>
#include <engine/gameEngine/coordinator/network/PacketManager.hpp>
#include <common/protocol/Protocol.hpp>
#include <cstring>

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
        //                              Render
        // ==============================================================

        /**
         * @brief Polls all pending input events from the window.
         * * Retrieves events (keyboard, mouse, window close) from SFML
         * and updates the internal state of GameActions via handleEvent().
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
            // TODO
            for (const auto&packet : packetsToProcess) {

                if (PacketManager::assertEntitySpawn(packet)) {
                    this->handlePacketCreateEntity(packet);
                }
                if (PacketManager::assertEntityDestroy(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertEntitySpawn(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertEntityUpdate(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertTransformSnapshot(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertHealthSnapshot(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertWeaponSnapshot(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertComponentRemove(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertAnimationSnapshot(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertTransformSnapshotDelta(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertHealthSnapshotDelta(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }

                //                  GAME_EVENTS (0x40-0x5F)
                

                if (PacketManager::assertPlayerHit(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertPlayerDeath(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertScoreUpdate(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertPowerupPickup(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertWeaponFire(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertVisualEffect(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertVisualEffect(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertAudioEffect(packet)) {
                    //this->hanndleEntitySpawn(packet);
                }
                if (PacketManager::assertParticleSpawn(packet)) {
                    //
                }

                //                  GAME_CONTROL (0x60-0x6F)

                if (PacketManager::assertGameStart(packet)) {
                    //
                }
                if (PacketManager::assertGameEnd(packet)) {
                    //
                }
                if (PacketManager::assertLevelComplete(packet)) {
                    //
                }
                if (PacketManager::assertLevelStart(packet)) {
                    //
                }
                if (PacketManager::assertForceState(packet)) {
                    //
                }
                if (PacketManager::assertAIState(packet)) {
                    //
                }




            }
        }

        void handlePlayerInputPacket(const common::protocol::Packet& packet, uint64_t elapsedMs)
        {
            // TODO
            //this->_renderManager->_activeActions[GameAction::]
        }

        void handlePacketCreateEntity(const common::protocol::Packet& packet)
        {
            // TODO
            // Extract entity data from the packet and create the entity
            Entity newEntity = this->createEntity("ReceivedEntity");
            // Further component initialization based on packet data would go here

            protocol::EntitySpawn  entitySpawn;
            std::memcpy(&entitySpawn, packet.data.data(), sizeof(protocol::EntitySpawn));

            
            //_coordinator->addComponent<Sprite>(staticEnemy, Sprite(BASE_ENEMY, 1, sf::IntRect(0, 0, 33, 36)));
            //_coordinator->addComponent<Transform>(staticEnemy, Transform(400.f, 200.f, 0.f, 2.0f));
            //_coordinator->addComponent<Health>(staticEnemy, Health(50, 50));
            //_coordinator->addComponent<HitBox>(staticEnemy, HitBox());
            //_coordinator->addComponent<Velocity>(staticEnemy, Velocity(0.f, 0.f));
            //_coordinator->addComponent<Weapon>(staticEnemy, Weapon(300, 0, 8, ProjectileType::MISSILE));  // 300ms fire rate, 8 damage

        }

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
        std::unique_ptr<RenderManager> _renderManager;
};

#endif /* !COORDINATOR_HPP_ */
