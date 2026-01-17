/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** GameEngine
*/

/**
 * @file GameEngine.hpp
 * @brief Header file for the GameEngine class, acting as the main coordinator for ECS and Rendering.
 */

#ifndef GAMEENGINE_HPP_
#define GAMEENGINE_HPP_

#include <memory>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/system/SystemManager.hpp>
#include <engine/render/RenderManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/audio/AudioManager.hpp>

/**
 * @namespace gameEngine
 * @brief Namespace for all game engine related structures and classes.
 */
namespace gameEngine {

    /**
     * @class GameEngine
     * @brief The main facade of the engine. It coordinates the EntityManager, SystemManager, and RenderManager.
     */
    class GameEngine {
        private:
            std::shared_ptr<EntityManager> _entityManager; ///< Manager handling entity lifecycle and components.
            std::shared_ptr<SystemManager> _systemManager; ///< Manager handling system registration and updates.
            std::shared_ptr<RenderManager> _renderManager; ///< Manager handling windowing, inputs, and drawing.
            std::shared_ptr<audio::AudioManager> _audioManager;  ///< Manager handling musics and sounds effects.

        public:

            // ################################################################
            // ########################### INIT ###############################
            // ################################################################

            /**
             * @brief Initializes the GameEngine and its internal managers.
             * * This method must be called before any other operation. It instantiates the
             * managers and links the SystemManager to the EntityManager.
             */
            void init()
            {
                this->_entityManager = std::make_unique<EntityManager>();
                this->_systemManager = std::make_unique<SystemManager>();
                this->_entityManager->setSystemManager(this->_systemManager.get());
            }


            /**
             * @brief Initializes RenderManager for the gameEngine, client side.
             * * This method must be called before any other operation.
             */
            void initRender()
            {
                this->_renderManager = std::make_unique<RenderManager>();
                this->_renderManager->init();
            }

            /**
             * @brief Initializes AudioManager for the gameEngine, client side.
             * * This method must be called before any other operation.
             */
            void initAudio()
            {
                this->_audioManager = std::make_unique<audio::AudioManager>();
                this->_audioManager->init();
            }

            // ################################################################
            // ########################### ENTITY #############################
            // ################################################################

            /**
             * @brief Spawns a new entity in the engine.
             * @param entityName A debug name for the entity.
             * @return Entity The ID/Handle of the created entity.
             */
            Entity createEntity(std::string entityName)
            {
                Entity entity = this->_entityManager->spawnEntity(entityName);
                return entity;
            }

            /**
             * @brief Spawns a new entity with a specific ID (for network synchronization).
             * @param id The specific ID to assign to the entity (from server)
             * @param name The name to assign to the entity
             * @return The created Entity
             */
            Entity createEntityWithId(uint32_t id, std::string entityName)
            {
                Entity entity = this->_entityManager->spawnEntityWithId(id, entityName);
                return entity;
            }

            /**
             * @brief Spawns a new entity in the engine.
             * @param entityName A debug name for the entity.
             * @return Entity The ID/Handle of the created entity.
             */
            Entity getEntityFromId(std::uint32_t entityId)
            {
                return this->_entityManager->getEntityFromID(entityId);
            }

            /**
             * @brief Removes an entity and all its associated components.
             * @param entityId Reference to the entity id to destroy.
             */
            void destroyEntity(std::uint32_t entityId)
            {
                Entity entity = getEntityFromId(entityId);
                this->_entityManager->killEntity(entity);
            }

            /**
             * @brief Checks if an entity still exists in the manager.
             * @param entity The entity to check.
             * @return true if the entity is active, false otherwise.
             */
            bool isAlive(Entity const &entity) const
            {
                return this->_entityManager->isAlive(entity);
            }

            /**
             * @brief Gets the debug name assigned to an entity.
             * @param entity The target entity.
             * @return std::string The name of the entity.
             */
            std::string getEntityName(Entity const &entity) const
            {
                return this->_entityManager->getEntityName(entity);
            }

            // ################################################################
            // ########################## COMPONENT ###########################
            // ################################################################

            /**
             * @brief Registers a new component type to the engine.
             * @tparam Component The type of the component to register.
             * @return ComponentManager<Component>& Reference to the manager for this component type.
             */
            template<class Component>
            ComponentManager<Component> &registerComponent()
            {
                return this->_entityManager->registerComponent<Component>();
            }

            /**
             * @brief Adds a component to a specific entity.
             * @tparam Component The type of the component.
             * @param entity The entity receiving the component.
             * @param component The component data.
             * @return Reference to the newly added component.
             */
            template <class Component>
            typename ComponentManager<Component>::referenceType
            addComponent(Entity const &entity, Component component)
            {
                return this->_entityManager->template addComponent<Component>(entity, std::move(component));
            }

            /**
             * @brief Constructs a component in-place for a specific entity.
             * @tparam Component The type of the component.
             * @tparam Params Types of the constructor arguments.
             * @param entity The entity receiving the component.
             * @param params Arguments passed to the component constructor.
             * @return Reference to the newly created component.
             */
            template<class Component, class... Params>
            typename ComponentManager<Component>::referenceType
            emplaceComponent(Entity const &entity, Params&&... params)
            {
                return this->_entityManager->template emplaceComponent<Component>(entity, std::forward<Params>(params)...);
            }

            /**
             * @brief Update a component in-place for a specific entity.
             * @tparam Component The type of the component.
             * @param entity The entity receiving the component.
             * @param newData Component with the new data.
             */
            template<class Component>
            void updateComponent(Entity const& e, const Component& newData)
            {
                this->_entityManager->updateComponent(e, newData);
            }

            /**
             * @brief Removes a component from an entity.
             * @tparam Component The type of component to remove.
             * @param entity The target entity.
             */
            template<class Component>
            void removeComponent(Entity const &entity)
            {
                this->_entityManager->removeComponent<Component>(entity);
            }

            /**
             * @brief Gets the container of all components of a certain type.
             * @tparam Component The type of component.
             * @return ComponentManager<Component>& The manager containing the sparse array of components.
             */
            template<typename Component>
            ComponentManager<Component> &getComponents() const
            {
                return this->_entityManager->getComponents<Component>();
            }

            /**
             * @brief Retrieves a specific component for a specific entity.
             * @tparam Component The type of component.
             * @param entity The target entity.
             * @return std::optional<Component>& An optional containing the component if it exists.
             */
            template<typename Component>
            std::optional<Component> &getComponentEntity(Entity const &entity) const
            {
                return this->_entityManager->getComponent<Component>(entity);
            }

            /**
             * @brief Gets the unique internal ID for a component type.
             * @tparam Component The component type.
             * @return std::size_t The internal bitset index for this component.
             */
            template <class Component>
            std::size_t getComponentTypeId() const {
                return this->_entityManager->getComponentTypeID<Component>();
            }


            void removeComponentByType(uint8_t componentType, Entity entity)
            {
                this->_entityManager->removeComponentByType(componentType, entity);
            }


            /**
             * @brief Checks whether an entity possesses a component.
             */
            template <class Component>
            bool hasComponent(Entity const& e) const {
                return this->_entityManager->hasComponent<Component>(e);
            }

            // ################################################################
            // ########################### SYSTEM #############################
            // ################################################################

            /**
             * @brief Registers a system to the engine.
             * @tparam System The class of the system.
             * @tparam Params Types of constructor arguments.
             * @param params Arguments to initialize the system.
             * @return System& Reference to the created system.
             */
            template<class System, class... Params>
            System &registerSystem(Params &&... params)
            {
                return this->_systemManager->addSystem<System>(std::forward<Params>(params)...);
            }

            /**
             * @brief Retrieves a registered system.
             * @tparam System The class of the system.
             * @return System& Reference to the system.
             */
            template<class System>
            System &getSystem() const
            {
                return this->_systemManager->getSystem<System>();
            }

            /**
             * @brief Removes a system from the engine.
             * @tparam System The class of the system to delete.
             */
            template<class System>
            void removeSystem()
            {
                this->_systemManager->deleteSystem<System>();
            }

            /**
             * @brief Triggers the update method of all registered systems.
             * @param dt Delta time since last frame.
             */
            void updateSystems(float dt)
            {
                this->_systemManager->updateAll(dt);
            }

            /**
             * @brief Calls the onCreate method for all systems.
             */
            void onCreateSystems()
            {
                _systemManager->onCreateAll();
            }

            /**
             * @brief Calls the onDestroy method for all systems.
             */
            void onDestroySystems()
            {
                _systemManager->onDestroyAll();
            }

            /**
             * @brief Defines which components a system requires to process an entity.
             * @tparam S The system type.
             * @tparam Components List of component types forming the signature.
             */
            template <class S, class... Components>
            void setSystemSignature()
            {
                Signature sig{};
                (sig.set(_entityManager->template getComponentTypeId<Components>()), ...);
                _systemManager->setSignature<S>(sig);
            }

            // ################################################################
            // ########################### INPUTS #############################
            // ################################################################

            /**
             * @brief Updates input state for a player (Server or Client side).
             * @param entity The player entity.
             * @param playerId Internal ID of the player.
             * @param action The action to update (e.g., Move Up, Shoot).
             * @param isActive Boolean state of the action.
             */
            void setPlayerInputAction(Entity entity, uint32_t playerId, GameAction action, bool isActive)
            {
                auto& inputComp = this->_entityManager->getComponent<InputComponent>(entity);
                if (inputComp.has_value()) {
                    inputComp.value().activeActions[action] = isActive;
                }
            }

            /**
             * @brief Checks if a player entity is performing a specific action.
             * @param entity The player entity.
             * @param action The action to check.
             * @return true if the action is currently active.
             */
            bool isPlayerActionActive(Entity entity, GameAction action) const
            {
                const auto& inputComp = this->_entityManager->getComponent<InputComponent>(entity);
                if (!inputComp.has_value()) return false;
                auto it = inputComp.value().activeActions.find(action);
                return it != inputComp.value().activeActions.end() && it->second;
            }

            /**
             * @brief Gets all current actions for a player.
             * @param entity The player entity.
             * @return std::map<GameAction, bool>& Map of actions and their states.
             */
            std::map<GameAction, bool>& getPlayerActiveActions(Entity entity)
            {
                auto& inputComp = this->_entityManager->getComponent<InputComponent>(entity);
                return inputComp.value().activeActions;
            }

            // ################################################################
            // ########################### RENDER #############################
            // ################################################################

            /** @brief Returns current mouse position relative to the window. */
            sf::Vector2i getMousePosition() const
            {
                return this->_renderManager->getMousePosition();
            }

            /** @brief Returns a reference to the SFML RenderWindow. */
            sf::RenderWindow& getWindow()
            {
                return this->_renderManager->getWindow();
            }

            /** @brief Checks if the graphical window is still open. */
            bool isWindowOpen()
            {
                return this->_renderManager->isOpen();
            }

            /** @brief Gets the current rendering scale factor (for responsive UI/Sprites). */
            float getScaleFactor() const
            {
                return this->_renderManager->getScaleFactor();
            }

            /** @brief Gets the map of all global actions from the RenderManager. */
            std::map<GameAction, bool> &getMapAction()
            {
                return this->_renderManager->getActiveActions();
            }

            /** @brief Checks if a global action is active. */
            bool isActionActive(GameAction action) const
            {
                return this->_renderManager->isActionActive(action);
            }

            /** @brief Checks if an action was pressed during this specific frame. */
            bool isActionJustPressed(GameAction action) const
            {
                return this->_renderManager->isActionJustPressed(action);
            }

            /** @brief Checks if an action was released during this specific frame. */
            bool isActionJustReleased(GameAction action) const
            {
                return this->_renderManager->isActionJustReleased(action);
            }

            /** @brief Retrieves a texture from the asset manager via its ID. */
            std::shared_ptr<sf::Texture> getTexture(Assets id) const
            {
                return this->_renderManager->getTexture(id);
            }

            /** @brief Retrieves a font from the asset manager via its ID. */
            std::shared_ptr<sf::Font> getFont(FontAssets id) const
            {
                return this->_renderManager->getFont(id);
            }

            /** @brief Processes window events and updates input states. */
            void processInput()
            {
                if (this->_renderManager) {
                    this->_renderManager->processInput();
                    
                    // Sync RenderManager inputs to playable player entities
                    syncInputsToPlayableEntities();
                }
            }
            
            /** @brief Syncs RenderManager input state to playable entities' InputComponents. */
            void syncInputsToPlayableEntities()
            {
                if (!this->_renderManager) return;
                
                // Get current input state from RenderManager
                auto& activeActions = this->_renderManager->getActiveActions();
                
                // Find all playable entities and update their InputComponent
                auto& playables = this->_entityManager->getComponents<Playable>();
                auto& inputs = this->_entityManager->getComponents<InputComponent>();
                
                int syncedCount = 0;
                for (size_t entityId = 0; entityId < playables.size(); ++entityId) {
                    if (playables[entityId].has_value() && inputs[entityId].has_value()) {
                        // This is a playable entity with an InputComponent
                        auto& inputComp = inputs[entityId].value();
                        
                        // Sync all actions from RenderManager to this entity's InputComponent
                        for (const auto& [action, isActive] : activeActions) {
                            inputComp.activeActions[action] = isActive;
                        }
                        syncedCount++;
                    }
                }
                
                // Log if any input is active (to avoid spam)
                bool hasInput = false;
                for (const auto& [action, isActive] : activeActions) {
                    if (isActive) {
                        hasInput = true;
                        break;
                    }
                }
                
                if (hasInput && syncedCount > 0) {
                    // Check if SHOOT is in the active actions for debugging
                    auto shootIt = activeActions.find(GameAction::SHOOT);
                    if (shootIt != activeActions.end() && shootIt->second) {
                        std::cout << "[GameEngine] SHOOT action synced to " << syncedCount << " playable entities" << std::endl;
                    }
                    LOG_DEBUG_CAT("GameEngine", "Synced inputs to {} playable entities", syncedCount);
                }
            }

            /** @brief Clears the window and prepares for a new frame. */
            void beginFrame()
            {
                if (this->_renderManager) {
                    this->_renderManager->beginFrame();
                }
            }

            /** @brief Displays everything that was drawn during the frame. */
            void render()
            {
                if (this->_renderManager) {
                    this->_renderManager->render();
                }
            }

            void closeWindow()
            {
                this->_renderManager->closeWindow();
            }

            std::shared_ptr<RenderManager> getRenderManager() const
            {
                return this->_renderManager;
            }

            // ################################################################
            // ########################### AUDIO #############################
            // ################################################################

            /**
             * @brief Play a positioned 3D sound effect
             * @param type The audio effect type
             * @param x,y The position of the sound
             * @param volume Volume multiplier (0.0 - 1.0)
             * @param pitch Pitch multiplier (1.0 = normal)
             */
            void playSound(protocol::AudioEffectType type, float x, float y, 
                        float volume = 1.0f, float pitch = 1.0f)
            {
                if (this->_audioManager) {
                    this->_audioManager->playSound(type, x, y, volume, pitch);
                }
            }

            /**
             * @brief Play a non-positioned UI sound effect
             * @param type The audio effect type
             * @param volume Volume multiplier (0.0 - 1.0)
             * @param pitch Pitch multiplier (1.0 = normal)
             */
            void playSoundUI(protocol::AudioEffectType type, 
                            float volume = 1.0f, float pitch = 1.0f)
            {
                if (this->_audioManager) {
                    this->_audioManager->playSoundUI(type, volume, pitch);
                }
            }

            /**
             * @brief Play background music (stops previous music)
             * @param filepath Path to the music file
             * @param volume Volume level (0.0 - 1.0)
             */
            void playMusic(const std::string& filepath, float volume = 0.5f)
            {
                if (this->_audioManager) {
                    this->_audioManager->playMusic(filepath, volume);
                }
            }

            /**
             * @brief Stop the currently playing music
             */
            void stopMusic()
            {
                if (this->_audioManager) {
                    this->_audioManager->stopMusic();
                }
            }

            /**
             * @brief Update the audio listener position (usually the player's position)
             * @param x,y The listener position
             */
            void updateAudioListener(float x, float y)
            {
                if (this->_audioManager) {
                    this->_audioManager->setListenerPosition(x, y);
                }
            }

            /**
             * @brief Set the master volume (affects all audio)
             * @param volume Volume level (0.0 - 1.0)
             */
            void setMasterVolume(float volume)
            {
                this->_audioManager->setMasterVolume(volume);
            }

            /**
             * @brief Set the sound effects volume
             * @param volume Volume level (0.0 - 1.0)
             */
            void setSoundVolume(float volume)
            {
                this->_audioManager->setSoundVolume(volume);
            }

            /**
             * @brief Set the music volume
             * @param volume Volume level (0.0 - 1.0)
             */
            void setMusicVolume(float volume)
            {
                this->_audioManager->setMusicVolume(volume);
            }

            /**
             * @brief Get the audio manager instance
             * @return Pointer to the audio manager
             */
            audio::AudioManager* getAudioManager()
            {
                return this->_audioManager.get();
            }

            /**
             * @brief Update audio systems (cleanup finished sounds, etc.)
             */
            void updateAudio()
            {
                this->_audioManager->update();
            }
    };
}

#endif /* !GAMEENGINE_HPP_ */