# Game Engine & Architecture

## Overview

This section covers the core architecture and design of the R-Type game engine, including the Entity Component System (ECS) that powers the game logic.

## What You'll Find Here

### Game Engine

Learn about the main `GameEngine` class that orchestrates the entire system, manages entities and components, and coordinates systems execution.

- **Class Overview**: High-level orchestrator design
- **Initialization**: Setup and configuration
- **Core Responsibilities**: Entity management, system coordination

### ECS (Entity Component System)

Understand the ECS architecture that separates concerns and enables flexible, data-driven game logic.

- **What is an ECS**: Core concepts and benefits
- **Entity**: The basic unit of game objects
- **Component Manager**: How components are stored and managed
- **System**: How systems process entities and components

## Architecture Principles

The R-Type game engine follows these key principles:

1. **Separation of Concerns**: Logic, data, and rendering are cleanly separated
2. **Data-Driven Design**: Behavior emerges from components rather than inheritance
3. **System Coordination**: Systems execute in a controlled, predictable order
4. **Thread-Safe Operations**: Safe for multithreaded game loops

## Quick Navigation

- Start with [What is an ECS](ECS/index.md) if you're new to entity component systems
- Read [Game Engine Overview](GameEngine/index.md) to understand the main orchestrator
- Explore individual ECS components to understand the architecture in detail
