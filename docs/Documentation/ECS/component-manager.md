# Component Manager

The `ComponentManager<Component>` class is responsible for **storing** and **managing** all instances of a specific **component type**. It uses a sparse-set-like approach where components are stored in a vector indexed by entity ID, with `std::optional` wrapping each slot to represent presence or absence.

## Class Overview

`ComponentManager` is a **template class** that wraps a `std::vector<std::optional<Component>>`. **Each index** in the vector corresponds to an **entity ID**. If an entity has the component, its slot contains a value; **otherwise**, it holds `std::nullopt`.

Here is the class:

```c++
template <typename Component>
class ComponentManager {
    public:
        using value_type = std::optional<Component>;
        using reference_type = value_type &;
        using const_reference_type = value_type const&;
        using container_t = std::vector<value_type>;
        using size_type = typename container_t::size_type;
        using iterator = typename container_t::iterator;
        using const_iterator = typename container_t::const_iterator;

    public:
        ComponentManager() = default;
        ComponentManager(ComponentManager const &) = default;
        ComponentManager(ComponentManager &&) noexcept = default;
        ~ComponentManager() = default;
        ComponentManager & operator=(ComponentManager const &) = default;
        ComponentManager & operator=(ComponentManager &&) noexcept = default;

        reference_type operator[](size_t idx);
        const_reference_type operator[](size_t idx) const;

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

        size_type size() const;

        void ensure_size(size_type pos);

        reference_type insert_at(size_type pos, Component const& c);
        reference_type insert_at(size_type pos, Component &&c);

        template <class... Params>
        reference_type emplace_at(size_type pos, Params &&... args);

        void erase(size_type pos);

        size_type get_index(value_type const& v) const;

    private:
        container_t _data;
};
```

## Types Aliases

The class defines several type **aliases** to match standard container conventions:

`value_type`: `std::optional<Component>` — the stored element type.
`reference_type`: `std::optional<Component>&` — mutable reference to an element.
`const_reference_type`: const `std::optional<Component>&` — const reference to an element.
`container_t`: `std::vector<std::optional<Component>>` — the underlying storage.
`size_type`: `std::vector::size_type` — unsigned integer type for sizes and indices.
`iterator` and `const_iterator`: standard iterator types from the vector.

## Construction and Assignment

`ComponentManager` supports default construction, copy, and move semantics:

```c++
ComponentManager() = default;
ComponentManager(ComponentManager const &) = default;
ComponentManager(ComponentManager &&) noexcept = default;
~ComponentManager() = default;
ComponentManager & operator=(ComponentManager const &) = default;
ComponentManager & operator=(ComponentManager &&) noexcept = default;
```
These defaults allow `ComponentManager` to be stored in containers like `std::any` and moved efficiently.

## Access Operations

`operator[]`

```c++
reference_type operator[](size_t idx);
const_reference_type operator[](size_t idx) const;
```
Provides direct access to the `std::optional<Component>` at the given index. No bounds checking is performed; the caller must ensure the index is valid.

`size()`

```c++
size_type size() const;
```
Returns the **current size** of the internal vector, which reflects the **highest entity ID** that has been allocated a slot.

#### Iteration

```c++
iterator begin();
const_iterator begin() const;
const_iterator cbegin() const;

iterator end();
const_iterator end() const;
const_iterator cend() const;
```
Standard iterator interface for traversing all slots. Each slot is a `std::optional<Component>`, so **you must** check `has_value()` before accessing the component.

## Modifying Components

`ensure_size(size_type pos)`

```c++
void ensure_size(size_type pos);
```
Resizes the internal vector if necessary **to ensure that index** `pos` **is valid**. If `pos` is beyond the current size, the vector grows to `pos + 1` elements, filling new slots with `std::nullopt`.

`insert_at(size_type pos, Component)`

```c++
reference_type insert_at(size_type pos, Component const& c);
reference_type insert_at(size_type pos, Component &&c);
```
Assigns a component to the slot at index `pos`. The **first overload copies the component**; **the second moves it**. Both call `ensure_size(pos)` to **guarantee the index exists**. Returns a reference to the updated slot.

`emplace_at(size_type pos, Params&&... args)`

```c++
template <class... Params>
reference_type emplace_at(size_type pos, Params &&... args);
```
*Constructs a component directly in the slot* at index `pos` using perfect forwarding. The slot is reset before emplacement to clear any previous value. Returns a reference to the newly constructed component.

`erase(size_type pos)`
```c++
void erase(size_type pos);
```
**Clears the component** at index `pos` by calling `reset()` on the `std::optional`. The slot becomes empty (**`std::nullopt`**) **but remains in the vector**.

`get_index(value_type const& v)`
```c++
size_type get_index(value_type const& v) const;
```
**Returns the index (entity ID)** of a given `std::optional<Component>` reference by computing pointer offset from the start of the internal vector. This is useful when iterating by reference and needing **to know which entity owns the component**.

## Example Usage

### Define Component

```c++
struct Health
{
    int current;
    int max;

    // Constructor is needed to use emplace_at() method
    Health(int curr, int m) : current(curr), max(m) {}
};
```

### Using ComponentManager Directly

```c++
int main()
{
    ComponentManager<Health> health_mgr;

    // Add health to entity 0
    health_mgr.insert_at(0, Health{100, 100});

    // Add health to entity 5 (automatically resizes)
    health_mgr.emplace_at(5, 50, 100);

    // Access component
    auto& health0 = health_mgr[0];
    if (health0.has_value()) {
        health0->current -= 10;
    }

    // Iterate over all components
    for (std::size_t id = 0; id < health_mgr.size(); ++id) {
        auto& health = health_mgr[id];
        if (!health.has_value())
            continue;

        std::cout << "Entity " << id << " has "
                  << health->current << "/" << health->max << " HP\n";
    }

    // Remove component from entity 5
    health_mgr.erase(5);

    return 0;
}
```

This example demonstrates component insertion, access, iteration, and removal using `ComponentManager` **as a standalone container**. **In practice**, `EntityManager` wraps `ComponentManager` **instances and provides higher-level access patterns.**

## Integration with EntityManager

`EntityManager` **stores one** `ComponentManager<T>` **per registered component type** inside a `std::unordered_map<std::type_index, std::any>`. When you call `register_component<T>()`, **it creates a** `ComponentManager<T>` **and wraps it in** `std::any`. All subsequent component operations extract the manager using `std::any_cast` and forward calls to the appropriate `ComponentManager` methods.

This architecture keeps **component storage contiguous and type-safe while allowing `EntityManager` to manage multiple component types dynamically**.