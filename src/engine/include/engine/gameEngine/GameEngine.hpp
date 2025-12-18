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
#include <engine/gameEngine/coordinator/ecs/entity/EntityManager.hpp>
#include <engine/gameEngine/coordinator/ecs/system/SystemManager.hpp>
#include <engine/gameEngine/coordinator/render/RenderManager.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>

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

                this->_renderManager = std::make_unique<RenderManager>();
                this->_renderManager->init();
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
             * @brief Removes an entity and all its associated components.
             * @param e Reference to the entity to destroy.
             */
            void destroyEntity(Entity &e)
            {
                this->_entityManager->killEntity(e);
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

            /** @brief Retrieves a texture from the asset manager via its ID. */
            std::shared_ptr<sf::Texture> getTexture(Assets id) const
            {
                return this->_renderManager->getTexture(id);
            }

            /** @brief Processes window events and updates input states. */
            void processInput()
            {
                this->_renderManager->processInput();
            }

            /** @brief Clears the window and prepares for a new frame. */
            void beginFrame()
            {
                this->_renderManager->beginFrame();
            }

            /** @brief Displays everything that was drawn during the frame. */
            void render()
            {
                this->_renderManager->render();
            }
    };
}

#endif /* !GAMEENGINE_HPP_ */