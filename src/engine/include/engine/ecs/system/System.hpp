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

    virtual void onCreate() {}          // appelé à l'init globale des systèmes
    virtual void onStartRunning() {}    // appelé au moment où le système commence à avoir des entités
    virtual void onUpdate(float dt) {}  // appelé chaque frame si le système a des entités
    virtual void onStopRunning() {}     // appelé quand le système n'a plus d'entités
    virtual void onDestroy() {}         // appelé à la destruction globale des systèmes

    void addEntity(Entity e);
    void removeEntity(Entity const& e);

    bool empty() const noexcept { return _entities.empty(); }
    bool isRunning() const noexcept { return _running; }

protected:
    std::vector<Entity> _entities;
    bool _running { false };
};

#endif /* !SYSTEM_HPP_ */
