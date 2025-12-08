# Coordinator

The `Coordinator` is the **central hub** of the ECS architecture (implementing the **Facade Pattern**). It acts as the single point of contact between the user's game logic and the internal engine subsystems (`EntityManager` and `SystemManager`).

Instead of managing memory or system logic manually, the developer communicates solely with the Coordinator to orchestrate the game.

## Architecture Overview

The Coordinator unifies the data (Entities/Components) and the logic (Systems).

```text
       User Code (Main Loop)
              │
        [ Coordinator ] <────── Facade Interface
              │
      ┌───────┴───────┐
      ▼               ▼
[EntityManager] [SystemManager]
      │               │
  [Entities]      [Systems]
  [Components]    (Logic)
```

## Core Responsibilities

1.  **Abstraction:** Hides the complexity of bitset manipulation and memory pooling.
2.  **Life-cycle Management:** Centralizes the creation and destruction of game objects.
3.  **Data Routing:** Ensures components are stored in the correct contiguous arrays for cache efficiency.
4.  **System Orchestration:** Manages the registration and execution order of game systems.

## API Reference

### 1. Initialization

| Method | Description |
| :--- | :--- |
| `void init()` | **Bootstraps the Engine**. Allocates memory for internal managers. **Must be called first.** |

### 2. Entity Management

Wrappers around the `EntityManager` to handle Game Object IDs.

| Method | Description |
| :--- | :--- |
| `Entity createEntity(string name)` | Spawns a new entity and returns its unique ID. The name is used for debugging. |
| `void destroyEntity(Entity &e)` | Marks an entity for destruction and recycles its ID. |
| `bool isAlive(Entity e)` | Returns `true` if the ID is valid and currently in use. |
| `std::string getEntityName(Entity e)` | Retrieves the debug name assigned during creation. |

### 3. Component Management

These methods use **C++ Templates** to maintain type safety while avoiding `void*` casting.

| Method | Usage & Performance Note |
| :--- | :--- |
| `registerComponent<T>()` | Prepares the memory arrays for type `T`. Must be called before using this component. |
| `addComponent(Entity, T&&)` | **Move Semantics:** Moves an existing object into storage. Use this if the object is already created. |
| `emplaceComponent(Entity, Args...)` | **Zero-Copy (Recommended):** Constructs the component *directly* inside the array using `Args`. Faster than `addComponent` for complex types. |
| `removeComponent<T>(Entity)` | Strips the specified component type from the entity. |
| `getComponentEntity<T>(Entity)` | Returns a `std::optional<T>` (or reference) to the data. Allows read/write access to entity state. |

### 4. System Management

| Method | Description |
| :--- | :--- |
| `registerSystem<S>(Args...)` | Instantiates a system of type `S`. Arguments are forwarded to the system's constructor. |
| `updateSystems(float dt)` | **The Heartbeat.** Iterates through all registered systems. `dt` (Delta Time) ensures frame-rate independence. |

## Integration Example: The Game Loop

This example demonstrates the standard workflow: **Init -> Register -> Create -> Loop**.

```cpp
#include "Coordinator.hpp"

// 1. Define Components (Data)
struct Transform { float x, y; };
struct RigidBody { float velocity, mass; };

// 2. Define System (Logic)
class GravitySystem : public ISystem {
public:
    void update(float dt) override {
        // In a real engine, we would iterate over entities with RigidBody here
        std::cout << "Applying gravity..." << std::endl;
    }
};

int main() {
    Coordinator gCoordinator;

    // A. Bootstrap
    gCoordinator.init();

    // B. Register Data Types
    gCoordinator.registerComponent<Transform>();
    gCoordinator.registerComponent<RigidBody>();

    // C. Register Logic
    gCoordinator.registerSystem<GravitySystem>();

    // D. Create Game Objects (Entities)
    Entity player = gCoordinator.createEntity("Player1");

    // E. Compose the Entity
    // "Emplace" is preferred over "Add" for efficiency
    gCoordinator.emplaceComponent<Transform>(player, 10.0f, 50.0f);
    gCoordinator.emplaceComponent<RigidBody>(player, 0.0f, 70.0f);

    // F. Main Loop
    float dt = 0.016f;
    while (true) {
        gCoordinator.updateSystems(dt);
        // ... Rendering code ...
    }
    return 0;
}
```