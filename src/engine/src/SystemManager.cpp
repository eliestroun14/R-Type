/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** SystemManager
*/

#ifndef SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_HPP_

#include "System.hpp"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>

class SystemManager {
public:
    SystemManager() = default;
    ~SystemManager() = default;

    template <class S, class... Args>
    S& addSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<System, S>, "S must derive from System");

        std::type_index key(typeid(S));
        auto [it, inserted] = _systems.try_emplace(
            key,
            std::make_unique<S>(std::forward<Args>(args)...)
        );

        return static_cast<S&>(*it->second);
    }

    template <class S>
    void deleteSystem()
    {
        std::type_index key(typeid(S));
        _systems.erase(key);
    }

    template <class S>
    S& getSystem()
    {
        std::type_index key(typeid(S));
        auto it = _systems.find(key);
        if (it == _systems.end())
            throw std::runtime_error("SystemManager::getSystem: system not found");
        return static_cast<S&>(*it->second);
    }

    template <class S>
    const S& getSystem() const
    {
        std::type_index key(typeid(S));
        auto it = _systems.find(key);
        if (it == _systems.end())
            throw std::runtime_error("SystemManager::getSystem: system not found");
        return static_cast<const S&>(*it->second);
    }

    template <class S>
    bool hasSystem() const
    {
        std::type_index key(typeid(S));
        return _systems.find(key) != _systems.end();
    }

    void onCreateAll()
    {
        for (auto& [_, sys] : _systems)
            sys->onCreate();
    }

    void onDestroyAll()
    {
        for (auto& [_, sys] : _systems)
            sys->onDestroy();
    }

    void updateAll(float dt)
    {
        for (auto& [_, sys] : _systems) {
            if (!sys->empty())
                sys->onUpdate(dt);
        }
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<System>> _systems;
};

#endif /* !SYSTEMMANAGER_HPP_ */