# R-Type

> A modern C++ game engine project built from scratch with a networked multiplayer shoot'em'up game implementation

## Overview

R-TYPE is a networked multiplayer implementation of the classic horizontal scrolling shooter game, built on a custom game engine demonstrating advanced software engineering practices. This project features:

- **Entity Component System (ECS)** architecture for flexible game logic
- **Client-Server network architecture** for multiplayer gameplay
- **Modular design** with clear separation of concerns

Multiple players can battle the evil Bydos across the network in real-time with a client-server model using UDP-based communication.

## Features

- **Multiplayer Gameplay**: Up to 4 players can join the same game instance
- **Networked Architecture**: Client-server model with UDP-based communication
- **Custom Game Engine**: Built from scratch with modular subsystems
- **Cross-platform Support**: Runs on both Linux and Windows
- **Entity-Component-System**: Decoupled architecture for extensibility
- **Classic R-TYPE Elements**: Enemies, missiles, power-ups, and scrolling starfield

## Gameplay

- Control your spaceship using arrow keys
- Shoot missiles to destroy enemy Bydos
- Survive waves of enemies with varying patterns
- Cooperate with other players across the network
- Four players are visually distinct with different colors/sprites

## Technical Stack

- **Language**: C++ (C++17 or higher)
- **Build System**: CMake 3.15+
- **Package Manager**: CPM (C++ Package Manager)
- **Graphics Library**: SFML
- **Networking**: UDP protocol with Asio
- **Logging**: spdlog (multithreaded logging)
- **Testing**: Google Test (gtest)

## Dependencies

- C++17 compatible compiler (GCC, Clang)
- CMake 3.15 or higher

All third-party dependencies are managed through CPM and will be automatically downloaded during the build process, including:

- SFML (Graphics library)
- Asio (Network library)
- spdlog (Logging library)
- Google Test (Testing framework)

## Building the Project

### Linux

```bash
# Clone the repository
git clone <repository-url>
cd r-type

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Binaries will be in build/
./r-type_server
./r-type_client
```

### Windows

```bash
# Clone the repository
git clone <repository-url>
cd r-type

# Create build directory
mkdir build
cd build

# Configure with CMake (Visual Studio)
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build . --config Release

# Binaries will be in build/Release/
r-type_server.exe
r-type_client.exe
```

## Usage

### Starting the Server

```bash
./r-type_server [port]
```

Default port is 4242 if not specified.

### Starting the Client

```bash
./r-type_client [server_ip] [port]
```

Connect to localhost:4242 by default if parameters are omitted.

### Controls

- **Arrow Keys**: Move spaceship (Up/Down/Left/Right)
- **Space**: Shoot missile
- **ESC**: Exit game

## Architecture

The project follows a modular, data-driven architecture with clear separation of concerns:

### Core Principles

1. **Separation of Concerns**: Logic, data, and rendering are cleanly separated
2. **Data-Driven Design**: Behavior emerges from components rather than inheritance
3. **System Coordination**: Systems execute in a controlled, predictable order
4. **Thread-Safe Operations**: Safe for multithreaded game loops

### Main Components

- **Entity-Component-System (ECS)**: Core architecture for game object management
  - **Entities**: Identifiers for game objects
  - **Components**: Data storage associated with entities
  - **Systems**: Process components and implement logic
  - **Component Manager**: Manages component storage and access

- **Game Engine**: Main orchestrator that manages entities, components, and system coordination
- **Rendering Engine**: Graphics and visual output via SFML
- **Network Layer**: UDP-based client-server communication with Asio

## Network Protocol

The game uses a custom binary UDP protocol (version 1.0) for real-time communication:

- **Transport Layer**: UDP (User Datagram Protocol) for low-latency transmission
- **Format**: Binary with network byte order (big-endian)
- **Maximum Packet Size**: 1200 bytes (to avoid fragmentation)
- **Connection Management**: Client registration and disconnection
- **Game State Updates**: Entity positions, states, and events
- **Input Commands**: Player actions sent to server
- **Event Notifications**: Spawns, deaths, scores
- **Reliability Mechanisms**: Optional reliability features over UDP

For detailed protocol specification, see [Network Protocol Documentation](docs/Network/network.tex)

## Documentation

Comprehensive documentation is available in the `docs/` folder:

- **[Game Engine & Architecture](docs/GameEngine&Architecture/index.md)** - Core engine design and ECS system
- **[ECS System](docs/GameEngine&Architecture/ECS/index.md)** - Detailed ECS architecture guide
- **[Logger System](docs/Logger/index.md)** - Logging configuration and usage
- **[Network Protocol](docs/Network/network.tex)** - Binary protocol specification
- **[Contributing Guidelines](docs/Contributing/contributing.md)** - Code style and conventions
- **[Testing Guide](docs/Test/tutorial.md)** - Testing framework and best practices
- **[Build System](docs/Cmake/cpm-rationale.md)** - CPM dependency management rationale

See all documentation at [ReadTheDocs](https://r-type-rennes.readthedocs.io/en/dev/)

## Testing

The project uses Google Test (gtest) for unit testing:

```bash
# Run all tests
cd build
ctest

# Or run test executable directly
./tests/ecs_tests

# Run with verbose output
ctest --verbose
```

For detailed testing setup and guidelines, see the [Testing Tutorial](docs/Test/tutorial.md)

## Contributing

We welcome contributions! Please see our [Contributing Guidelines](docs/Contributing/contributing.md) for:

- **Commit Message Convention**: We follow Conventional Commits 1.0.0 specification
- **Commit Types**: feat, fix, docs, style, refactor, perf, test, build, ci, chore
- **Code Style**: Proper naming conventions and formatting
- **Testing Requirements**: All new features must include tests
- **Merge Request Process**: Standard review and merge workflow

## Team

See the [Contributors](#contributors) section below for team members and their GitHub profiles.

## Useful Links

- [Original R-TYPE Game](https://en.wikipedia.org/wiki/R-Type)
- [Project Documentation](docs/)
- [Issue Tracker](https://github.com/eliestroun14/R-Type/issues)
- [SFML Documentation](https://www.sfml-dev.org/documentation/)

## Acknowledgments

- Epitech for the project subject
- Original R-TYPE developers at Irem
- Open source community for libraries and tools

## Contact

For questions or support, please open an issue on the repository

---

**Status**: In Development | **Version**: 1.0.0 | **Last Updated**: December 2025

## Contributors

| [Elie Stroun](https://github.com/eliestroun14)<br/><img src="https://avatars.githubusercontent.com/u/146193362?v=4" width=92> | [Manech Dubreil](https://github.com/nitrached)<br/><img src="https://avatars.githubusercontent.com/u/146340173?v=4" width=92> | [Timéo Trégarot](https://github.com/Timeotr)<br/><img src="https://avatars.githubusercontent.com/u/146066700?v=4" width=92> | [Pablo Jesus](https://github.com/Smoulmouc)<br/><img src="https://avatars.githubusercontent.com/u/130741436?v=4" width=92> | [Aymeric Jouannet-Mimy](https://github.com/aymericJM)<br/><img src="https://avatars.githubusercontent.com/u/146735159?v=4" width=92> |
|:---:|:---:|:---:|:---:|:---:|
