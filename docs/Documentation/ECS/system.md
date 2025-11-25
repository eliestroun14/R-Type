# System

In our ECS, we have two classes reserved to systems. We have the System class and the SystemManager class.

## System class

A System is a base class that processes entities with specific component combinations. **It maintains a list of entities** that match its requirements and provides lifecycle hooks for initialization, updates, and cleanup.

Here is the class:

```cpp
class System {
    public:
        using entity_type = Entity;

        System() = default;
        virtual ~System() = default;

        virtual void onCreate() {}
        virtual void onStartRunning() {}
        virtual void onUpdate(float dt) {}
        virtual void onStopRunning() {}
        virtual void onDestroy() {}

        void addEntity(Entity e);
        void removeEntity(Entity const& e);

        bool empty() const noexcept { return _entities.empty(); }
        bool isRunning() const noexcept { return _running; }

    protected:
        std::list<Entity> _entities;
        bool _running { false };
};
```

### Design Principles

The System class follows an event-driven lifecycle pattern:

- **Virtual lifecycle hooks**: Systems can override methods to respond to specific events.
- **Entity tracking**: Each system maintains its own list of relevant entities.
- **Running state**: Systems track whether they have entities and automatically manage their running state.

This design ensures that systems only execute when they have entities to process and provides clear extension points for custom behavior.

### Lifecycle Hooks

Systems provide five virtual methods that can be overridden to implement custom behavior at different stages:

#### onCreate()

```cpp
virtual void onCreate() {}
```

**Called during global system initialization**, before any entities are added. Override this method to perform one-time setup such as resource loading, initializing state, or preparing data structures.

#### onStartRunning()

```cpp
virtual void onStartRunning() {}
```

**Called when the system receives its first entity**. This happens automatically when `addEntity()` transitions the system from empty to non-empty. Use this to activate resources or begin tracking that only makes sense when entities are present.

#### onUpdate(float dt)

```cpp
virtual void onUpdate(float dt) {}
```

**Called every frame while the system has entities**. The `dt` parameter represents the delta time since the last frame. This is where the primary logic of your system should be implemented—iterating over entities and updating their components.

#### onStopRunning()

```cpp
virtual void onStopRunning() {}
```

**Called when the system loses its last entity**. This happens automatically when `removeEntity()` empties the entity list. Use this to deactivate resources or pause tracking that was started in `onStartRunning()`.

#### onDestroy()

```cpp
virtual void onDestroy() {}
```

**Called during global system cleanup**, after all entities have been processed. Override this method to release resources, save state, or perform final cleanup operations.

### Entity Management Operations

#### addEntity(Entity e)

```cpp
void addEntity(Entity e);
```

**Adds an entity to the system's tracked list**. If the entity is already present (checked by ID), it is not added again. If this is the first entity added to an empty system, the system transitions to running state and `onStartRunning()` is automatically called.

#### removeEntity(Entity const& e)

```cpp
void removeEntity(Entity const& e);
```

**Removes an entity from the system's tracked list** by ID. If removing this entity empties the list and the system was running, the system transitions to stopped state and `onStopRunning()` is automatically called.

### State Query Operations

#### empty()

```cpp
bool empty() const noexcept { return _entities.empty(); }
```

Returns `true` **if the system has no entities**, `false` otherwise.

#### isRunning()

```cpp
bool isRunning() const noexcept { return _running; }
```

Returns `true` **if the system is currently in running state** (has entities and has called `onStartRunning()`), `false` otherwise.

## SystemManager Class

SystemManager **owns and coordinates all systems in the ECS**. It manages system registration, retrieval, and orchestrates lifecycle events and updates across all systems.

Here is the class:

```cpp
class SystemManager {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

        template <class S, class... Args>
        S& addSystem(Args&&... args) {}

        template <class S>
        void deleteSystem() {}

        template <class S>
        S& getSystem() {}

        template <class S>
        const S& getSystem() const {}

        template <class S>
        bool hasSystem() const {}

        void onCreateAll() {}
        void onDestroyAll() {}
        void updateAll(float dt) {}

    private:
        std::unordered_map<std::type_index, std::unique_ptr<System>> _systems;
};
```

### Core Responsibilities

SystemManager performs four primary tasks:

1. **System lifecycle management**: Create, store, and destroy system instances.
2. **System registration**: Maintain one unique instance per system type using type-safe storage.
3. **System access**: Provide type-safe retrieval of registered systems.
4. **Coordination**: Orchestrate lifecycle events and updates across all systems.

### Internal Storage

The class maintains a single data structure:

- **\_systems**: Maps `std::type_index` to `std::unique_ptr<System>` instances, ensuring one system per type.

**Each system type** is stored under its `std::type_index` key, which provides type-safe lookup at runtime while allowing heterogeneous storage in a single container.

### System Registration Operations

#### addSystem(Args&&... args)

```cpp
template <class S, class... Args>
S& addSystem(Args&&... args);
```

**Creates and registers a new system of type S** with the provided constructor arguments. The template parameter `S` must derive from `System`. If a system of this type already exists, the existing instance is returned instead of creating a new one. Returns a reference to the system instance.

#### deleteSystem()

```cpp
template <class S>
void deleteSystem();
```

**Removes and destroys the system of type S** from the manager. If no such system exists, this operation has no effect.

#### hasSystem()

```cpp
template <class S>
bool hasSystem() const;
```

Returns `true` **if a system of type S is registered**, `false` otherwise.

### System Access Operations

#### getSystem()

```cpp
template <class S>
S& getSystem();

template <class S>
const S& getSystem() const;
```

Retrieves the system instance of type S. Throws `std::runtime_error` if the system is not registered. The const overload provides read-only access.

### Coordination Operations

#### onCreateAll()

```cpp
void onCreateAll();
```

**Calls onCreate() on every registered system** in an unspecified order. This should be called once during ECS initialization, before the main loop begins.

#### onDestroyAll()

```cpp
void onDestroyAll();
```

**Calls onDestroy() on every registered system** in an unspecified order. This should be called once during ECS shutdown, after the main loop ends.

#### updateAll(float dt)

```cpp
void updateAll(float dt);
```

**Calls onUpdate(dt) on every non-empty system**. Systems with no entities are skipped automatically. This should be called every frame in the main loop.

## Example Usage

The following examples demonstrate typical ECS workflows using SystemManager and custom systems.

### Define a Custom System

```cpp
class MovementSystem : public System {
    public:
        void onCreate() override {
            std::cout << "MovementSystem initialized\n";
        }

        void onStartRunning() override {
            std::cout << "MovementSystem started\n";
        }

        void onUpdate(float dt) override {
            for (auto const& entity : _entities) {
                // Access EntityManager to get components
                // Process position and velocity components
            }
        }

        void onStopRunning() override {
            std::cout << "MovementSystem stopped\n";
        }

        void onDestroy() override {
            std::cout << "MovementSystem destroyed\n";
        }
};
```

> Custom systems typically need access to an EntityManager to retrieve and modify components.
> Consider storing a reference to EntityManager in your system or passing it as a parameter.

### Basic System Registration and Update Loop

```cpp
int main()
{
    // Initialize ECS
    EntityManager em;
    SystemManager sm;

    // Register systems
    sm.addSystem<MovementSystem>();
    sm.addSystem<RenderSystem>();

    // Initialize all systems
    sm.onCreateAll();

    // Create entities and add them to systems
    Entity player = em.spawn_entity("Player");
    sm.getSystem<MovementSystem>().addEntity(player);

    // Main game loop
    float dt = 0.016f; // ~60 FPS
    while (running) {
        // Update all systems
        sm.updateAll(dt);
    }

    // Cleanup
    sm.onDestroyAll();

    return 0;
}
```

This example demonstrates the **complete workflow**: registering systems, initializing them, adding entities, running the update loop, and cleaning up.

### System with EntityManager Integration

```cpp
class PhysicsSystem : public System {
    public:
        PhysicsSystem(EntityManager& em) : _em(em) {}

        void onUpdate(float dt) override {
            auto& positions = _em.get_components<Position>();
            auto& velocities = _em.get_components<Velocity>();

            for (auto const& entity : _entities) {
                std::size_t id = static_cast<std::size_t>(entity);

                if (!positions[id].has_value() || !velocities[id].has_value())
                    continue;

                auto& pos = positions[id].value();
                auto& vel = velocities[id].value();

                pos.x += vel.vx * dt;
                pos.y += vel.vy * dt;
            }
        }

    private:
        EntityManager& _em;
};

int main()
{
    EntityManager em;
    SystemManager sm;

    // Register components
    em.register_component<Position>();
    em.register_component<Velocity>();

    // Add system with EntityManager reference
    sm.addSystem<PhysicsSystem>(em);

    // Create entity with components
    Entity e = em.spawn_entity("MovingObject");
    em.emplace_component<Position>(e, 0.f, 0.f);
    em.emplace_component<Velocity>(e, 10.f, 5.f);

    // Add entity to system
    sm.getSystem<PhysicsSystem>().addEntity(e);

    // Initialize and run
    sm.onCreateAll();
    sm.updateAll(0.016f);

    return 0;
}
```

This pattern shows how to integrate EntityManager with systems by passing it as a constructor parameter, allowing systems to access and modify component data.

## Integration Notes

SystemManager serves as **the central coordinator** for all game logic systems. It provides lifecycle management and update orchestration while allowing systems to focus on their specific responsibilities. The use of `std::type_index` and `std::unique_ptr` ensures type-safe, efficient storage of heterogeneous system types with minimal runtime overhead.

Systems are **not automatically aware of which entities match their requirements**—entity assignment must be handled by higher-level logic (often through an archetype system or manual filtering). The System base class provides the infrastructure for tracking assigned entities and managing running state, but component filtering logic is left to the application layer.
