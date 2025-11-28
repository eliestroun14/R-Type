/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** EntityManager
*/

#ifndef ENTITYMANAGER_HPP_
#define ENTITYMANAGER_HPP_

#include "../Component/ComponentManager.hpp"
#include "Entity.hpp"
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <any>
#include <unordered_set>
#include <stdexcept>
#include <optional>

class EntityManager {

    private:
        template<class Component>
        static void eraseComponentHelper(EntityManager& em, Entity const& e) {
            std::type_index key(typeid(Component));
            auto it = em._componentsArrays.find(key);
            if (it != em._componentsArrays.end()) {
                auto& components = std::any_cast<ComponentManager<Component>&>(it->second);
                if (components.size())
                    components.erase(static_cast<size_t>(e));
            }
        }

    public :
        EntityManager() = default;

        template<class Component>
        ComponentManager<Component>& registerComponent() {
            std::type_index key(typeid(Component));

            auto [it, inserted] = this->_componentsArrays.try_emplace(
                key,
                std::any(ComponentManager<Component>{})
            );

            if (inserted)
                this->_erasers[key] = &eraseComponentHelper<Component>;

            return std::any_cast<ComponentManager<Component>&>(it->second);
        }


        template <class Component>
        ComponentManager<Component> &getComponents() {
            return std::any_cast<ComponentManager<Component>&>(
                this->_componentsArrays.at(std::type_index(typeid(Component)))
            );
        }

        // register_compoenent doit être absolument appelé avec le component en question
        template <class Component>
        ComponentManager<Component> const &getComponents() const {
            return std::any_cast<const ComponentManager<Component>&>(
                this->_componentsArrays.at(std::type_index(typeid(Component)))
            );
        }


        template<class Component>
        std::optional<Component>& getComponent(Entity const& e) {
            return getComponents<Component>()[static_cast<std::size_t>(e)];
        }

        template<class Component>
        const std::optional<Component>& getComponent(Entity const& e) const {
            return getComponents<Component>()[static_cast<std::size_t>(e)];
        }

        // Usage de getComponent()
        /*
            auto& pos_opt = EntityManager.getComponent<Position>(entity);
            if (pos_opt) {
                Position& pos = *pos_opt;
                // utiliser pos
            }
        */


        Entity spawnEntity(std::string name) {
            std::size_t id;

            if (!this->_freeIds.empty()) {
                id = this->_freeIds.back();
                this->_freeIds.pop_back();
            } else
                id = this->_nextId++;

            this->_aliveEntities.insert(id);
            return Entity(id, name);
        }

        void killEntity(Entity const &entity) {
            std::size_t id = static_cast<std::size_t>(entity);

            if (this->_aliveEntities.find(id) == this->_aliveEntities.end())
                return; // l'entity est déjà morte

            this->_aliveEntities.erase(id);
            this->_freeIds.push_back(id);

            for (auto& [key, fn] : this->_erasers)
                fn(*this, entity);
        }

        bool isAlive(Entity const& e) const {
            return this->_aliveEntities.find(static_cast<std::size_t>(e)) != this->_aliveEntities.end();
        }

        template <class Component>
        bool hasComponent(Entity const& e) const {
            auto& components = getComponents<Component>();

            std::size_t id = static_cast<std::size_t>(e);
            return id < components.size() && components[id].has_value();
        }

        std::string getEntityName(Entity const& e) const {
            return e._name;
        }


        template <class Component>
        typename ComponentManager<Component>::referenceType addComponent(Entity const &e, Component &&c)
        {
            if (!isAlive(e))
                throw std::runtime_error("Cannot add component to dead entity");
            return getComponents<Component>().insertAt(static_cast<std::size_t>(e), std::forward<Component>(c));
        }

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType emplaceComponent(Entity const& e, Params&&... ps) {
            if (!isAlive(e))
                throw std::runtime_error("Cannot add component to dead entity");
            return getComponents<Component>().emplaceAt(static_cast<std::size_t>(e), std::forward<Params>(ps)...);
        }

        template<class Component>
        void removeComponent(Entity const& e) {
            if (!isAlive(e))
                throw std::runtime_error("Cannot add component to dead entity");
            getComponents<Component>().erase(static_cast<std::size_t>(e));
        }

    private:
        std::unordered_map<std::type_index, std::any> _componentsArrays;
        std::unordered_map<std::type_index, std::function<void(EntityManager&,Entity const&)>> _erasers;

        size_t _nextId = 0;
        std::vector<std::size_t> _freeIds;
        std::unordered_set<std::size_t> _aliveEntities;
};

#endif /* !ENTITYMANAGER_HPP_ */
