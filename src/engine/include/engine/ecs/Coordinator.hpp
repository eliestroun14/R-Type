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

#include "system/SystemManager.hpp"
#include "entity/EntityManager.hpp"
#include "system/SystemManager.hpp"

class Coordinator {
    public:
        // ==============================================================
        //                          Initialisation
        // ==============================================================

        void init()
        {
            this->_entityManager = std::make_unique<EntityManager>();
            this->_systemManager = std::make_unique<SystemManager>();
        }

        // ==============================================================
        //                        Gestion des entités
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
        //                       Gestion des composants
        // ==============================================================

        template<class Component>
        ComponentManager<Component> &registerComponent()
        {
            return this->_entityManager->registerComponent<Component>();
        }

        template <class Component>
        typename ComponentManager<Component>::referenceType
            addComponent(Entity const &entity, Component &&component)
        {
            return this->_entityManager->addComponent(entity, component);
        }

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType
            emplaceComponent(Entity const &entity, Params&&... params)
        {
            return this->_entityManager->emplaceComponent<Component>(entity, params...);
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
        //                        Gestion des systèmes
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

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
};

#endif /* !COORDINATOR_HPP_ */
