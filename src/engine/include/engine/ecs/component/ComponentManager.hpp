/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ComponentManager
*/

#ifndef COMPONENTMANAGER_HPP_
#define COMPONENTMANAGER_HPP_

#include <optional>
#include <vector>
#include <cstddef>

template <typename Component>

class ComponentManager {
    public :
        using valueType = std::optional<Component>; // optional component type
        using referenceType = valueType &;
        using constReferenceType = valueType const&;
        using containerT = std::vector<valueType>; // optionally add your allocator template here.
        using sizeType = typename containerT::size_type;

        using iterator = typename containerT::iterator;
        using constIterator = typename containerT::const_iterator;

    public :
        ComponentManager () = default; // You can add more constructors .

        ComponentManager (ComponentManager const &) = default; // copy constructor
        ComponentManager (ComponentManager &&) noexcept = default; // move constructor
        ~ComponentManager () = default;

        ComponentManager & operator=(ComponentManager const &) = default; // copy assignment operator
        ComponentManager & operator=(ComponentManager &&) noexcept = default; // move assignment operator

        // Access
        referenceType operator[](size_t idx) {
            return this->_data[idx];
        }

        constReferenceType operator[](size_t idx) const {
            return this->_data[idx];
        }

        iterator begin () {
            return this->_data.begin();
        }
        constIterator begin () const {
            return this->_data.begin();
        }
        constIterator cbegin () const {
            return this->_data.cbegin();
        }

        iterator end () {
            return this->_data.end();
        }
        constIterator end () const {
            return this->_data.end();
        }
        constIterator cend () const {
            return this->_data.cend();
        }

        sizeType size () const {
            return this->_data.size();
        }

        void ensureSize(sizeType pos) {
            if (pos >= this->_data.size())
                this->_data.resize(pos + 1);
        }

        referenceType insertAt (sizeType pos, Component const& c) {
            ensureSize(pos);
            this->_data[pos] = c;
            return this->_data[pos];
        }

        referenceType insertAt (sizeType pos, Component &&c) {
            ensureSize(pos);
            this->_data[pos] = std::move(c);
            return this->_data[pos];
        }

        template <class... Params>
        referenceType emplaceAt (sizeType pos, Params &&... args) {
            ensureSize(pos);
            this->_data[pos].reset();
            this->_data[pos].emplace(std::forward<Params>(args)...);
            return this->_data[pos];
        }

        void erase (sizeType pos) {
            if (this->_data[pos])
                this->_data[pos].reset();
        }
        sizeType getIndex (valueType const& v) const {
            return &v - this->_data.data();
        }

    private :
        containerT _data;
};

#endif /* !COMPONENTMANAGER_HPP_ */
