/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** ComponentManager
*/

#ifndef COMPONENTMANAGER_HPP_
#define COMPONENTMANAGER_HPP_

template <typename Component> // You can also mirror the definition of std::vector, that takes an additional allocator.

class ComponentManager {
    public :
        using value_type = std::optional<Component>; // optional component type
        using reference_type = value_type &;
        using const_reference_type = value_type const&;
        using container_t = std::vector<value_type>; // optionally add your allocator template here.
        using size_type = typename container_t::size_type;

        using iterator = typename container_t::iterator;
        using const_iterator = typename container_t::const_iterator;

    public :
        ComponentManager () = default; // You can add more constructors .

        ComponentManager (ComponentManager const &) = default; // copy constructor
        ComponentManager (ComponentManager &&) noexcept = default; // move constructor
        ~ComponentManager () = default;

        ComponentManager & operator=(ComponentManager const &) = default; // copy assignment operator
        ComponentManager & operator=(ComponentManager &&) noexcept = default; // move assignment operator

        // Access
        reference_type operator[](size_t idx) {
            return this->_data[idx];
        }

        const_reference_type operator[](size_t idx) const {
            return this->_data[idx];
        }

        iterator begin () {
            return this->_data.begin();
        }
        const_iterator begin () const {
            return this->_data.begin();
        }
        const_iterator cbegin () const {
            return this->_data.cbegin();
        }

        iterator end () {
            return this->_data.end();
        }
        const_iterator end () const {
            return this->_data.end();
        }
        const_iterator cend () const {
            return this->_data.cend();
        }

        size_type size () const {
            return this->_data.size();
        }

        // sert à garantir que l’index demandé existe dans le vecteur avant d’y écrire
        void ensure_size(size_type pos) {
            if (pos >= this->_data.size())
                this->_data.resize(pos + 1);
        }

        reference_type insert_at (size_type pos, Component const& c) {
            ensure_size(pos); // pour agrandir
            this->_data[pos] = c;
            return this->_data[pos]; // rajouter dans le vector
        }

        reference_type insert_at (size_type pos, Component &&c) {
            ensure_size(pos); // pour agrandir
            this->_data[pos] = std::move(c);
            return this->_data[pos]; // rajouter dans le vector
        }

        template <class... Params>
        reference_type emplace_at (size_type pos, Params &&... args) {
            ensure_size(pos);
            this->_data[pos].reset();
            this->_data[pos].emplace(std::forward<Params>(args)...);
            return this->_data[pos];
        }

        void erase (size_type pos) {
            if (this->_data[pos])
                this->_data[pos].reset();
        }
        size_type get_index (value_type const& v) const {
            return &v - this->_data.data();
        }

    private :
        container_t _data;
};

#endif /* !COMPONENTMANAGER_HPP_ */
