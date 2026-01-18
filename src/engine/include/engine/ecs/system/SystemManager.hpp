/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** SystemManager
*/

#ifndef SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/Signature.hpp>
#include <common/error/Error.hpp>
#include <common/logger/Logger.hpp>

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>
#include <algorithm>

class EntityManager;

/**
 * @class SystemManager
 * @brief Centralizes the management of systems.
 *
 * A SystemManager:
 *  - stores all registered systems;
 *  - maintains a required signature for each system;
 *  - automatically updates the list of entities inside each system when an entity’s signature changes;
 *  - invokes lifecycle callbacks (create, destroy, update) on all systems.
 */
class SystemManager {

public:
    SystemManager() = default;
    ~SystemManager() = default;

    /**
     * @brief Links this SystemManager to an EntityManager.
     */
    void setEntityManager(EntityManager* em) { _entityManager = em; }

    /**
     * @brief Adds a system to the manager.
     * @tparam S System type derived from System.
     * @tparam Args Forwarded arguments passed to the system constructor.
     * @return Reference to the newly added system.
     * @throws ErrorType::EcsDuplicateSystem if the system is already registered.
     */
    template <class S, class... Args>
    S& addSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<System, S>, "S must derive from System");

        std::type_index key(typeid(S));
        auto [it, inserted] = _systems.try_emplace(key, std::make_unique<S>(std::forward<Args>(args)...));
        if (!inserted)
            throw Error(ErrorType::EcsDuplicateSystem, ErrorMessages::ECS_DUPLICATE_SYSTEM);
        return static_cast<S&>(*it->second);
    }

    /**
     * @brief Removes a system from the manager.
     * @tparam S The type of the system to remove.
     */
    template <class S>
    void deleteSystem()
    {
        std::type_index key(typeid(S));
        _systems.erase(key);
    }

    /**
     * @brief Retrieves a system by type.
     * @tparam S System type.
     * @throws ErrorType::EcsInvalidSystem if the system does not exist.
     */
    template <class S>
    S& getSystem()
    {
        std::type_index key(typeid(S));
        auto it = _systems.find(key);
        if (it == _systems.end())
            throw Error(ErrorType::EcsInvalidSystem, ErrorMessages::ECS_SYSTEM_NOT_FOUND);
        return static_cast<S&>(*it->second);
    }

    /**
     * @brief Const version of getSystem().
     * @throws ErrorType::EcsInvalidSystem if the system does not exist.
     */
    template <class S>
    const S& getSystem() const
    {
        std::type_index key(typeid(S));
        auto it = _systems.find(key);
        if (it == _systems.end())
            throw Error(ErrorType::EcsInvalidSystem, ErrorMessages::ECS_SYSTEM_NOT_FOUND);
        return static_cast<const S&>(*it->second);
    }

    /**
     * @brief Checks whether a system exists.
     * @tparam S System type.
     */
    template <class S>
    bool hasSystem() const
    {
        std::type_index key(typeid(S));
        return _systems.find(key) != _systems.end();
    }

    /**
     * @brief Sets the required signature of a system.
     * @tparam S System type.
     * @param sig Signature to assign.
     * @throws ErrorType::EcsInvalidSystem if the system does not exist.
     */
    template<class S>
    void setSignature(const Signature& sig)
    {
        if (!hasSystem<S>())
            throw Error(ErrorType::EcsInvalidSystem, ErrorMessages::ECS_SYSTEM_NOT_FOUND);
        _signatures[typeid(S)] = sig;
    }

    /**
     * @brief Notifies the SystemManager that an entity's signature has changed.
     *
     * This method:
     *  - checks whether the entity matches the signature of each system;
     *  - adds or removes the entity from each system accordingly.
     *
     * @param entity Entity ID.
     * @param entitySig Signature of the entity.
     */
    void entitySignatureChanged(size_t entity, const Signature& entitySig)
    {
        for (auto& [type, system] : _systems) {

            auto it = _signatures.find(type);
            if (it == _signatures.end())
                throw Error(ErrorType::EcsMissingSignature, ErrorMessages::ECS_MISSING_SIGNATURE);

            const Signature& systemSig = it->second;
            bool matches = (entitySig & systemSig) == systemSig;
            
            LOG_DEBUG("entitySignatureChanged: entity={} system={} entitySig={} systemSig={} matches={}",
                      entity, type.name(), entitySig.to_string(), systemSig.to_string(), matches);

            if (matches)
                addEntityToSystem(system.get(), entity);
            else
                removeEntityFromSystem(system.get(), entity);
        }
    }

    /**
     * @brief Calls onCreate() on all systems.
     */
    void onCreateAll()
    {
        for (auto& [_, sys] : _systems)
            sys->onCreate();
    }

    /**
     * @brief Calls onDestroy() on all systems.
     */
    void onDestroyAll()
    {
        for (auto& [_, sys] : _systems)
            sys->onDestroy();
    }

    /**
     * @brief Calls onStartRunning() on all systems.
     */
    void onStartRunningAll()
    {
        for (auto& [_, sys] : _systems) {
            sys->_running = true;
            sys->onStartRunning();
        }
    }

    /**
     * @brief Calls onStopRunning() on all systems.
     */
    void onStopRunningAll()
    {
        for (auto& [_, sys] : _systems) {
            sys->_running = false;
            sys->onStopRunning();
        }
    }

    /**
     * @brief Updates all systems.
     * @param dt Delta time.
     */
    void updateAll(float dt)
    {
        for (auto& [_, sys] : _systems)
            sys->onUpdate(dt);
    }

private:

    /**
     * @brief Adds an entity to a system if not already present.
     */
    void addEntityToSystem(System* sys, size_t entity)
    {
        auto& vec = sys->_entities;
        if (std::find(vec.begin(), vec.end(), entity) == vec.end())
            vec.push_back(entity);
    }

    /**
     * @brief Removes an entity from a system.
     */
    void removeEntityFromSystem(System* sys, size_t entity)
    {
        auto& vec = sys->_entities;
        vec.erase(std::remove(vec.begin(), vec.end(), entity), vec.end());
    }

private:
    EntityManager* _entityManager = nullptr;                           /**< Linked EntityManager */
    std::unordered_map<std::type_index, std::unique_ptr<System>> _systems;   /**< All registered systems */
    std::unordered_map<std::type_index, Signature> _signatures;             /**< Required signatures for each system */
};

#endif /* !SYSTEMMANAGER_HPP_ */
