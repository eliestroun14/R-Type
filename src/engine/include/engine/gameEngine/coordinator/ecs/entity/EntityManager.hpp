/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** EntityManager
*/

#ifndef ENTITYMANAGER_HPP_
#define ENTITYMANAGER_HPP_

#include "../component/ComponentManager.hpp"
#include "Entity.hpp"
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <any>
#include <unordered_set>
#include <stdexcept>
#include <optional>
#include <bitset>

#define MAX_COMPONENTS 15

class EntityManager {

    using Signature = std::bitset<MAX_COMPONENTS>;

    private:
        template<class Component>
        static void eraseComponentHelper(EntityManager& em, Entity const& e) {
            std::type_index key(typeid(Component));
            auto it = em._componentsArrays.find(key);
            if (it != em._componentsArrays.end()) {
                auto& components = std::any_cast<ComponentManager<Component>&>(it->second);
                if (components.size())
                    components.erase(e);
            }
        }

        inline static std::size_t _componentTypeCounter;

        template <class Component>
        static std::size_t getComponentTypeID() {
            static std::size_t typeId = _componentTypeCounter++;
            return typeId;
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

        template <class Component>
        ComponentManager<Component> const &getComponents() const {
            return std::any_cast<const ComponentManager<Component>&>(
                this->_componentsArrays.at(std::type_index(typeid(Component)))
            );
        }


        template<class Component>
        std::optional<Component>& getComponent(Entity const& e) {
            return getComponents<Component>()[e];
        }

        template<class Component>
        const std::optional<Component>& getComponent(Entity const& e) const {
            return getComponents<Component>()[e];
        }


        void setSignature(Entity const &e, Signature signature) {
            this->_signatures[e] = signature;
        }

        Signature getSignature(Entity const &e) {
            return this->_signatures[e];
        }

        Entity spawnEntity(std::string name) {
            std::size_t id;

            if (!this->_freeIds.empty()) {
                id = this->_freeIds.back();
                this->_freeIds.pop_back();
            } else {
                id = this->_nextId++;
                if (id >= this->_signatures.size())
                    this->_signatures.resize(id + 1);
            }

            this->_signatures[id].reset();

            this->_aliveEntities.insert(id);

            if (id >= this->_entitiesName.size())
                this->_entitiesName.resize(id + 1);
            this->_entitiesName[id] = name;

            return Entity(id);
        }

        void killEntity(Entity const &entity) {
            if (this->_aliveEntities.find(entity) == this->_aliveEntities.end())
                return;

            this->_aliveEntities.erase(entity);
            this->_freeIds.push_back(entity);

            this->_signatures[entity].reset();

            for (auto& [key, fn] : this->_erasers)
                fn(*this, entity);
        }

        bool isAlive(Entity const& e) const {
            return this->_aliveEntities.find(e) != this->_aliveEntities.end();
        }

        template <class Component>
        bool hasComponent(Entity const& e) const {
            auto& components = getComponents<Component>();

            std::size_t id = e;
            return id < components.size() && components[id].has_value();
        }

        std::string getEntityName(Entity const& e) const {
            return this->_entitiesName[e];
        }


        template <class Component>
        typename ComponentManager<Component>::referenceType addComponent(Entity const &e, Component &&c)
        {
            if (!isAlive(e))
                throw std::runtime_error("Cannot add component to dead entity");

            std::size_t componentId = getComponentTypeID<Component>();
            this->_signatures[e].set(componentId, true);

            return getComponents<Component>().insertAt(e, std::forward<Component>(c));
        }

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType emplaceComponent(Entity const& e, Params&&... ps) {
            if (!isAlive(e))
                throw std::runtime_error("Cannot add component to dead entity");

            std::size_t componentId = getComponentTypeID<Component>();
            this->_signatures[e].set(componentId, true);

            return getComponents<Component>().emplaceAt(e, std::forward<Params>(ps)...);
        }

        template<class Component>
        void removeComponent(Entity const& e) {
            if (!isAlive(e))
                throw std::runtime_error("Cannot remove component from dead entity");

            std::size_t componentId = getComponentTypeID<Component>();
            this->_signatures[e].set(componentId, false);

            getComponents<Component>().erase(e);
        }

    private:
        std::unordered_map<std::type_index, std::any> _componentsArrays;
        std::unordered_map<std::type_index, std::function<void(EntityManager&,Entity const&)>> _erasers;

        size_t _nextId = 0;
        std::vector<std::size_t> _freeIds;
        std::unordered_set<std::size_t> _aliveEntities;
        std::vector<Signature> _signatures;
        std::vector<std::string> _entitiesName;
};

#endif /* !ENTITYMANAGER_HPP_ */
