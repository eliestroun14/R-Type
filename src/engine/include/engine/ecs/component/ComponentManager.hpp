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

/**
 * @class ComponentManager
 * @brief Stores and manages all components of a specific type.
 *
 * Responsibilities:
 *  - maintain a dense array of optional components indexed by entity ID;
 *  - automatically expand when needed;
 *  - provide insertion, emplacement, retrieval, and removal operations;
 *  - offer iterator access over all component slots.
 *
 * This class contains no logic related to systems or signatures.
 *
 * @tparam Component The type of component stored.
 */
template <typename Component>
class ComponentManager {
public:
    using valueType = std::optional<Component>;        /**< Optional component slot */
    using referenceType = valueType&;                  /**< Mutable reference to a component slot */
    using constReferenceType = const valueType&;       /**< Const reference to a component slot */
    using containerT = std::vector<valueType>;         /**< Internal container type */
    using sizeType = typename containerT::size_type;   /**< Size/index type */

    using iterator = typename containerT::iterator;            /**< Iterator over component slots */
    using constIterator = typename containerT::const_iterator; /**< Const iterator */

public:
    /** @brief Default constructor */
    ComponentManager() = default;

    /** @brief Copy constructor */
    ComponentManager(const ComponentManager&) = default;

    /** @brief Move constructor */
    ComponentManager(ComponentManager&&) noexcept = default;

    /** @brief Destructor */
    ~ComponentManager() = default;

    /** @brief Copy assignment */
    ComponentManager& operator=(const ComponentManager&) = default;

    /** @brief Move assignment */
    ComponentManager& operator=(ComponentManager&&) noexcept = default;

    /**
     * @brief Access a component slot by index.
     * @param idx Entity ID.
     * @return Reference to the optional component at this index.
     */
    referenceType operator[](size_t idx) {
        ensureSize(idx);
        return _data[idx];
    }

    /**
     * @brief Const-qualified access to a component slot.
     */
    constReferenceType operator[](size_t idx) const {
        if (idx >= _data.size()) {
            static std::optional<Component> empty;
            return empty;
        }
        return _data[idx];
    }

    /** @return Iterator to the beginning of the internal storage */
    iterator begin() { return _data.begin(); }

    /** @return Const iterator to the beginning of the internal storage */
    constIterator begin() const { return _data.begin(); }

    /** @return Const iterator to the beginning of the internal storage */
    constIterator cbegin() const { return _data.cbegin(); }

    /** @return Iterator to the end of the internal storage */
    iterator end() { return _data.end(); }

    /** @return Const iterator to the end of the internal storage */
    constIterator end() const { return _data.end(); }

    /** @return Const iterator to the end of the internal storage */
    constIterator cend() const { return _data.cend(); }

    /**
     * @brief Returns the number of allocated component slots.
     */
    sizeType size() const {
        return _data.size();
    }

    /**
     * @brief Ensures the internal vector can hold at least pos+1 entries.
     * @param pos Required index.
     */
    void ensureSize(sizeType pos) {
        if (pos >= _data.size())
            _data.resize(pos + 1);
    }

    /**
     * @brief Inserts a component by copy at a given index.
     * @param pos Entity ID.
     * @param c Component to copy.
     * @return Reference to the optional component slot.
     */
    referenceType insertAt(sizeType pos, const Component& c) {
        ensureSize(pos);
        _data[pos] = c;
        return _data[pos];
    }

    /**
     * @brief Inserts a component by move at a given index.
     * @param pos Entity ID.
     * @param c Component to move.
     * @return Reference to the optional component slot.
     */
    referenceType insertAt(sizeType pos, Component&& c) {
        ensureSize(pos);
        _data[pos] = std::move(c);
        return _data[pos];
    }

    /**
     * @brief Constructs a component in-place at a given index.
     * @tparam Params Constructor parameter types.
     * @param pos Entity ID.
     * @param args Arguments forwarded to the component constructor.
     * @return Reference to the optional component slot.
     */
    template <class... Params>
    referenceType emplaceAt(sizeType pos, Params&&... args) {
        ensureSize(pos);
        _data[pos].reset();
        _data[pos].emplace(std::forward<Params>(args)...);
        return _data[pos];
    }

    /**
     * @brief Removes the component at the given index.
     * @param pos Entity ID.
     */
    void erase(sizeType pos) {
        if (_data[pos])
            _data[pos].reset();
    }

    /**
     * @brief Computes the array index of a component slot.
     * @param v Reference to a component slot.
     * @return The index of the slot inside the internal vector.
     */
    sizeType getIndex(const valueType& v) const {
        return &v - _data.data();
    }

private:
    containerT _data; /**< Internal storage for optional components */
};

#endif /* !COMPONENTMANAGER_HPP_ */
