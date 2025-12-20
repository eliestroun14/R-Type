# Welcome to R-Type Docs

## About R-Type

R-Type is a modern C++ game engine project built from scratch. It demonstrates advanced software engineering practices including:

- **Entity Component System (ECS)** architecture for flexible game logic
- **Multithreaded logging** system for production debugging
- **Client-Server network architecture** for multiplayer gameplay
- **Modular design** with clear separation of concerns

---

## 🚀 Quick Start

### For New Contributors

1. Read the [Contributing Guide](Documentation/index.md)
2. Explore the [Game Engine Architecture](GameEngine&Architecture/index.md)

### For Game Development

1. Understand the [ECS Architecture](GameEngine&Architecture/ECS/index.md)
2. Learn about [Entity and Components](GameEngine&Architecture/ECS/entity.md)
3. Explore the [Game Engine](GameEngine&Architecture/GameEngine/index.md) class

### For Debugging

1. Set up the [Logger System](Logger/index.md)
2. Configure [Log Levels](Logger/configuration.md)
3. Use [Categories](Logger/usage.md) to filter logs

---

## 📚 Documentation Sections

| Section | Purpose |
|---------|---------|
| **Game Engine & Architecture** | Core engine design and ECS system |
| **Logger** | Logging system configuration and usage |
| **Tests** | Testing framework and best practices |
| **Build System** | CPM dependency management rationale |
| **Client** | Client-side implementation details |
| **Server** | Server-side implementation details and protocol |
| **Contributing** | How to contribute and documentation guidelines |

---

## 🎯 Key Concepts

### Entity Component System (ECS)

A data-oriented architecture where:

- **Entities** are identifiers for game objects
- **Components** store data associated with entities
- **Systems** process components and implement logic

[Learn more about ECS →](GameEngine&Architecture/ECS/index.md)

### Game Engine

The `GameEngine` class orchestrates the entire system:

- Manages entities and components
- Coordinates system execution
- Maintains game state

[Learn more about Game Engine →](GameEngine&Architecture/GameEngine/index.md)

### Logging

Thread-safe logging with:

- Multiple severity levels (Trace → Critical)
- Category filtering for noise reduction
- Colored console and file output

[Learn more about Logging →](Logger/index.md)

---

## 🛠️ Build & Setup

The project uses **CPM (C++ Package Manager)** for dependency management:

```bash
git clone https://github.com/eliestroun14/R-Type.git
cd R-Type
mkdir build && cd build
cmake ..
make
```

[Learn why we use CPM →](Cmake/cpm-rationale.md)

---

## 📖 Need Help?

- **Getting Started**: Check the [documentation guide](Contributing/overview.md)
- **Contributing**: Read [how to contribute](Contributing/overview.md)
- **Questions**: See the [FAQ](#faq) below

---

## FAQ

**Q: What is an Entity?**  
A: An Entity is a unique identifier representing a game object (player, enemy, projectile, etc.).

**Q: How do I add a new Component?**  
A: See the [Component Manager documentation](GameEngine&Architecture/ECS/component-manager.md).

**Q: How do I configure logging?**  
A: Check the [Logger Configuration](Logger/configuration.md) guide.

**Q: Why CPM and not Conan/vcpkg?**  
A: See the [CPM Design Rationale](Cmake/cpm-rationale.md).

---

## 📝 License

This project is part of the EPITECH curriculum. See [Expectations](Expectations/expectations.md) for details.
