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
    // conversion implicite vers size_t
        operator std::size_t() const {
            return this->_id;
        }

    private:
        std::size_t _id;
        std::string _name; //TODO: revoir si on met un name à notre entity

        // constructeur privé : seul le EntityManager peut créer une entité
        explicit Entity(std::size_t id, std::string name)
            : _id(id), _name(name) {}

        // friend du EntityManager
        friend class EntityManager;
};

#endif /* !ENTITY_HPP_ */
