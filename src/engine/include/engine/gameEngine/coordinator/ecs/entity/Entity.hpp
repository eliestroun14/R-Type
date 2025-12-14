/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <iostream>

class EntityManager;

class Entity {
    public:
        operator std::size_t() const {
            return this->_id;
        }

        static Entity fromId(std::size_t id)
        {
            return Entity(id);
        }

    private:
        std::size_t _id;

        explicit Entity(std::size_t id) : _id(id) {}

        friend class EntityManager;
};

#endif /* !ENTITY_HPP_ */