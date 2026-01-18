# Coordinator

The `Coordinator` is the **central hub** of the ECS architecture (implementing the **Facade Pattern**). It acts as the single point of contact between the high-level Game Engine and the internal subsystems (`EntityManager`, `SystemManager`, `RenderManager`).

Instead of managing memory, system logic, rendering, or packet parsing manually, the developer communicates solely with the Coordinator to orchestrate the game state.

## Architecture Overview

The Coordinator unifies Data (Entities), Logic (Systems), IO (Input/Render), and Networking events.

```text
       GameLoop (Client/Server)
              │
        [ Coordinator ] <────────────────── Facade Interface
              │
      ┌───────┼───────────────┬────────────────┐
      ▼       ▼               ▼                ▼
[EntityManager] [SystemManager] [RenderManager]  [Packet Logic]
      │       │               │                │
  [Entities]  [Systems]       [Window/Input]   [Protocol/Parsing]
  [Components]
```

## Core Responsibilities

1.  **ECS Orchestration:** Manages the lifecycle of Entities, Components, and Systems.
2.  **Input Abstraction:** Bridges SFML inputs (via RenderManager) to ECS Components (`InputComponent`).
3.  **Render Proxying:** Exposes rendering primitives (Window, Textures) to Systems without exposing the raw manager.
4.  **Network Reconciliation:** Decodes incoming network packets (Spawn, Move, GameState) and applies them to the ECS world.

## API Reference

### 1. Initialization

| Method | Description |
| :--- | :--- |
| `void init()` | **Core Init**. Allocates `EntityManager` and `SystemManager`. Essential for both Server and Client |
| `void initRender()` | **Graphics Init.** Allocates `RenderManager` and opens the window. **Client Only.** |

### 2. Entity Management

| Method | Description |
| :--- | :--- |
| `Entity createEntity(string)` | Spawns a raw entity with a debug name. |
| `Entity createPlayerEntity(id, playable)` | Helper to spawn a Player. Adds `InputComponent`. If `playable` is true, adds `Playable` tag (Local Player). |
| `void destroyEntity(Entity)` | Marks an entity for destruction. |
| `bool isAlive(Entity)` | Checks if an ID is valid. |
| `string getEntityName(Entity)` | Retrieves the debug name assigned during creation. |

### 3. Component Management

Wrappers around `EntityManager` using C++ Templates.

| Method | Description |
| :--- | :--- |
| `registerComponent<T>()` | Registers a component type `T` (Arrays allocation). |
| `addComponent(Entity, T)` | Moves a component into the array for the given entity. |
| `getComponentEntity<T>(Entity)` | Returns `std::optional<T>` to read/write component data. |
| `removeComponent<T>(Entity)` | Removes the component from the entity. |

### 4. System Management

| Method | Description |
| :--- | :--- |
| `registerSystem<S>()` | Instantiates a system. |
| `setSystemSignature<S, Comps...>()` | Defines which Components an entity must possess to be processed by System `S`. |
| `updateSystems(float dt)` | **Update Loop.** Runs logic for all systems. |
| `onCreateSystems()` / `onDestroySystems()` | Triggers `onCreate` / `onDestroy` callbacks in all systems. |

### 5. Input Management

Handles the flow of inputs from hardware (Client) or Network (Server) to the Entity logic.

| Method | Usage |
| :--- | :--- |
| `setLocalPlayerEntity(Entity, id)` | Links the RenderManager input system to a specific ECS Entity (The local player). |
| `processInput()` | **Client Only.** Polls window events (Keyboard/Mouse) and updates the local player's `InputComponent`. |
| `setPlayerInputAction(...)` | **Server/Network.** Manually sets an action state (used when applying an Input Packet from a client). |
| `isPlayerActionActive(...)` | Checks if a specific action is active for a specific player entity. |
| `isActionActive(GameAction)` | Checks if a key is currently held down locally (e.g., for UI logic). |
| `isActionJustPressed(GameAction)` | Checks if a key was pressed this exact frame (Edge detection). |
| `getMousePosition()` | Returns the mouse coordinates relative to the window. |

### 6. Rendering & Resources (Client Only)

Proxies for the `RenderManager`.

| Method | Description |
| :--- | :--- |
| `beginFrame()` | Clears the window. Call at start of render loop. |
| `render()` | Swaps buffers (display). Call at end of render loop. |
| `getTexture(Assets id)` | Retrieves a shared pointer to a loaded texture resource. |
| `getWindow()` | Returns the raw `sf::RenderWindow&`. |
| `getScaleFactor()` | Returns the window scaling factor for velocity adjustments. |

### 7. Network Packet Processing

Methods to apply network state to the local world.

| Method | Description |
| :--- | :--- |
| `processServerPackets(...)` | **Server Side.** Parses packets (Inputs) and updates entities input state via `PacketManager`. |
| `processClientPackets(...)` | **Client Side.** Handles Game State updates (`ENTITY_SPAWN`, `TRANSFORM_SNAPSHOT`, `GAME_END`, etc.) and reconciles the world state. |
| `handlePacketCreateEntity(...)` | **Internal.** Logic to spawn entities (Player, Enemy, Projectile) based on server data. |
| `handlePacketTransformSnapshot(...)` | **Internal.** Interpolates position/rotation from server snapshots. |

## Integration Example: Client Game Loop

```cpp
#include "Coordinator.hpp"

// Example Workflow
void runClient() {
    Coordinator gCoordinator;

    // 1. Initialize
    gCoordinator.init();       // ECS Logic
    gCoordinator.initRender(); // Window & Assets

    // 2. Register Logic
    gCoordinator.registerComponent<Transform>();
    gCoordinator.registerComponent<Sprite>();
    gCoordinator.registerComponent<InputComponent>();
    
    // 3. Register Systems
    auto& renderSys = gCoordinator.registerSystem<RenderSystem>();
    gCoordinator.setSystemSignature<RenderSystem, Transform, Sprite>();
    
    // 4. Game Loop
    while (gCoordinator.isOpen()) {
        
        // A. Input
        gCoordinator.processInput(); // Polls events -> Updates Local Player Component

        // B. Network (Incoming)
        // std::vector<Packet> packets = network.receive();
        // gCoordinator.processClientPackets(packets, time);

        // C. Logic
        gCoordinator.updateSystems(dt);

        // D. Render
        gCoordinator.beginFrame();
        gCoordinator.updateSystems(dt); // Calls RenderSystem::update
        gCoordinator.render();
    }
}
```