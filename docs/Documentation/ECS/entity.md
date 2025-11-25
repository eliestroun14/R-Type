# Entity

In our ECS, we have two classes reserved to entities. We have the `Entity` class and the `EntityManager` class.

## `Entity` class

An `Entity` simply an ID with a name for debugging purposes. **It does not contain anything.**

Here the class:

```c++
class Entity {
    public:
        operator std::size_t() const {
            return this->_id;
        }

    private:
        std::size_t _id;
        std::string _name;

        // private constructor: only the EntityManager is able to create new entity
        explicit Entity(std::size_t id, std::string name)
            : _id(id), _name(name) {}

        // friend of EntityManager
        friend class EntityManager;
};
```

### Design Principles
The `Entity` class follows a strict encapsulation pattern:

- **Private constructor**: Only `EntityManager` can instantiate entities.
- **Friend declaration**: `EntityManager` has privileged access to create entities.
- **Implicit conversion**: The `operator std::size_t()` allows entities to be used directly as indices.

This design ensures that all entity creation and lifecycle management flows through **a single point of control**.

```c++
explicit Entity(std::size_t id, std::string name) : _id(id), _name(name) {}

friend class EntityManager;
```

## `EntityManager` Class

`EntityManager` **owns the complete lifecycle of every entity in the system**. It allocates IDs, reclaims them, coordinates all component storage, and provides the only interface for entity and component operations.

Here is the class:

```c++
class EntityManager {
    public :
        EntityManager() = default;

        template<class Component>
        ComponentManager<Component>& register_component() {}


        template <class Component>
        ComponentManager<Component> &get_components() {}

        template <class Component>
        ComponentManager<Component> const &get_components() const {}


        template<class Component>
        std::optional<Component>& get_component(Entity const& e) {}

        template<class Component>
        const std::optional<Component>& get_component(Entity const& e) const {}


        Entity spawn_entity(std::string name) {}

        void kill_entity(Entity const &entity) {}

        bool is_alive(Entity const& e) const {}

        std::string getEntityName(Entity const& e) const {}


        template <class Component>
        bool has_component(Entity const& e) const {}

        template <class Component>
        typename ComponentManager<Component>::reference_type add_component(Entity const &e, Component &&c) {}

        template<class Component, class... Params>
        typename ComponentManager<Component>::reference_type emplace_component(Entity const& e, Params&&... ps) {}

        template<class Component>
        void remove_component(Entity const& e) {}


    private:
        std::unordered_map<std::type_index, std::any> _components_arrays;
        std::unordered_map<std::type_index, std::function<void(EntityManager&,Entity const&)>> _erasers;

        size_t _next_id = 0;
        std::vector<std::size_t> _free_ids;

        std::unordered_set<std::size_t> _alive_entities;
};
```

### Core responsabilities

`EntityManager` performs four primary tasks:

1. **Entity lifecycle management**: Create and destroy entities with ID allocation and recycling.
2. **Component type registration**: Maintain one ComponentManager<T> per registered component type.
3. **Component operations**: Attach, construct, access, and remove components for any entity.
4. **Entity tracking**: Monitor which entity IDs are alive and recycle dead ones.

### Internal Storage
The class maintains **several data structures**:

- `_components_arrays`: Maps `std::type_index` to `std::any`-wrapped `ComponentManager<T>` instances, one per component type.
- `_erasers`: Maps `std::type_index` to cleanup functions that remove components when an entity is destroyed.
- `_next_id`: Counter for issuing fresh entity IDs.
- `_free_ids`: Pool of recycled IDs from destroyed entities.
- `_alive_entities`: Set of currently active entity IDs.

**Each component type** is stored in a dedicated `ComponentManager<T>`. When registering a component type, an eraser function is also installed to ensure proper cleanup when entities are destroyed.

### Entity Lifecycle Operations

`spawn_entity(std::string name)`
```c++
Entity spawn_entity(std::string name);
```
**Creates a new entity with the given name**. If recycled IDs are available in `_free_ids`, one is reused; otherwise, a fresh ID is issued from `_next_id`. The ID is recorded in `_alive_entities` and returned as an Entity object.


`kill_entity(Entity const& entity)`
```c++
void kill_entity(Entity const& entity);
```
**Destroys an entity** by removing its ID from `_alive_entities`, returning the ID to `_free_ids` for recycling, and invoking every registered eraser function to remove all of its components across all component types.

`is_alive(Entity const& e)`
```c++
bool is_alive(Entity const& e) const;
```
Checks whether the given entity ID is present in the set of alive entities. Returns `true` **if the entity is active**, `false` otherwise.

`getEntityName(Entity const& e)`
```c++
std::string getEntityName(Entity const& e) const;
```
**Returns the debug name associated with the given entity**. Useful for logging and debugging purposes.

### Component Type Registration

`register_component<Component>()`
```c++
template<class Component>
ComponentManager<Component>& register_component();
```
**Initializes storage for a component type** by creating a new `ComponentManager<Component>` and inserting it into `_components_arrays` under the corresponding `std::type_index`. If this is the first registration for the type, an eraser function is added to `_erasers` to ensure components of this type are removed when entities are destroyed.

Returns a reference to the newly created or existing `ComponentManager<Component>`.

`get_components<Component>()`
```c++
template <class Component>
ComponentManager<Component>& get_components();

template <class Component>
ComponentManager<Component> const& get_components() const;
```
Retrieves the `ComponentManager<Component>` for the specified type by extracting it from `_components_arrays` using `std::any_cast`. The const overload allows read-only access to component storage.

### Component Access Operations

`get_component<Component>(Entity const& e)`
```c++
template<class Component>
std::optional<Component>& get_component(Entity const& e);

template<class Component>
const std::optional<Component>& get_component(Entity const& e) const;
```
Returns a reference to the `std::optional<Component>` stored at the entity's ID index. The caller must check `has_value()` before dereferencing. The const overload provides read-only access.

`has_component<Component>(Entity const& e)`
```c++
template <class Component>
bool has_component(Entity const& e) const;
```
Checks whether the given entity is alive and possesses a component of the specified type. Returns `true` **if both conditions are met**.

### Component Modification Operations

`add_component<Component>(Entity const& e, Component&& c)`
```c++
template <class Component>
typename ComponentManager<Component>::reference_type add_component(Entity const& e, Component&& c);
```
Attaches an already-constructed component to the entity by moving it into storage. The entity must be alive. Returns a reference to the stored `std::optional<Component>`.

`emplace_component<Component>(Entity const& e, Params&&... ps)`
```c++
template<class Component, class... Params>
typename ComponentManager<Component>::reference_type emplace_component(Entity const& e, Params&&... ps);
```
Constructs a component directly in the storage at the entity's index using perfect forwarding. **This avoids unnecessary copies or moves**. The entity **must be alive**, and the component type **must have a constructor matching the provided arguments**. Returns a reference to the newly constructed component.

`remove_component<Component>(Entity const& e)`
```c++
template<class Component>
void remove_component(Entity const& e);
```
**Erases the component of the specified type from the entity** by calling `reset()` on the corresponding `std::optional`. The slot becomes empty but remains allocated in the `ComponentManager`.

## Example Usage

The following examples demonstrate typical ECS workflows using `EntityManager`.

#### Define Components

```c++
struct Position {
    float x;
    float y;

    // Constructor is needed to use emplace_component()
    Position(float xx, float yy) : x(xx), y(yy) {}
};

struct Velocity {
    float vx;
    float vy;

    // Constructor is needed to use emplace_component()
    Velocity(float vxx, float vyy) : vxx(vx), vy(vyy) {}
};
```
> [!NOTE]
> Components must have constructors if you intend to use `emplace_component()`. 
> Without a constructor, use the aggregate initialization syntax with `add_component()` instead.

#### Basic Entity and Component Operations

```c++
int main()
{
    // Initialize ECS
    EntityManager em;

    // Register component types
    em.register_component<Position>();
    em.register_component<Velocity>();

    // Create entities
    Entity e1 = em.spawn_entity("Player");
    Entity e2 = em.spawn_entity("Enemy");

    // Attach components to entities
    em.emplace_component<Position>(e1, 10.f, 20.f);
    em.emplace_component<Velocity>(e1, 1.f, 0.f);

    em.emplace_component<Position>(e2, -5.f, 3.f);

    // Access components
    auto& pos1 = em.get_component<Position>(e1);
    auto& vel1 = em.get_component<Velocity>(e1);

    if (pos1 && vel1) {
        pos1->x += vel1->vx;
        pos1->y += vel1->vy;
    }

    // Check component presence
    bool has_vel = em.has_component<Velocity>(e2);

    // Remove components
    em.remove_component<Position>(e2);

    // Destroy entities
    em.kill_entity(e2);

    return 0;
}
```
This example demonstrates the **complete workflow**: registering component types, creating entities, attaching components, accessing and modifying component data, and cleaning up entities.

#### Iterating Over Components

```c++
int main(void)
{
    // Setup
    EntityManager em;
    em.register_component<Position>();

    Entity e1 = em.spawn_entity("A");
    Entity e2 = em.spawn_entity("B");
    Entity e3 = em.spawn_entity("C");

    em.emplace_component<Position>(e1, 1.f, 2.f);
    em.emplace_component<Position>(e2, 3.f, 4.f);
    em.emplace_component<Position>(e3, 5.f, 6.f);

    // Iterate over all positions
    auto& positions = em.get_components<Position>();

    for (std::size_t id = 0; id < positions.size(); ++id) {
        // Skip empty slots
        if (!positions[id].has_value())
            continue;

        // Skip dead entities
        if (!em.is_alive(Entity{id, ""}))
            continue;

        // Process component
        auto& pos = positions[id].value();
        pos.x += 10.f;
        pos.y += 10.f;
    }

    return 0;
}
```
This pattern is typical for systems in an ECS architecture: retrieve the `ComponentManager` for relevant types, iterate over all indices, filter out empty slots and dead entities, then process the components. This approach provides cache-friendly iteration over dense component data.

## Integration Notes

`EntityManager` serves as **the central coordinator** between **entities and their components**. It maintains type-erased storage for all component types while providing type-safe access through template methods. The use of `std::any` allows dynamic registration of component types without requiring compile-time knowledge of all possible components, while `std::type_index` ensures type safety at runtime.
