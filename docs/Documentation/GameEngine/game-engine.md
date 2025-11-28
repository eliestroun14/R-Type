# GameEngine
The `GameEngine` class serves as the **main facade** for the ECS architecture. It unifies **entity management** and **system coordination** into a **single interface**, providing a simplified API for game logic while maintaining clean separation between components and systems.

## Class Overview
`GameEngine` is a high-level **orchestrator** that owns both an `EntityManager` and a `SystemManager`. It delegates entity and component operations to the `EntityManager` and handles system registration and execution through the `SystemManager`.

Here is the class:
```c++
class GameEngine {
    public:
        void init();

        Entity createEntity(std::string entityName);
        void destroyEntity(Entity& e);
        bool isAlive(Entity const& entity) const;
        std::string getEntityName(Entity const& entity) const;

        template<class Component>
        ComponentManager<Component>& registerComponent();

        template <class Component>
        typename ComponentManager<Component>::referenceType
        addComponent(Entity const& entity, Component&& component);

        template<class Component, class... Params>
        typename ComponentManager<Component>::referenceType
        emplaceComponent(Entity const& entity, Params&&... params);

        template<class Component>
        void removeComponent(Entity const& entity);

        template<typename Component>
        ComponentManager<Component>& getComponents() const;

        template<typename Component>
        std::optional<Component>& getComponentEntity(Entity const& entity) const;

        template<class System, class... Params>
        System& registerSystem(Params&&... params);

        template<class System>
        System& getSystem() const;

        template<class System>
        void removeSystem();

        void updateSystems(float dt);

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
};
```

## Initialization
`init()`
```c++
void init();
```
**Initializes** the game engine by creating fresh instances of `EntityManager` and `SystemManager`. **This method must be called before any other operations**. It allocates the internal managers using `std::make_unique` to ensure proper lifetime management.


## Entity Lifecyle Operations
`createEntity(std::string entityName)`
```c++
Entity createEntity(std::string entityName);
```
**Creates a new entity** with the specified debug name. This method delegates to `EntityManager::spawnEntity()`. Returns the newly created `Entity` object.


`destroyEntity(Entity& e)`
```c++
void destroyEntity(Entity& e);
```
**Destroys an entity** and **removes all of its components**. This method delegates to `EntityManager::killEntity()`. The entity ID is recycled for future use.

> [!NOTE]
> System cleanup for destroyed entities is currently under development.


`isAlive(Entity const& entity)`
```c++
bool isAlive(Entity const& entity) const;
```
Checks whether the specified entity is currently active. Returns `true` if the **entity exists**, `false` **otherwise**.


`getEntityName(Entity const& entity)`
```c++
std::string getEntityName(Entity const& entity) const;
```
Retrieves the **debug name** of the specified entity. Useful for logging and debugging purposes.


## Component Type Registration
`registerComponent<Component>()`
```c++
template<class Component>
ComponentManager<Component>& registerComponent();
```
Registers a component type with the engine by creating a `ComponentManager<Component>` in the underlying `EntityManager`. **This must be called once per component type before that type can be used**. Returns a reference to the component manager for the registered type.


## Component Operations
`addComponent<Component>(Entity const& entity, Component&& component)`
```c++
template <class Component>
typename ComponentManager<Component>::referenceType
addComponent(Entity const& entity, Component&& component);
```
Attaches an **already-constructed component** to an entity by moving it into storage. The entity **must be alive**. Returns a reference to the stored `std::optional<Component>`.


`emplaceComponent<Component>(Entity const& entity, Params&&... params)`
```c++
template<class Component, class... Params>
typename ComponentManager<Component>::referenceType
emplaceComponent(Entity const& entity, Params&&... params);
```
Constructs a component directly in storage at the entity's index using perfect forwarding. This is more efficient than `addComponent` as it avoids copies or moves. **The component type must have a constructor matching the provided arguments**. Returns a reference to the newly constructed component.


`removeComponent<Component>(Entity const& entity)`
```c++
template<class Component>
void removeComponent(Entity const& entity);
```
Removes the specified component type from an entity. The component slot becomes empty but **remains allocated** in the `ComponentManager`.

## Component Access
`getComponents<Component>()`
```c++
template<typename Component>
ComponentManager<Component>& getComponents() const;
```
Retrieves the `ComponentManager<Component>` for the specified type. This **allows direct iteration** over all components of a given type across all entities. **Useful for implementing systems that process multiple entities**.


`getComponentEntity<Component>(Entity const& entity)`
```c++
template<typename Component>
std::optional<Component>& getComponentEntity(Entity const& entity) const;
```
Returns a reference to the `std::optional<Component>` for the specified entity. The caller **must check `has_value()` before dereferencing** to ensure the component exists.


## System Management

The `GameEngine` provides a complete interface for managing game systems, which are the logic units that process entities and their components each frame.

### System Registration

`registerSystem<System>(Params&&... params)`
```c++
template<class System, class... Params>
System& registerSystem(Params&&... params);
```
**Registers a new system** with the engine by forwarding the provided constructor arguments to the system's constructor. This method delegates to `SystemManager::addSystem()`. **Each system type can only be registered once**. Returns a reference to the newly created system instance.

**Parameters:**
- `System`: The system type to register
- `params`: Constructor arguments to forward to the system

**Example:**
```c++
// Register a system with no constructor arguments
engine.registerSystem<RenderSystem>();

// Register a system with constructor arguments
engine.registerSystem<PhysicsSystem>(9.81f, 60);
```


### System Access

`getSystem<System>()`
```c++
template<class System>
System& getSystem() const;
```
Retrieves a reference to a previously registered system. This allows direct access to system-specific methods and state. **The system must have been registered before calling this method**.

**Parameters:**
- `System`: The system type to retrieve

**Example:**
```c++
auto& physics = engine.getSystem<PhysicsSystem>();
physics.SetGravity(12.0f);
```


### System Removal

`removeSystem<System>()`
```c++
template<class System>
void removeSystem();
```
**Removes a system** from the engine. This method delegates to `SystemManager::deleteSystem()`. The system is destroyed and can no longer be accessed or updated.

**Parameters:**
- `System`: The system type to remove

**Example:**
```c++
engine.removeSystem<DebugRenderSystem>();
```


### System Execution

`updateSystems(float dt)`
```c++
void updateSystems(float dt);
```
**Executes all registered systems** in the order they were registered. This method should be called once per frame in the game loop. Each system's `update()` method is invoked with the provided delta time.

**Parameters:**
- `dt`: Delta time in seconds since the last frame

**Example:**
```c++
// In game loop
while (running) {
    float deltaTime = calculateDeltaTime();
    engine.updateSystems(deltaTime);
}
```


## Example Usage
The following examples demonstrate complete game setups using `GameEngine`.

### Define Components
```c++
struct Transform {
    float x, y;
    float rotation;

    Transform(float xx, float yy, float rot)
        : x(xx), y(yy), rotation(rot) {}
};

struct Health {
    int current;
    int max;

    Health(int hp, int maxHp)
        : current(hp), max(maxHp) {}
};

struct Velocity {
    float vx, vy;

    Velocity(float x, float y)
        : vx(x), vy(y) {}
};
```

### Define Systems
```c++
class MovementSystem {
public:
    MovementSystem(GameEngine& engine) : _engine(engine) {}

    void update(float dt) {
        auto& transforms = _engine.getComponents<Transform>();
        auto& velocities = _engine.getComponents<Velocity>();

        for (std::size_t id = 0; id < transforms.size(); ++id) {
            if (!transforms[id].has_value() || !velocities[id].has_value())
                continue;

            auto& transform = transforms[id].value();
            auto& velocity = velocities[id].value();

            transform.x += velocity.vx * dt;
            transform.y += velocity.vy * dt;
        }
    }

private:
    GameEngine& _engine;
};

class HealthSystem {
public:
    HealthSystem(GameEngine& engine) : _engine(engine) {}

    void update(float dt) {
        auto& healths = _engine.getComponents<Health>();

        for (std::size_t id = 0; id < healths.size(); ++id) {
            if (!healths[id].has_value())
                continue;

            auto& health = healths[id].value();
            
            // Kill entities with zero health
            if (health.current <= 0) {
                Entity entity(id);
                if (_engine.isAlive(entity)) {
                    _engine.destroyEntity(entity);
                }
            }
        }
    }

private:
    GameEngine& _engine;
};
```

### Complete Game Setup with Systems
```c++
int main()
{
    // Initialize engine
    GameEngine engine;
    engine.init();

    // Register component types
    engine.registerComponent<Transform>();
    engine.registerComponent<Health>();
    engine.registerComponent<Velocity>();

    // Register systems
    engine.registerSystem<MovementSystem>(engine);
    engine.registerSystem<HealthSystem>(engine);

    // Create entities
    Entity player = engine.createEntity("Player");
    Entity enemy = engine.createEntity("Enemy");

    // Attach components
    engine.emplaceComponent<Transform>(player, 0.f, 0.f, 0.f);
    engine.emplaceComponent<Health>(player, 100, 100);
    engine.emplaceComponent<Velocity>(player, 5.f, 0.f);

    engine.emplaceComponent<Transform>(enemy, 50.f, 30.f, 180.f);
    engine.emplaceComponent<Health>(enemy, 50, 50);

    // Game loop
    bool running = true;
    float deltaTime = 0.016f; // ~60 FPS

    while (running) {
        // Update all systems
        engine.updateSystems(deltaTime);

        // Access and modify components
        auto& playerHealth = engine.getComponentEntity<Health>(player);
        if (playerHealth.has_value()) {
            playerHealth->current -= 1; // Take damage each frame
        }

        // Check entity status
        if (!engine.isAlive(player)) {
            std::cout << "Player died!\n";
            running = false;
        }
    }

    return 0;
}
```

### Iterating Over Components With GameEngine
```c++
int main()
{
    GameEngine engine;
    engine.init();

    engine.registerComponent<Transform>();

    Entity e1 = engine.createEntity("ObjectA");
    Entity e2 = engine.createEntity("ObjectB");
    Entity e3 = engine.createEntity("ObjectC");

    engine.emplaceComponent<Transform>(e1, 10.f, 20.f, 0.f);
    engine.emplaceComponent<Transform>(e2, 30.f, 40.f, 90.f);
    engine.emplaceComponent<Transform>(e3, 50.f, 60.f, 180.f);

    // Get all transforms
    auto& transforms = engine.getComponents<Transform>();

    // Process all entities with transforms
    for (std::size_t id = 0; id < transforms.size(); ++id) {
        if (!transforms[id].has_value())
            continue;

        // Simply check using the ID directly
        // Since transforms[id] has a value, the entity with this ID exists
        auto& transform = transforms[id].value();
        transform.rotation += 1.f;
    }

    return 0;
}
```


## Design Rationale
The `GameEngine` class acts as a facade pattern, hiding the complexity of the underlying ECS architecture. By providing a unified interface, it:

- **Simplifies API usage**: Game developers interact with a **single class** instead of managing multiple managers.
- **Encapsulates implementation details**: Internal changes to `EntityManager` or `SystemManager` **don't affect** client code.
- **Centralizes initialization**: All ECS components are initialized through **a single `init()` call**.
- **Maintains type safety**: Template methods preserve compile-time type checking while allowing flexible component types.
- **Coordinates system execution**: The engine manages system lifecycle and execution order through a simple update loop.

The use of `std::unique_ptr` for internal managers ensures **proper ownership semantics** and **automatic cleanup when the engine is destroyed**.


## System Design Pattern

Systems in this ECS architecture follow these conventions:

1. **Constructor Injection**: Systems typically receive a reference to the `GameEngine` in their constructor to access components.
2. **Update Method**: Each system implements an `update(float dt)` method that processes relevant entities each frame.
3. **Component Queries**: Systems retrieve component managers and iterate over entities that have the required components.
4. **Stateless or Stateful**: Systems can maintain internal state but should avoid storing entity-specific data (use components instead).


## Best Practices

- **Register all component types** before creating entities that use them.
- **Register all systems** before entering the game loop.
- **Call `updateSystems()`** once per frame with accurate delta time.
- **Systems should be independent**: Avoid tight coupling between systems; use components for communication.
- **Use system order wisely**: Systems execute in registration order, so register dependent systems accordingly (e.g., physics before rendering).
- **Handle destroyed entities**: Systems should check `isAlive()` or validate component existence before accessing entity data.