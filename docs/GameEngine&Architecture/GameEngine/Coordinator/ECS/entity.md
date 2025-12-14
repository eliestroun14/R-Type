# Entity

In our ECS, we have two classes reserved to entities. We have the `Entity` class and the `EntityManager` class.

## `Entity` class

An `Entity` is simply an ID. **It does not contain anything.**

Here the class:

```c++
class Entity {
    public:
        // Implicit conversion to size_t allows Entity to be used as an array index
        operator std::size_t() const {
            return this->_id;
        }

    private:
        std::size_t _id;

        // private constructor: only the EntityManager is able to create new entity
        explicit Entity(std::size_t id) : _id(id) {}

        // friend of EntityManager
        friend class EntityManager;
};
```

### Design Principles
The `Entity` class follows a strict encapsulation pattern:

- **Private constructor**: Only `EntityManager` can instantiate entities.
- **Friend declaration**: `EntityManager` has privileged access to create entities.
- **Implicit conversion**: The `operator std::size_t()` allows entities to be used directly as indices in `std::vector` or `std::array`.

This design ensures that all entity creation and lifecycle management flows through **a single point of control**.

```c++
explicit Entity(std::size_t id) : _id(id) {}

friend class EntityManager;
```

## `EntityManager` Class

`EntityManager` **owns the complete lifecycle of every entity in the system**. It allocates IDs, manages "Signatures" (bitsets representing active components), coordinates component storage, and provides the interface for entity/component operations.

Here is the class:

```c++
#define MAX_COMPONENTS 15

class EntityManager {
    using Signature = std::bitset<MAX_COMPONENTS>;

    public :
        EntityManager() = default;

        // Component Registration
        template<class Component>
        ComponentManager<Component>& registerComponent();

        template <class Component>
        ComponentManager<Component> &getComponents();

        template <class Component>
        ComponentManager<Component> const &getComponents() const;

        // Component Access
        template<class Component>
        std::optional<Component>& getComponent(Entity const& e);

        template<class Component>
        const std::optional<Component>& getComponent(Entity const& e) const;

        // Signature Management
        void setSignature(Entity const &e, Signature signature);
        Signature getSignature(Entity const &e);

        // Entity Lifecycle
        Entity spawnEntity(std::string name);
        void killEntity(Entity const &entity);
        bool isAlive(Entity const& e) const;
        std::string getEntityName(Entity const& e) const;

        // Component Logic
        template <class Component>
        bool hasComponent(Entity const& e) const;

        template <class Component>
        typename ComponentManager<Component>::referenceType addComponent(Entity const &e, Component &&c);

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType emplaceComponent(Entity const& e, Params&&... ps);

        template<class Component>
        void removeComponent(Entity const& e);


    private:
        std::unordered_map<std::type_index, std::any> _componentsArrays;
        std::unordered_map<std::type_index, std::function<void(EntityManager&,Entity const&)>> _erasers;

        size_t _nextId = 0;
        std::vector<std::size_t> _freeIds;
        std::unordered_set<std::size_t> _aliveEntities;

        std::vector<Signature> _signatures;
        std::vector<std::string> _entitiesName;
};
```

### Core responsabilities

`EntityManager` performs four primary tasks:

1. **Entity lifecycle management**: Create/destroy entities and recycle IDs.
2. **Signature Management**: Track which components an entity possesses using bitsets (`std::bitset`).
3. **Component type registration**: Assign unique IDs to component types and maintain storage.
4. **Component operations**: Attach/detach components and automatically update Entity Signatures.
5. **Entity tracking**: Monitor alive entities and their debug names.

### Internal Storage
The class maintains **several data structures**:

- `_componentsArrays`: Maps `std::type_index` to `ComponentManager<T>`.
- `_erasers`: Cleanup functions for component removal.
- `_signatures`: A `std::vector` of **Bitsets**. Index `i` corresponds to Entity `i`. It allows Systems to quicly filter entities based on which components they possess.
- `_aliveEntities`: Set of currently active entity IDs.
- `_entitiesName`: Vector storing the name string of each entity (for debugging).
- `_nextId` & `_freeIds`: ID generation and recycling.

**Each component type** is stored in a dedicated `ComponentManager<T>`. When registering a component type, an eraser function is also installed to ensure proper cleanup when entities are destroyed.

### Entity Lifecycle Operations

`spawnEntity(std::string name)`
```c++
Entity spawnEntity(std::string name);
```
**Creates a new entity**.
1. Allocates an ID (new or recycled).
2. **Resets the entity's Signature** (sets all bits to 0) to ensure it starts fresh.
3. Marks the ID as alive.
4. Stores the entity's name.


`killEntity(Entity const& entity)`
```c++
void killEntity(Entity const& entity);
```
**Destroys an entity**.
1. Removes ID from `_aliveEntities`.
2. Adds ID to `_freeIds`.
3. **Resets the Signature** immediately. This ensures Systems stop processing this entity even before the data is overwritten.
4. Invokes all eraser functions to clear component data.

`isAlive(Entity const& e)`
```c++
bool isAlive(Entity const& e) const;
```
Checks whether the given entity ID is present in the set of alive entities. Returns `true` **if the entity is active**, `false` otherwise.

`getEntityName(Entity const& e)`
```c++
std::string getEntityName(Entity const& e) const;
```
**Returns the debug name associated with the given entity**. Useful for logging and debugging purposes.

`setSignature(Entity const& e, Signature s) / getSignature(Entity const& e)`
**Manages the entity's bitset**. A Signature represents the "DNA" of the entity

### Component Modification Operations (Signature Updates)

The most significant update in this version is that adding or removing components **automatically updates the Entity's Signature**.

`addComponent<Component>(Entity const& e, Component&& c)`
```c++
template <class Component>
typename ComponentManager<Component>::referenceType addComponent(Entity const &e, Component &&c);
```
1. Checks if `e` is alive.
2. Calculates the unique ID for `Component` type.
3. **Sets the corresponding bit to true** in `_signatures[e]`.
4. Moves the component into storage.

`emplaceComponent<Component>(Entity const& e, Params&&... ps)`
```c++
template<class Component, class... Params>
typename ComponentManager<Component>::referenceType emplaceComponent(Entity const& e, Params&&... ps);
```
Constructs a component in-place.
1. Checks if `e` is alive.
2. Calculates the unique ID for `Component` type.
3. **Sets the corresponding bit to true** in `_signatures[e]`.
4. Constructs the component using perfect forwarding.

`removeComponent<Component>(Entity const& e)`
```c++
template<class Component>
void removeComponent(Entity const& e);
```
1. Checks if `e` is alive.
2. Calculates the unique ID for `Component` type.
3. **Sets the corresponding bit to false** in `_signatures[e]`.
4. Erases the data from the component manager.

### Component Type Registration

`registerComponent<Component>()`
```c++
template<class Component>
ComponentManager<Component>& registerComponent();
```
Initializes storage for a component type. Crucially, it assigns a **Static Type ID** (internal counter) to `Component`. This ID corresponds to the bit index used in `Signature`.

**Note:** `MAX_COMPONENTS` is defined as **15**. Registering more than 15 component types will exceed the bitset capacity.

## Example Usage

#### Signature & Lifecycle Workflow
```c++
int main()
{
    EntityManager em;
    em.registerComponent<Position>(); // Type ID 0
    em.registerComponent<Velocity>(); // Type ID 1

    Entity player = em.spawnEntity("Player");
    
    // Signature starts at 00...00

    // Add Position -> Bit 0 turns ON
    em.emplaceComponent<Position>(player, 10, 20); 
    // Signature is now 00...01

    // Add Velocity -> Bit 1 turns ON
    em.emplaceComponent<Velocity>(player, 1, 1);
    // Signature is now 00...11

    // Systems can now match this signature!

    // Remove Position -> Bit 0 turns OFF
    em.removeComponent<Position>(player);
    // Signature is now 00...10

    em.killEntity(player);
    // Signature is reset to 00...00 immediately
}
```
