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

        void processInput()
        {
            this->_renderManager->processInput();
        }

        void render()
        {
            this->_renderManager->render();
        }

        bool isActionActive(GameAction action) const
        {
            return this->_renderManager->isActionActive(action);
        }

        std::map<GameAction, bool>& getActiveActions()
        {
            return this->_renderManager->getActiveActions();
        }

        bool isOpen()
        {
            return this->_renderManager->isOpen();
        }

        std::shared_ptr<sf::Texture> getTexture(Assets id) const {
            return this->_renderManager->getTexture(id);
        }

        sf::RenderWindow& getWindow() {
            return this->_renderManager->getWindow();
        }

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
        std::unique_ptr<RenderManager> _renderManager;
};

#endif /* !COORDINATOR_HPP_ */
