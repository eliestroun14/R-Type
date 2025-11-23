/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** SystemManager
*/

#ifndef SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_HPP_

#include <System.hpp>
#include <map>
#include <typeindex>

class SystemManager {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

        void addSystem();

        void deleteSystem(System& system);

        System getSystem();

    private:
        std::map<std::type_index, System> _systems;
};

#endif /* !SYSTEMMANAGER_HPP_ */
