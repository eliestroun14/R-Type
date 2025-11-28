# Component Manager

The `ComponentManager<Component>` class is responsible for **storing** and **managing** all instances of a specific **component type**. It uses a sparse-set-like approach where components are stored in a vector indexed by entity ID, with `std::optional` wrapping each slot to represent presence or absence.

## Class Overview

`ComponentManager` is a **template class** that wraps a `std::vector<std::optional<Component>>`. **Each index** in the vector corresponds to an **entity ID**. If an entity has the component, its slot contains a value; **otherwise**, it holds `std::nullopt`.

Here is the class:

```c++
template <typename Component>
class ComponentManager {
    public:
        using valueType = std::optional<Component>;
        using referenceType = valueType &;
        using constReferenceType = valueType const&;
        using containerT = std::vector<valueType>;
        using sizeType = typename containerT::sizeType;
        using iterator = typename containerT::iterator;
        using constIterator = typename containerT::constIterator;

    public:
        ComponentManager() = default;
        ComponentManager(ComponentManager const &) = default;
        ComponentManager(ComponentManager &&) noexcept = default;
        ~ComponentManager() = default;
        ComponentManager & operator=(ComponentManager const &) = default;
        ComponentManager & operator=(ComponentManager &&) noexcept = default;

        referenceType operator[](size_t idx);
        constReferenceType operator[](size_t idx) const;

        iterator begin();
        constIterator begin() const;
        constIterator cbegin() const;
        iterator end();
        constIterator end() const;
        constIterator cend() const;

        sizeType size() const;

        void ensureSize(sizeType pos);

        referenceType insertAt(sizeType pos, Component const& c);
        referenceType insertAt(sizeType pos, Component &&c);

        template <class... Params>
        referenceType emplaceAt(sizeType pos, Params &&... args);

        void erase(sizeType pos);

        sizeType getIndex(valueType const& v) const;

    private:
        containerT _data;
};
```

## Types Aliases

The class defines several type **aliases** to match standard container conventions:

`valueType`: `std::optional<Component>` — the stored element type.
`referenceType`: `std::optional<Component>&` — mutable reference to an element.
`constReferenceType`: const `std::optional<Component>&` — const reference to an element.
`containerT`: `std::vector<std::optional<Component>>` — the underlying storage.
`sizeType`: `std::vector::sizeType` — unsigned integer type for sizes and indices.
`iterator` and `constIterator`: standard iterator types from the vector.

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
referenceType operator[](size_t idx);
constReferenceType operator[](size_t idx) const;
```
Provides direct access to the `std::optional<Component>` at the given index. No bounds checking is performed; the caller must ensure the index is valid.

`size()`

```c++
sizeType size() const;
```
Returns the **current size** of the internal vector, which reflects the **highest entity ID** that has been allocated a slot.

#### Iteration

```c++
iterator begin();
constIterator begin() const;
constIterator cbegin() const;

iterator end();
constIterator end() const;
constIterator cend() const;
```
Standard iterator interface for traversing all slots. Each slot is a `std::optional<Component>`, so **you must** check `has_value()` before accessing the component.

## Modifying Components

`ensureSize(sizeType pos)`

```c++
void ensureSize(sizeType pos);
```
Resizes the internal vector if necessary **to ensure that index** `pos` **is valid**. If `pos` is beyond the current size, the vector grows to `pos + 1` elements, filling new slots with `std::nullopt`.

`insertAt(sizeType pos, Component)`

```c++
referenceType insertAt(sizeType pos, Component const& c);
referenceType insertAt(sizeType pos, Component &&c);
```
Assigns a component to the slot at index `pos`. The **first overload copies the component**; **the second moves it**. Both call `ensureSize(pos)` to **guarantee the index exists**. Returns a reference to the updated slot.

`emplaceAt(sizeType pos, Params&&... args)`

```c++
template <class... Params>
referenceType emplaceAt(sizeType pos, Params &&... args);
```
*Constructs a component directly in the slot* at index `pos` using perfect forwarding. The slot is reset before emplacement to clear any previous value. Returns a reference to the newly constructed component.

`erase(sizeType pos)`
```c++
void erase(sizeType pos);
```
**Clears the component** at index `pos` by calling `reset()` on the `std::optional`. The slot becomes empty (**`std::nullopt`**) **but remains in the vector**.

`getIndex(valueType const& v)`
```c++
sizeType getIndex(valueType const& v) const;
```
**Returns the index (entity ID)** of a given `std::optional<Component>` reference by computing pointer offset from the start of the internal vector. This is useful when iterating by reference and needing **to know which entity owns the component**.

## Example Usage

### Define Component

```c++
struct Health
{
    int current;
    int max;

    // Constructor is needed to use emplaceAt() method
    Health(int curr, int m) : current(curr), max(m) {}
};
```

### Using ComponentManager Directly

```c++
int main()
{
    ComponentManager<Health> health_mgr;

    // Add health to entity 0
    health_mgr.insertAt(0, Health{100, 100});

    // Add health to entity 5 (automatically resizes)
    health_mgr.emplaceAt(5, 50, 100);

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