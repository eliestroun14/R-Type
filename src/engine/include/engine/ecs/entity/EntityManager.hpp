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
 * @enum EntityCategory
 * @brief Categorizes entities into local and networked types.
 */
enum class EntityCategory : uint8_t {
    LOCAL = 0,      /**< Local entity (not synchronized over network) */
    NETWORKED = 1   /**< Networked entity (synchronized over network) */
};

/**
 * @brief Offset for networked entity IDs to avoid collisions with local IDs.
 * Local entities use IDs 1 to NETWORKED_ID_OFFSET-1.
 * Networked entities use IDs NETWORKED_ID_OFFSET and above.
 */
static constexpr size_t NETWORKED_ID_OFFSET = 1000000;

/**
 * @class EntityManager
 * @brief Manages entity creation, destruction, signatures, and components.
 *
 * Responsibilities:
 *  - allocate and recycle entity IDs;
 *  - store each entity's signature;
 *  - handle component addition and removal through ComponentManager;
 *  - notify the SystemManager whenever an entity's signature changes;
 *  - manage separate ID spaces for local and networked entities.
 *
 * Does not store any system logic.
 * 
 * Entity ID space is divided:
 *  - Local entities:    1 to NETWORKED_ID_OFFSET-1
 *  - Networked entities: NETWORKED_ID_OFFSET+
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
     * @brief Get all local entities.
     */
    const std::unordered_set<std::size_t>& getLocalEntities() const {
        return _localEntities;
    }

    /**
     * @brief Get all networked entities.
     */
    const std::unordered_set<std::size_t>& getNetworkedEntities() const {
        return _networkedEntities;
    }

    /**
     * @brief Check if an entity ID belongs to the networked category.
     */
    static bool isNetworkedId(std::size_t id) {
        return id >= NETWORKED_ID_OFFSET;
    }

    /**
     * @brief Convert a networked entity ID to its network-relative ID (without offset).
     */
    static std::size_t toNetworkRelativeId(std::size_t id) {
        return id >= NETWORKED_ID_OFFSET ? id - NETWORKED_ID_OFFSET : id;
    }

    /**
     * @brief Convert a network-relative ID to the actual entity ID (with offset).
     */
    static std::size_t fromNetworkRelativeId(std::size_t networkId) {
        return networkId + NETWORKED_ID_OFFSET;
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
     * @brief Updates an entity's signature and notifies SystemManager.
     */
    void setSignature(Entity const &e, Signature signature) {
        std::size_t id = e;
        if (id >= _signatures.size())
            _signatures.resize(id + 1);
        _signatures[id] = signature;
        if (_systemManager)
            _systemManager->entitySignatureChanged(id, _signatures[id]);
    }

    /**
     * @brief Returns an entity's signature.
     */
    Signature getSignature(Entity const &e) {
        std::size_t id = e;
        if (id >= _signatures.size())
            return Signature();
        return _signatures[id];
    }

    /**
     * @brief Spawns a new entity and assigns it a name.
     * @param name The name of the entity
     * @param category The category of the entity (LOCAL or NETWORKED)
     * @return The created entity
     */
    Entity spawnEntity(std::string name, EntityCategory category = EntityCategory::LOCAL) {
        size_t id;
        
        if (category == EntityCategory::LOCAL) {
            if (!_freeIdsLocal.empty()) {
                id = _freeIdsLocal.back();
                _freeIdsLocal.pop_back();
            } else {
                id = _nextIdLocal++;
            }
            _localEntities.insert(id);
        } else {
            if (!_freeIdsNetworked.empty()) {
                id = _freeIdsNetworked.back();
                _freeIdsNetworked.pop_back();
            } else {
                id = NETWORKED_ID_OFFSET + _nextIdNetworked++;
            }
            _networkedEntities.insert(id);
        }

        if (id >= _signatures.size())
            _signatures.resize(id + 1);
        if (id >= _entitiesName.size())
            _entitiesName.resize(id + 1);

        _signatures[id].reset();
        _entitiesName[id] = name;

        if (_systemManager)
            _systemManager->entitySignatureChanged(id, _signatures[id]);

        return Entity(id);
    }

    /**
     * @brief Get the next entity ID that will be assigned.
     * @return The next available local entity ID.
     */
    uint32_t getNextEntityId() const {
        return _nextIdLocal;
    }

    /**
     * @brief Get the next networked entity ID that will be assigned.
     * Prioritizes reusing recycled IDs from the free list (only if they're actually free).
     * Skips any IDs that are still in use.
     * @return The next available networked entity ID (network-relative, without offset).
     */
    uint32_t getNextNetworkedEntityId() {
        // Clean up free list: remove IDs that are still alive (shouldn't happen, but be safe)
        while (!_freeIdsNetworked.empty()) {
            uint32_t candidateId = _freeIdsNetworked.back();
            if (_networkedEntities.find(candidateId) == _networkedEntities.end()) {
                // This ID is truly free, return the network-relative ID
                _freeIdsNetworked.pop_back();
                return toNetworkRelativeId(candidateId);
            }
            // This ID is still alive somehow, remove it from free list and continue
            _freeIdsNetworked.pop_back();
        }
        
        // No recyclable IDs available, generate a new one
        // Skip any IDs that are still in use (shouldn't happen normally)
        size_t actualId = NETWORKED_ID_OFFSET + _nextIdNetworked;
        while (_networkedEntities.find(actualId) != _networkedEntities.end()) {
            _nextIdNetworked++;
            actualId = NETWORKED_ID_OFFSET + _nextIdNetworked;
        }
        return _nextIdNetworked++;
    }

    /**
     * @brief Spawns a new entity with a specific ID (for network synchronization).
     * @param id The specific ID to assign to the entity. 
     *           Can be either the actual internal ID (with offset) or network-relative ID.
     *           The function auto-detects based on whether the ID >= NETWORKED_ID_OFFSET.
     * @param name The name to assign to the entity
     * @param category The category of the entity (LOCAL or NETWORKED)
     * @return The created Entity
     */
    Entity spawnEntityWithId(std::size_t id, std::string name, EntityCategory category = EntityCategory::NETWORKED) {
        std::size_t actualId = id;
        
        // For networked entities, auto-detect if the ID already has the offset
        // If id >= NETWORKED_ID_OFFSET, assume it's already the full ID
        // If id < NETWORKED_ID_OFFSET, add the offset
        if (category == EntityCategory::NETWORKED && id < NETWORKED_ID_OFFSET) {
            actualId = NETWORKED_ID_OFFSET + id;
        }

        // Check only in the appropriate list (not in both)
        if (category == EntityCategory::LOCAL) {
            if (_localEntities.find(actualId) != _localEntities.end()) {
                LOG_ERROR("Entity with ID {} already exists in LOCAL list, cannot spawn", actualId);
                throw Error(ErrorType::EcsInvalidEntity, "Entity ID already in use");
            }
        } else {
            if (_networkedEntities.find(actualId) != _networkedEntities.end()) {
                LOG_ERROR("Entity with ID {} already exists in NETWORKED list, cannot spawn", actualId);
                throw Error(ErrorType::EcsInvalidEntity, "Entity ID already in use");
            }
        }

        // Get the appropriate free list based on category
        std::vector<std::size_t>& freeIdsList = (category == EntityCategory::LOCAL) ? _freeIdsLocal : _freeIdsNetworked;

        // remove the id from the free ids' list if it is inside
        auto it = std::find(freeIdsList.begin(), freeIdsList.end(), actualId);
        if (it != freeIdsList.end())
            freeIdsList.erase(it);

        // update _nextId to avoid collision
        if (category == EntityCategory::LOCAL) {
            if (actualId >= _nextIdLocal)
                _nextIdLocal = actualId + 1;
            _localEntities.insert(actualId);
        } else {
            std::size_t relativeId = actualId - NETWORKED_ID_OFFSET;
            if (relativeId >= _nextIdNetworked)
                _nextIdNetworked = relativeId + 1;
            _networkedEntities.insert(actualId);
        }

        // resize vectors (if necessary)
        if (actualId >= _signatures.size())
            _signatures.resize(actualId + 1);
        if (actualId >= _entitiesName.size())
            _entitiesName.resize(actualId + 1);

        // init the entity
        _signatures[actualId].reset();
        _entitiesName[actualId] = name;

        // notify the systemManager
        if (_systemManager)
            _systemManager->entitySignatureChanged(actualId, _signatures[actualId]);

        return Entity::fromId(actualId);
    }

    /**
     * @brief Get an entity from his id.
     * @param entityId The network-relative ID for networked entities, or direct ID for local entities.
     * @param isNetworked Whether this is a networked entity ID.
     */
    Entity getEntityFromID(std::uint32_t entityId, bool isNetworked = false)
    {
        std::size_t actualId = isNetworked ? (NETWORKED_ID_OFFSET + entityId) : entityId;
        Entity entity = Entity::fromId(actualId);
        return entity;
    }

    /**
     * @brief Get an entity from his id (direct ID, no conversion).
     */
    Entity getEntityFromDirectID(std::size_t entityId)
    {
        return Entity::fromId(entityId);
    }

    /**
     * @brief Destroys an entity and removes all its components.
     */
    void killEntity(Entity const &entity) {
        std::size_t id = entity;
        
        // Find which list it's in
        bool isLocal = _localEntities.find(id) != _localEntities.end();
        bool isNetworked = _networkedEntities.find(id) != _networkedEntities.end();

        if (!isLocal && !isNetworked)
            return;

        // Remove from appropriate list
        if (isLocal) {
            _localEntities.erase(id);
            _freeIdsLocal.push_back(id);
        } else {
            _networkedEntities.erase(id);
            _freeIdsNetworked.push_back(id);
        }

        if (id < _signatures.size())
            _signatures[id].reset();

        if (_systemManager)
            _systemManager->entitySignatureChanged(id, Signature());

        for (auto& [key, fn] : _erasers)
            fn(*this, entity);
    }

    /**
     * @brief Checks whether an entity is alive.
     */
    bool isAlive(Entity const& e) const {
        std::size_t id = e;
        return _localEntities.find(id) != _localEntities.end() || 
               _networkedEntities.find(id) != _networkedEntities.end();
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
        std::size_t id = e;
        if (id >= _entitiesName.size())
            return "";
        return _entitiesName[id];
    }

    /**
     * @brief Adds a component to an entity.
     */
    template <class Component>
    typename ComponentManager<Component>::referenceType addComponent(Entity const &e, Component &&c)
    {
        if (!isAlive(e))
            throw Error(ErrorType::EcsInvalidEntity, ErrorMessages::ECS_INVALID_ENTITY);

        std::size_t id = e;
        std::size_t componentId = getComponentTypeID<Component>();
        
        if (id >= _signatures.size())
            _signatures.resize(id + 1);
        _signatures[id].set(componentId, true);

        LOG_DEBUG("addComponent: entity={} componentType={} componentId={} signature={}", 
                  id, typeid(Component).name(), componentId, _signatures[id].to_string());

        if (_systemManager)
            _systemManager->entitySignatureChanged(id, _signatures[id]);

        return getComponents<Component>().insertAt(id, std::forward<Component>(c));
    }

    /**
     * @brief Constructs a component in-place for an entity.
     */
    template<class Component, class... Params>
    typename ComponentManager<Component>::referenceType emplaceComponent(Entity const& e, Params&&... ps) {
        if (!isAlive(e))
            throw Error(ErrorType::EcsInvalidEntity, ErrorMessages::ECS_INVALID_ENTITY);

        std::size_t id = e;
        std::size_t componentId = getComponentTypeID<Component>();
        
        if (id >= _signatures.size())
            _signatures.resize(id + 1);
        _signatures[id].set(componentId, true);

        if (_systemManager)
            _systemManager->entitySignatureChanged(id, _signatures[id]);

        return getComponents<Component>().emplaceAt(id, std::forward<Params>(ps)...);
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

        std::size_t id = e;
        std::size_t componentId = getComponentTypeID<Component>();
        
        if (id < _signatures.size())
            _signatures[id].set(componentId, false);

        if (_systemManager)
            _systemManager->entitySignatureChanged(id, id < _signatures.size() ? _signatures[id] : Signature());

        getComponents<Component>().erase(id);
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

    // Local entity management (IDs: 1 to NETWORKED_ID_OFFSET-1)
    size_t _nextIdLocal = 1;  // Start at 1, 0 is reserved for errors
    std::vector<std::size_t> _freeIdsLocal; /**< Recycled local entity IDs */
    std::unordered_set<std::size_t> _localEntities; /**< Local entities (not networked) */

    // Networked entity management (IDs: NETWORKED_ID_OFFSET+)
    size_t _nextIdNetworked = 0;  // Relative ID, actual ID = NETWORKED_ID_OFFSET + _nextIdNetworked
    std::vector<std::size_t> _freeIdsNetworked; /**< Recycled networked entity IDs (actual IDs with offset) */
    std::unordered_set<std::size_t> _networkedEntities; /**< Networked entities (synchronized) */

    // Shared state (single unified space since IDs don't overlap)
    std::vector<Signature> _signatures; /**< Signatures per entity */
    std::vector<std::string> _entitiesName; /**< Names per entity */

    SystemManager* _systemManager = nullptr; /**< Callback target for signature updates */
};

#endif /* !ENTITYMANAGER_HPP_ */
