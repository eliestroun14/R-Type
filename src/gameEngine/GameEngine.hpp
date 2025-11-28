/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** GameEngine
*/

#ifndef GAMEENGINE_HPP_
#define GAMEENGINE_HPP_

#include <memory>
#include <string>

#include "ECS/System/SystemManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"

class GameEngine {
    public:
        // ==============================================================
        //                          Initialisation
        // ==============================================================

        void Init()
        {
            this->_entityManager = std::make_unique<EntityManager>();
            this->_systemManager = std::make_unique<SystemManager>();
        }

        // ==============================================================
        //                        Gestion des entités
        // ==============================================================

        Entity CreateEntity(const std::string &entityName)
        {
            return this->_entityManager->spawnEntity(entityName);
        }

        void DestroyEntity(Entity &e)
        {
            this->_entityManager->killEntity(e);
        }

        bool IsAlive(Entity const &entity) const
        {
            return this->_entityManager->isAlive(entity);
        }

        std::string GetEntityName(Entity const &entity) const
        {
            return this->_entityManager->getEntityName(entity);
        }

        // ==============================================================
        //                       Gestion des composants
        // ==============================================================

        template<class Component>
        ComponentManager<Component> &RegisterComponent()
        {
            return this->_entityManager->registerComponent<Component>();
        }

        template <class Component>
        typename ComponentManager<Component>::referenceType
            AddComponent(Entity const &entity, Component &&component)
        {
            return this->_entityManager->addComponent(entity, component);
        }

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType
            EmplaceComponent(Entity const &entity, Params&&... params)
        {
            return this->_entityManager->emplaceComponent<Component>(entity, params...);
        }

        template<class Component>
        void RemoveComponent(Entity const &entity)
        {
            this->_entityManager->removeComponent<Component>(entity);
        }

        template<typename Component>
        ComponentManager<Component> &GetComponents() const
        {
            return this->_entityManager->getComponents<Component>();
        }

        template<typename Component>
        std::optional<Component> &GetComponentEntity(Entity const &entity) const
        {
            return this->_entityManager->getComponent<Component>(entity);
        }

        // ==============================================================
        //                        Gestion des systèmes
        // ==============================================================

        template<class System, class... Params>
        System &RegisterSystem(Params &&... params)
        {
            return this->_systemManager->addSystem<System>(std::forward<Params>(params)...);
        }

        template<class System>
        System &GetSystem() const
        {
            return this->_systemManager->getSystem<System>();
        }

        template<class System>
        void RemoveSystem()
        {
            this->_systemManager->deleteSystem<System>();
        }

        void UpdateSystems(float dt)
        {
            this->_systemManager->updateAll(dt);
        }

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
};

#endif /* !GAMEENGINE_HPP_ */
