/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** GameEngine
*/

#ifndef GAMEENGINE_HPP_
#define GAMEENGINE_HPP_

#include "ECS/System/SystemManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
// #include "System/System.hpp"

class GameEngine {
    public:
        void Init()
        {
            this->_entityManager = std::make_unique<EntityManager>();
            this->_systemManager = std::make_unique<SystemManager>();
        }

        Entity CreateEntity(std::string entityName)
        {
            return this->_entityManager->spawn_entity(entityName);
        }

        void DestroyEntity(Entity& e)
        {
            this->_entityManager->kill_entity(e);
            //TODO: demander à titi uwu pour comment on détruit le system d'une entity
        }

        bool IsAlive(Entity const &entity) const
        {
            return this->_entityManager->is_alive(entity);
        }

        std::string GetEntityName(Entity const &entity) const
        {
            return this->_entityManager->get_entity_name(entity);
        }


        template<class Component>
        ComponentManager<Component>& RegisterComponent()
        {
            return this->_entityManager->register_component<Component>();
        }


        template <class Component>
        typename ComponentManager<Component>::reference_type
            AddComponent(Entity const &entity, Component &&component)
        {
            return this->_entityManager->add_component(entity, component);
        }

        template<class Component, class... Params>
        typename ComponentManager<Component>::reference_type
            EmplaceComponent(Entity const &entity, Params&&... params)
        {
            return this->_entityManager->emplace_component<Component>(entity, params...);
        }

        template<class Component>
        void RemoveComponent(Entity const &entity)
        {
            this->_entityManager->remove_component<Component>(entity);
        }


        template<typename Component>
        ComponentManager<Component> &GetComponents() const
        {
            return this->_entityManager->get_components<Component>();
        }

        template<typename Component>
        std::optional<Component>& GetComponentEntity(Entity const &entity) const
        {
            return this->_entityManager->get_component<Component>(entity);
        }


        // TODO: RegisterSystem, à voir avec titi uwu

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
};

#endif /* !GAMEENGINE_HPP_ */
