# Game Engine & Architecture

## Overview

This section documents the core architecture behind the R-Type execution engine, centered around the `GameEngine` class used to coordinate runtime behavior, input flow, update cycles, and rendering depending on the network execution mode (SERVER, CLIENT, STANDALONE).

`GameEngine` acts as a **runtime controller** that delegates all game-logic operations to an internal `Coordinator`. It does not directly manage entities or components that responsibility is delegated to the Coordinator.

## What You'll Find Here

### Game Engine

Learn about the `GameEngine` class responsible for:

- **Initialization**: Creating and preparing the Coordinator
- **Frame Execution**: Processing input, updating logic, and rendering
- **Network-Mode Adaptation**: Switching behavior depending on SERVER / CLIENT / STANDALONE modes
- **Delegation Model**: Forwarding operations to the underlying Coordinator

### Coordinator (Overview)

The Coordinator serves as the unified execution backend:

- **Systems Execution** (`updateSystems(dt)`)
- **Input Handling** (`processInput()`)
- **Rendering** (`render()`)

The `GameEngine` does not implement logic; it **calls** the Coordinator, which contains the actual subsystems.

This keeps the engine modular and easy to extend

---

## Architecture Principles

This new engine layer follows several key architectural principles:

1. **Clear Delegation**
   The `GameEngine` itself contains no game logic. All operations are delegated to the Coordinator, simplifying testing and reasoning.

2. **Mode-Dependent Execution**
   Rendering and input processing depend on the current network execution mode:
   - `SERVER` ignores rendering and normally avoids client-side input
   - `CLIENT` only renders and processes input
   - `STANDALONE` runs full input → update → render flow

3. **Minimal Responsibility**
   The `GameEngine` only orchestrates:
   - `processInput()`
   - `update()`
   - `render()`
   - combined `process()` loop

4. **Loop Safety**
   Each call is isolated and predictable:
   - No shared state leaks
   - No implicit coupling between update phases

5. **Replaceable Coordinator**
   The Coordinator can be replaced or mocked for testing, allowing high-level validation of runtime behavior without needing full ECS execution.

---

## Quick Navigation

- Read the updated [Game Engine Overview](GameEngine/index.md) to understand the runtime orchestration layer
- Explore the [Coordinator Documentation](ECS/coordinator.md) for details about system execution, input handling, and rendering

