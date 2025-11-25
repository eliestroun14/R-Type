# GameEngine
The `GameEngine` class serves as the **main facade** for the ECS architecture. It unifies **entity management** and **system coordination** into a **single interface**, providing a simplified API for game logic while maintaining clean separation between components and systems.

## Class Overview
`GameEngine` is a high-level **orchestrator** that owns both an `EntityManager` and a `SystemManager`. It delegates entity and component operations to the `EntityManager` and will handle system registration and execution through the `SystemManager`.

Here is the class:
```c++
class GameEngine {
    public:
        void Init();

        Entity CreateEntity(std::string entityName);
        void DestroyEntity(Entity& e);
        bool IsAlive(Entity const& entity) const;
        std::string GetEntityName(Entity const& entity) const;

        template<class Component>
        ComponentManager<Component>& RegisterComponent();

        template <class Component>
        typename ComponentManager<Component>::reference_type
        AddComponent(Entity const& entity, Component&& component);

        template<class Component, class... Params>
        typename ComponentManager<Component>::reference_type
        EmplaceComponent(Entity const& entity, Params&&... params);

        template<class Component>
        void RemoveComponent(Entity const& entity);

        template<typename Component>
        ComponentManager<Component>& GetComponents() const;

        template<typename Component>
        std::optional<Component>& GetComponentEntity(Entity const& entity) const;

    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<SystemManager> _systemManager;
};
```

## Initialization
`Init()`
```c++
void Init();
```
**Initializes** the game engine by creating fresh instances of `EntityManager` and `SystemManager`. **This method must be called before any other operations**. It allocates the internal managers using `std::make_unique` to ensure proper lifetime management.


## Entity Lifecyle Operations
`CreateEntity(std::string entityName)`
```c++
Entity CreateEntity(std::string entityName);
```
**Creates a new entity** with the specified debug name. This method delegates to `EntityManager::spawn_entity()`. Returns the newly created `Entity` object.


`DestroyEntity(Entity& e)`
```c++
void DestroyEntity(Entity& e);
```
**Destroys an entity** and **removes all of its components**. This method delegates to `EntityManager::kill_entity()`. The entity ID is recycled for future use.

> [!NOTE]
> System cleanup for destroyed entities is currently under development.


`IsAlive(Entity const& entity)`
```c++
bool IsAlive(Entity const& entity) const;
```
Checks whether the specified entity is currently active. Returns `true` if the **entity exists**, `false` **otherwise**.


`GetEntityName(Entity const& entity)`
```c++
std::string GetEntityName(Entity const& entity) const;
```
Retrieves the **debug name** of the specified entity. Useful for logging and debugging purposes.


## Component Type Registration
`RegisterComponent<Component>()`
```c++
template<class Component>
ComponentManager<Component>& RegisterComponent();
```
Registers a component type with the engine by creating a `ComponentManager<Component>` in the underlying `EntityManager`. **This must be called once per component type before that type can be used**. Returns a reference to the component manager for the registered type.


## Component Operations
`AddComponent<Component>(Entity const& entity, Component&& component)`
```c++
template <class Component>
typename ComponentManager<Component>::reference_type
AddComponent(Entity const& entity, Component&& component);
```
Attaches an **already-constructed component** to an entity by moving it into storage. The entity **must be alive**. Returns a reference to the stored `std::optional<Component>`.


`EmplaceComponent<Component>(Entity const& entity, Params&&... params)`
```c++
template<class Component, class... Params>
typename ComponentManager<Component>::reference_type
EmplaceComponent(Entity const& entity, Params&&... params);
```
Constructs a component directly in storage at the entity's index using perfect forwarding. This is more efficient than `AddComponent` as it avoids copies or moves. **The component type must have a constructor matching the provided arguments**. Returns a reference to the newly constructed component.


`RemoveComponent<Component>(Entity const& entity)`
```c++
template<class Component>
void RemoveComponent(Entity const& entity);
```
Removes the specified component type from an entity. The component slot becomes empty but **remains allocated** in the `ComponentManager`.

## Component Access
`GetComponents<Component>()`
```c++
template<typename Component>
ComponentManager<Component>& GetComponents() const;
```
Retrieves the `ComponentManager<Component>` for the specified type. This **allows direct iteration** over all components of a given type across all entities. **Useful for implementing systems that process multiple entities**.


`GetComponentEntity<Component>(Entity const& entity)`
```c++
template<typename Component>
std::optional<Component>& GetComponentEntity(Entity const& entity) const;
```
Returns a reference to the `std::optional<Component>` for the specified entity. The caller **must check `has_value()` before dereferencing** to ensure the component exists.


## Example Usage
The following example demonstrates a complete game setup using `GameEngine`.

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

### Example main()
```c++
int main()
{
    // Initialize engine
    GameEngine engine;
    engine.Init();

    // Register component types
    engine.RegisterComponent<Transform>();
    engine.RegisterComponent<Health>();
    engine.RegisterComponent<Velocity>();

    // Create entities
    Entity player = engine.CreateEntity("Player");
    Entity enemy = engine.CreateEntity("Enemy");

    // Attach components
    engine.EmplaceComponent<Transform>(player, 0.f, 0.f, 0.f);
    engine.EmplaceComponent<Health>(player, 100, 100);
    engine.EmplaceComponent<Velocity>(player, 5.f, 0.f);

    engine.EmplaceComponent<Transform>(enemy, 50.f, 30.f, 180.f);
    engine.EmplaceComponent<Health>(enemy, 50, 50);

    // Access and modify components
    auto& playerHealth = engine.GetComponentEntity<Health>(player);
    if (playerHealth.has_value()) {
        playerHealth->current -= 10;
    }

    // Check entity status
    if (engine.IsAlive(player)) {
        std::cout << engine.GetEntityName(player) << " is alive\n";
    }

    // Remove component
    engine.RemoveComponent<Velocity>(player);

    // Destroy entity
    engine.DestroyEntity(enemy);

    return 0;
}
```

### Iterating Over Components With GameEngine
```c++
int main()
{
    GameEngine engine;
    engine.Init();

    engine.RegisterComponent<Transform>();

    Entity e1 = engine.CreateEntity("ObjectA");
    Entity e2 = engine.CreateEntity("ObjectB");
    Entity e3 = engine.CreateEntity("ObjectC");

    engine.EmplaceComponent<Transform>(e1, 10.f, 20.f, 0.f);
    engine.EmplaceComponent<Transform>(e2, 30.f, 40.f, 90.f);
    engine.EmplaceComponent<Transform>(e3, 50.f, 60.f, 180.f);

    // Get all transforms
    auto& transforms = engine.GetComponents<Transform>();

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
- **Centralizes initialization**: All ECS components are initialized through **a single `Init()` call**.
- **Maintains type safety**: Template methods preserve compile-time type checking while allowing flexible component types.

The use of `std::unique_ptr` for internal managers ensures **proper ownership semantics** and **automatic cleanup when the engine is destroyed**.

<!--
## Future Extensions
The `GameEngine` class is designed to be extended with system management features:

- System registration: Methods to register and configure game systems (rendering, physics, AI, etc.).
- System execution: Update loops that run registered systems each frame.
- System dependencies: Mechanisms to define execution order and dependencies between systems.

These features will be implemented through the `SystemManager` interface as the architecture evolves.
-->
