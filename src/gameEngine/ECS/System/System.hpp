/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** System
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include "Entity.hpp"
#include <list>

class System {
    public:
        System() = default;
        ~System() = default;

        void onCreate(); // When system is create

        void onStartRunning(); // Before the first onUpdate(), everytime the system launch

        void onUpdate(); // every frame, if system find entities

        void onStopRunning(); // when system does not find any entities

        void onDestroy(); // when system is destroyed

    private:
        std::list<Entity> _entities;
};

#endif /* !SYSTEM_HPP_ */
