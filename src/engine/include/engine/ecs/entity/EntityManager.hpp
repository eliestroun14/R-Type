/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** EntityManager
*/

#ifndef ENTITYMANAGER_HPP_
#define ENTITYMANAGER_HPP_

#include <engine/ecs/component/ComponentManager.hpp>
#include <engine/ecs/system/SystemManager.hpp>
#include <engine/ecs/entity/Entity.hpp>
#include <engine/ecs/Signature.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/logger/Logger.hpp>

#include <common/error/Error.hpp>

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <any>
#include <unordered_set>
#include <optional>
#include <cstdint>

/**
 * @class EntityManager
 * @brief Manages entity creation, destruction, signatures, and components.
 *
 * Responsibilities:
 *  - allocate and recycle entity IDs;
 *  - store each entity’s signature;
 *  - handle component addition and removal through ComponentManager;
 *  - notify the SystemManager whenever an entity’s signature changes.
 *
 * Does not store any system logic.
 */
class EntityManager {

private:
    /**
     * @brief Removes a component from an entity (generic helper).
     */
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

    inline static std::size_t _componentTypeCounter = 0;

    /**
     * @brief Returns the unique component type ID.
     */
    template <class Component>
    static std::size_t getComponentTypeID() {
        static std::size_t typeId = _componentTypeCounter++;
        return typeId;
    }

public:
    EntityManager() = default;

    /**
     * @brief Returns the unique component type ID for the given component.
     */
    template <class Component>
    std::size_t getComponentTypeId() const {
        return getComponentTypeID<Component>();
    }

    /**
     * @brief Links this EntityManager to a SystemManager.
     */
    void setSystemManager(SystemManager* sm) {
        _systemManager = sm;
    }

    /**
     * @brief Registers a component type in the ECS.
     */
    template<class Component>
    ComponentManager<Component>& registerComponent() {
        std::type_index key(typeid(Component));

        auto [it, inserted] = _componentsArrays.try_emplace(
            key,
            std::any(ComponentManager<Component>{})
        );

        if (inserted)
            _erasers[key] = &eraseComponentHelper<Component>;

        return std::any_cast<ComponentManager<Component>&>(it->second);
    }

    /**
     * @brief Retrieves the ComponentManager associated with a component type.
     */
    template <class Component>
    ComponentManager<Component>& getComponents() {
        auto key = std::type_index(typeid(Component));

        auto it = _componentsArrays.find(key);
        if (it == _componentsArrays.end())
            throw Error(ErrorType::EcsComponentAccessError, ErrorMessages::ECS_COMPONENT_ACCESS_ERROR);

        return std::any_cast<ComponentManager<Component>&>(it->second);
    }

    template <class Component>
    const ComponentManager<Component>& getComponents() const {
        auto key = std::type_index(typeid(Component));

        auto it = _componentsArrays.find(key);
        if (it == _componentsArrays.end())
            throw Error(ErrorType::EcsComponentAccessError, ErrorMessages::ECS_COMPONENT_ACCESS_ERROR);

        return std::any_cast<const ComponentManager<Component>&>(it->second);
    }

    /**
     * @brief Retrieves an optional component for a given entity.
     */
    template<class Component>
    std::optional<Component>& getComponent(Entity const& e) {
        return getComponents<Component>()[e];
    }

    template<class Component>
    const std::optional<Component>& getComponent(Entity const& e) const {
        return getComponents<Component>()[e];
    }

    /**
     * @brief Updates an entity’s signature and notifies SystemManager.
     */
    void setSignature(Entity const &e, Signature signature) {
        _signatures[e] = signature;
        if (_systemManager)
            _systemManager->entitySignatureChanged((std::size_t)e, _signatures[e]);
    }

    /**
     * @brief Returns an entity’s signature.
     */
    Signature getSignature(Entity const &e) {
        return _signatures[e];
    }

    /**
     * @brief Spawns a new entity and assigns it a name.
     */
    Entity spawnEntity(std::string name) {
        size_t id;

        if (!_freeIds.empty()) {
            id = _freeIds.back();
            _freeIds.pop_back();
        } else {
            id = _nextId++;
            if (id >= _signatures.size())
                _signatures.resize(id + 1);
        }

        _signatures[id].reset();
        _aliveEntities.insert(id);

        if (id >= _entitiesName.size())
            _entitiesName.resize(id + 1);
        _entitiesName[id] = name;

        if (_systemManager)
            _systemManager->entitySignatureChanged(id, _signatures[id]);

        return Entity(id);
    }

    /**
     * @brief Spawns a new entity with a specific ID (for network synchronization).
     * @param id The specific ID to assign to the entity (from server)
     * @param name The name to assign to the entity
     * @return The created Entity
     */
    Entity spawnEntityWithId(std::size_t id, std::string name) {
        // check if id already exists
        if (_aliveEntities.find(id) != _aliveEntities.end()) {
            LOG_ERROR("Entity with ID %zu already exists, cannot spawn", id);
            throw Error(ErrorType::EcsInvalidEntity, "Entity ID already in use");
        }

        // remove the id from the free ids'list if it is inside
        auto it = std::find(_freeIds.begin(), _freeIds.end(), id);
        if (it != _freeIds.end())
            _freeIds.erase(it);

        // update _nextId to avoid collision
        if (id >= _nextId)
            _nextId = id + 1;

        // resize vectors (if necessary)
        if (id >= _signatures.size())
            _signatures.resize(id + 1);
        if (id >= _entitiesName.size())
            _entitiesName.resize(id + 1);

        // init the entity
        _signatures[id].reset();
        _aliveEntities.insert(id);
        _entitiesName[id] = name;

        // notify the systemManager
        if (_systemManager)
            _systemManager->entitySignatureChanged(id, _signatures[id]);

        return Entity::fromId(id);
    }

    /**
     * @brief Get an entity from his id.
     */
    Entity getEntityFromID(std::uint32_t entityId)
    {
        Entity entity = Entity::fromId(static_cast<std::size_t>(entityId));
        return entity;
    }

    /**
     * @brief Destroys an entity and removes all its components.
     */
    void killEntity(Entity const &entity) {
        if (_aliveEntities.find(entity) == _aliveEntities.end())
            return;

        _aliveEntities.erase(entity);
        _freeIds.push_back(entity);

        _signatures[entity].reset();

        if (_systemManager)
            _systemManager->entitySignatureChanged((std::size_t)entity, _signatures[entity]);

        for (auto& [key, fn] : _erasers)
            fn(*this, entity);
    }

    /**
     * @brief Checks whether an entity is alive.
     */
    bool isAlive(Entity const& e) const {
        return _aliveEntities.find(e) != _aliveEntities.end();
    }

    /**
     * @brief Checks whether an entity possesses a component.
     */
    template <class Component>
    bool hasComponent(Entity const& e) const {
        auto& components = getComponents<Component>();
        std::size_t id = e;
        return id < components.size() && components[id].has_value();
    }

    /**
     * @brief Retrieves the name assigned to an entity.
     */
    std::string getEntityName(Entity const& e) const {
        return _entitiesName[e];
    }

    /**
     * @brief Adds a component to an entity.
     */
    template <class Component>
    typename ComponentManager<Component>::referenceType addComponent(Entity const &e, Component &&c)
    {
        if (!isAlive(e))
            throw Error(ErrorType::EcsInvalidEntity, ErrorMessages::ECS_INVALID_ENTITY);

        std::size_t componentId = getComponentTypeID<Component>();
        _signatures[e].set(componentId, true);

        if (_systemManager)
            _systemManager->entitySignatureChanged((std::size_t)e, _signatures[e]);

        return getComponents<Component>().insertAt(e, std::forward<Component>(c));
    }

    /**
     * @brief Constructs a component in-place for an entity.
     */
    template<class Component, class... Params>
    typename ComponentManager<Component>::referenceType emplaceComponent(Entity const& e, Params&&... ps) {
        if (!isAlive(e))
            throw Error(ErrorType::EcsInvalidEntity, ErrorMessages::ECS_INVALID_ENTITY);

        std::size_t componentId = getComponentTypeID<Component>();
        _signatures[e].set(componentId, true);

        if (_systemManager)
            _systemManager->entitySignatureChanged((std::size_t)e, _signatures[e]);

        return getComponents<Component>().emplaceAt(e, std::forward<Params>(ps)...);
    }

    template<class Component>
    void updateComponent(Entity const& e, const Component& newData) {
        if (!isAlive(e))
            throw Error(ErrorType::EcsInvalidEntity, ErrorMessages::ECS_INVALID_ENTITY);

        auto& component = getComponent<Component>(e);
        component = newData;
    }

    /**
     * @brief Removes a component from an entity.
     */
    template<class Component>
    void removeComponent(Entity const& e) {
        if (!isAlive(e))
            throw Error(ErrorType::EcsInvalidEntity, ErrorMessages::ECS_INVALID_ENTITY);

        std::size_t componentId = getComponentTypeID<Component>();
        _signatures[e].set(componentId, false);

        if (_systemManager)
            _systemManager->entitySignatureChanged((std::size_t)e, _signatures[e]);

        getComponents<Component>().erase(e);
    }


    void removeComponentByType(uint8_t componentType, Entity entity)
    {
        switch (componentType) {
            case 0x01:
                this->removeComponent<Transform>(entity);
                break;
            case 0x02:
                this->removeComponent<Velocity>(entity);
                break;
            case 0x03:
                this->removeComponent<Health>(entity);
                break;
            case 0x04:
                this->removeComponent<Weapon>(entity);
                break;
            case 0x05:
                this->removeComponent<AI>(entity);
                break;
            case 0x07:
                this->removeComponent<HitBox>(entity);
                break;
            case 0x08:
                this->removeComponent<Sprite>(entity);
                break;
            case 0x09:
                this->removeComponent<Animation>(entity);
                break;
            case 0x0A:
                this->removeComponent<Powerup>(entity);
                break;
            case 0x0C:
                this->removeComponent<InputComponent>(entity);
                break;
            default:
                LOG_ERROR("Unknown component type: %u", componentType);
                break;
        }
    }
private:
    std::unordered_map<std::type_index, std::any> _componentsArrays; /**< Storage for all component arrays */
    std::unordered_map<std::type_index, std::function<void(EntityManager&,Entity const&)>> _erasers; /**< Generic component removers */

    size_t _nextId = 1;  // Start at 1, 0 is reserved for errors
    std::vector<std::size_t> _freeIds; /**< Recycled entity IDs */
    std::unordered_set<std::size_t> _aliveEntities; /**< Active entities */
    std::vector<Signature> _signatures; /**< Signatures per entity */
    std::vector<std::string> _entitiesName; /**< Names per entity */

    SystemManager* _systemManager = nullptr; /**< Callback target for signature updates */
};

#endif /* !ENTITYMANAGER_HPP_ */
