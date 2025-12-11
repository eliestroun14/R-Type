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

/**
 * @class System
 * @brief Base class for all ECS systems.
 *
 * A System maintains a list of entity IDs and defines a lifecycle interface
 * allowing the SystemManager to control creation, execution, and destruction.
 *
 * Responsibilities:
 *  - store the list of entities matching the system's signature;
 *  - expose lifecycle callbacks invoked by SystemManager;
 *  - provide utility methods to query entity membership.
 *
 * Systems do not store components and do not directly modify entity signatures.
 */
class System {
public:
    /**
     * @brief Default constructor.
     */
    System() = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~System() = default;

    /**
     * @brief Called once when the system is created.
     */
    virtual void onCreate() {}

    /**
     * @brief Called once before the system begins active execution.
     */
    virtual void onStartRunning() {}

    /**
     * @brief Called every update cycle.
     * @param dt Delta time.
     */
    virtual void onUpdate(float dt) {}

    /**
     * @brief Called once when the system stops running.
     */
    virtual void onStopRunning() {}

    /**
     * @brief Called once when the system is destroyed.
     */
    virtual void onDestroy() {}

    /**
     * @brief Returns whether the system is currently running.
     * @return True if running.
     */
    bool isRunning() const noexcept { return _running; }

    /**
     * @brief Returns the number of entities tracked by the system.
     * @return Entity count.
     */
    size_t entityCount() const noexcept { return _entities.size(); }

    /**
     * @brief Checks whether a given entity is contained in the system.
     * @param id Entity ID.
     * @return True if present.
     */
    bool hasEntity(size_t id) const noexcept {
        return std::find(_entities.begin(), _entities.end(), id) != _entities.end();
    }

    friend class SystemManager;

protected:
    std::vector<size_t> _entities;  /**< List of entity IDs matching the system’s signature */
    bool _running { false };        /**< Indicates whether the system is currently running */
};

#endif /* !SYSTEM_HPP_ */
