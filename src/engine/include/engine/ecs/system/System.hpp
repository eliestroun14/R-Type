/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** System
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include "../entity/Entity.hpp"
#include <vector>

class System {
public:
    using entity_type = Entity;

    System() = default;
    virtual ~System() = default;

    virtual void onCreate() {}
    virtual void onStartRunning() {}
    virtual void onUpdate(float dt) {}
    virtual void onStopRunning() {}
    virtual void onDestroy() {}

    void addEntity(Entity e);
    void removeEntity(Entity const& e);

    bool empty() const noexcept { return _entities.empty(); }
    bool isRunning() const noexcept { return _running; }

protected:
    std::vector<Entity> _entities;
    bool _running { false };
};

#endif /* !SYSTEM_HPP_ */
