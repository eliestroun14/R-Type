/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** System
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <vector>
#include <algorithm>


class System {
public:
    System() = default;
    virtual ~System() = default;

    virtual void onCreate() {}
    virtual void onStartRunning() {}
    virtual void onUpdate(float dt) {}
    virtual void onStopRunning() {}
    virtual void onDestroy() {}

    bool isRunning() const noexcept { return _running; }

    size_t entityCount() const noexcept { return _entities.size(); }

    bool hasEntity(size_t id) const noexcept {
        return std::find(_entities.begin(), _entities.end(), id) != _entities.end();
    }

    friend class SystemManager;

protected:
    std::vector<size_t> _entities;
    bool _running { false };

};


#endif /* !SYSTEM_HPP_ */
