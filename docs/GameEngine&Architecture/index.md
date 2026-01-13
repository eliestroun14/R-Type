# Game Engine & Architecture

## Overview

This section documents the core architecture behind the R-Type execution engine, centered around the `GameEngine` class which directly manages the ECS subsystems (`EntityManager`, `SystemManager`, `RenderManager`).

`GameEngine` acts as a **facade** that owns and coordinates the core ECS managers. It provides a unified API for entity lifecycle, component management, system execution, rendering, and audio.

## What You'll Find Here

### Game Engine

Learn about the `GameEngine` class responsible for:

- **Initialization**: Creating EntityManager, SystemManager, RenderManager, and AudioManager
- **Entity Management**: Spawning, destroying, and querying entities
- **Component Management**: Registering component types and adding/removing components
- **System Management**: Registering systems and updating them each frame
- **Rendering**: Coordinating window, input, and sprite rendering via RenderManager
- **Audio**: Managing music and sound effects via AudioManager

### Coordinator (Gameplay Layer)

The `Coordinator` is a **gameplay-level class** that wraps a `GameEngine` instance:

- **Network Integration**: Processes server/client packets to sync game state
- **Entity Factories**: Provides helpers to spawn players, enemies, projectiles with proper components
- **Packet Handlers**: Translates network messages into ECS operations
- **Packet Builders**: Creates outgoing packets based on entity state

The Coordinator does not replace the GameEngine—it uses it as its ECS backend.

---

## Architecture Principles

This new engine layer follows several key architectural principles:

1. **Direct Ownership**
   The `GameEngine` directly owns its subsystems (EntityManager, SystemManager, RenderManager, AudioManager), providing a clear facade API for ECS operations.

2. **Separation of Concerns**
   Each manager has a clear responsibility:
   - `EntityManager`: Entity lifecycle and component storage
   - `SystemManager`: System registration and execution
   - `RenderManager`: Window, input, and rendering (client-side only)
   - `AudioManager`: Music and sound effects (client-side only)

3. **Coordinator as Integration Layer**
   The gameplay `Coordinator` wraps `GameEngine` to handle:
   - Network packet processing and state synchronization
   - Entity factory methods (players, enemies, projectiles)
   - Translation between network protocol and ECS operations

4. **Template-Based API**
   GameEngine uses C++ templates for type-safe component and system operations:
   - `registerComponent<T>()` and `addComponent<T>()`
   - `registerSystem<T>()` and `setSystemSignature<T>()`

5. **Testability**
   GameEngine can be instantiated and tested independently, with optional RenderManager/AudioManager initialization for client-side features.

---

## Quick Navigation

- Read the updated [Game Engine Overview](GameEngine/index.md) to understand the ECS facade and manager APIs
- Explore the [Coordinator Documentation](GameEngine/Coordinator/index.md) for details about network integration and entity factories

