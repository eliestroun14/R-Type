# R-Type Project Checklist

## COMMON PART (Evaluated at both defenses)

### Software Engineering Requirements (p.4-5)

#### Build System

- [ ] CMake mandatory (p.4)
- [ ] Mandatory package manager example: Conan, Vcpkg, or CMake CPM (p.4)
- [ ] Self-contained project without system dependencies (p.4)
- [ ] "The project must not rely on system-wide installed libraries or development headers"
- [ ] No copying of dependency source code into the repo (p.4)

#### Platforms

- [ ] Linux mandatory (client + server) (p.4)
- [ ] Cross-Platform Windows using Microsoft Visual C++ compiler (p.4)
- [ ] "The project MUST run on Linux, for both the client and the server"

#### Workflow

- [ ] Git with best practices: feature branches, merge requests, issues, tags (p.4)
- [ ] CI/CD recommended but no full-time dedicated member (p.4-5)
- [ ] Smart cache for dependencies in CI/CD (p.5)
- [ ] Linters/formatters recommended (clang-tidy, clang-format) (p.5)

### Documentation Requirements (p.5-7)

#### README (p.5)

- [ ] Short, practical, useful
- [ ] "project purpose, dependencies/requirements/supported platforms, build and usage instructions, license, authors/contacts, useful links or quick-start information"

#### Developer Documentation (p.5-6)

- [ ] Architectural diagrams (p.6)
- [ ] "Architectural diagrams (a typical 'layer/subsystem' view)"
- [ ] Description of main systems and implementation (p.6)
- [ ] Tutorials and How-To's (p.6)
- [ ] Contribution guidelines and code conventions (p.6)
- [ ] High-level documentation (not just Doxygen) (p.6)

#### Technical and Comparative Study (p.6)

- [ ] Justification of used technologies (p.6)
- [ ] Comparative study: Algorithms and Data Structures (p.6)
- [ ] Comparative study: Storage (persistence, reliability, constraints) (p.6)
- [ ] Comparative study: Security (vulnerabilities, monitoring) (p.6)

#### Documentation Format (p.6-7)

- [ ] Modern format accessible online (p.6)
- [ ] "markdown, reStructuredText, Sphinx, Gitbook, Doxygen, Wikis" (p.7)
- [ ] No PDF or .docx (p.6)

#### Protocol Documentation (p.7)

- [ ] Description of network commands and packets (p.7)
- [ ] "Someone SHOULD be able to write a new client for your server, just by reading the protocol documentation"
- [ ] RFC format recommended (p.7)

### Accessibility Requirements (p.7)

- [ ] Solutions for Physical and Motor Disabilities (p.7)
- [ ] Solutions for Audio and Visual Disabilities (p.7)
- [ ] Solutions for Mental and Cognitive Disabilities (p.7)

---

## PART 1: Software Architecture & First Game Prototype (p.8-13)

**Deadline: 4 weeks**

### General Objectives (p.8)

- [ ] Playable game with star-field, ships, Bydos enemies, missiles (p.8)
- [ ] "The game MUST be playable at the end of this part"
- [ ] Networked game with distinct clients and central server (p.8)
- [ ] Game engine foundations with decoupled subsystems: Rendering, Networking, Game Logic (p.8)

### Server (p.9)

#### Architecture

- [ ] Implementation of all game logic (p.9)
- [ ] "The server implements all the game logic"
- [ ] Server = authoritative source (p.9)

#### Requirements

- [ ] Notify clients for: spawn, movements, destructions, shots, deaths, actions of other clients (p.9)
- [ ] "The server MUST notify each client when a monster spawns, moves, is destroyed, fires, kills a player"
- [ ] Multithreading mandatory (p.9)
- [ ] "The server MUST be multithreaded, so that it does not block or wait for clients messages"
- [ ] Robustness: continues if client crashes, notifies others (p.9)
- [ ] "If a client crashes for any reason, the server MUST continue to work and MUST notify other clients"

#### Networking

- [ ] Asio allowed OR low-level API (BSD Sockets, Windows Sockets) (p.9)
- [ ] If low-level sockets: encapsulation with abstractions mandatory (p.9)
- [ ] "you MUST encapsulate them with proper abstractions"

### Client (p.10)

#### Architecture

- [ ] Graphical display of the game (p.10)
- [ ] Player input management (p.10)
- [ ] Gameplay logic on server (p.10)
- [ ] "everything related to gameplay logic shall occur on the server"

#### Server Authority

- [ ] Server has final authority (p.10)
- [ ] "the server MUST have authority on what happens in the end"
- [ ] Server authoritative for: deaths, pickups, high-impact effects (p.10)

#### Libraries

- [ ] SFML allowed (rendering/audio/input/network) OR SDL, Raylib (p.10)
- [ ] Forbidden: UE, Unity, Godot, existing engines (p.10)

#### Visual Elements (p.10)

- [ ] Player
- [ ] Monster
- [ ] Monster spawning powerup
- [ ] Enemy missile
- [ ] Player missile
- [ ] Stage obstacles
- [ ] Destroyable tile
- [ ] Background (starfield)

### Protocol (p.11)

#### Specifications

- [ ] Mandatory binary protocol (p.11)
- [ ] "You MUST design a binary protocol"
- [ ] Mandatory UDP for communications (p.11)
- [ ] "You MUST use UDP for communications between the server and the clients"
- [ ] TCP tolerated for specific cases with strong justification (p.11)
- [ ] ALL in-game communications via UDP (entities, movements, events) (p.11)
- [ ] "ALL in-game communications (entities, movements, events) MUST use UDP"

#### Robustness

- [ ] Handling of erroneous messages (p.11)
- [ ] Handling of buffer overflows (p.11)
- [ ] Malformed messages must not crash or compromise security (p.11)
- [ ] "Such malformed messages or packets MUST NOT lead the client or server to crash"

#### Documentation

- [ ] Protocol documentation mandatory (p.11)

### Game Engine (p.12)

#### Principles

- [ ] Create game engine prototype before the game (p.12)
- [ ] "it is important that you start by creating a prototype game engine"
- [ ] Maximum decoupling between systems (p.12)
- [ ] "decoupling is the most important thing you should focus on"

#### Patterns

- [ ] Look at Entity-Component-System (ECS) (p.12)
- [ ] Look at Mediator design pattern (p.12)
- [ ] "We recommend taking a look at the Entity-Component-System architectural pattern"

### Gameplay (p.13)

#### Background

- [ ] Mandatory slow horizontal scrolling (p.13)
- [ ] Star-field (space with stars, planets) (p.13)
- [ ] Scrolling not tied to CPU speed, use timers (p.13)
- [ ] "must NOT be tied to the CPU speed. Instead, you MUST use timers"

#### Controls & Entities

- [ ] Movement with arrow keys (p.13)
- [ ] Bydos slaves present (p.13)
- [ ] Missiles present (p.13)
- [ ] Random monster spawn on right side of screen (p.13)

#### Multiplayer

- [ ] 4 distinctly identifiable players (color, sprite, etc.) (p.13)
- [ ] "The four players in a game MUST be distinctly identifiable"

#### Assets & Audio

- [ ] R-Type sprites (from Internet or provided) (p.13)
- [ ] Basic sound design important (p.13)

---

## PART 2: Advanced Topics (p.14-22)

**Deadline: 3 weeks after part 1**

**Important note (p.14):** "Due to the scope of this part, bear in mind not everything have to be done to validate the project. However, it is expected some significant work to be done on one or more topics and features."

### TRACK #1: Advanced Architecture (p.15-17)

#### Ultimate Objective (p.15)

- [ ] Engine separated as standalone project (p.15)
- [ ] R-Type uses engine as dependency (p.15)
- [ ] Engine independent from R-Type (p.15)
- [ ] "your R-Type game will be using the engine as a library dependency"
- [ ] 2nd sample game using the standalone engine (p.15)
- [ ] "create a 2nd sample game (different from R-Type !), using your standalone game engine"

#### Modularity (p.16)

**Compile-time:**

- [ ] Choice of modules to compile via build system flags (p.16)

**Link-time:**

- [ ] Engine in several libraries (p.16)
- [ ] Developer chooses what to link with (p.16)

**Run-time plugin API:**

- [ ] Modules as shared-object libraries (p.16)
- [ ] Dynamic loading during runtime (p.16)

#### Engine Subsystems (p.16-17)

**Rendering Engine (p.16):**

- [ ] Features: 2.5D/3D, particle system, UI elements

**Physics Engine (p.16):**

- [ ] Collisions and gravity
- [ ] Advanced: deformation, breaking, bouncing

**Audio Engine (p.16):**

- [ ] Basic: background music
- [ ] Advanced: SFX, positional sounds

**Human-Machine Interface (p.16):**

- [ ] Basic: keyboard, gamepad
- [ ] Advanced: UI click events, touchpad, physical key referencing

**Message Passing Interface (p.16):**

- [ ] Event system
- [ ] Advanced: priorities, responses, synchronous messages

**Resources & Asset Management (p.16-17):**

- [ ] Managed by engine, referenced by ID/name
- [ ] Preloading or on-the-fly loading
- [ ] Resource cache

**Scripting (p.17):**

- [ ] Entity behaviors in external scripts
- [ ] Typically LUA or Python

#### Tooling (p.17)

**Developer Console:**

- [ ] Trigger actions/scripts/sounds (p.17)
- [ ] Customizable Console Variables (CVars) (p.17)

**In-game Metrics and Profiling:**

- [ ] Metrics: world position, CPU/Memory, FPS, Lagometer (p.17)

**World/Scene/Assets Editor:**

- [ ] Standalone or enabled by flag (p.17)
- [ ] Asset placement in world (p.17)
- [ ] "The quality of the editor(s) will be evaluated" (p.17)

---

### TRACK #2: Advanced Networking (p.18-20)

#### Multi-instance Server (p.18)

- [ ] Multiple game instances in parallel (p.18)
- [ ] "The ability for the server to run several different game instances in parallel"
- [ ] Lobby/Room system for matchmaking (p.18)
- [ ] User management: storage, sessions, authentication (p.18)
- [ ] User communication: text or voice chat (p.18)
- [ ] Game rules management per instance (p.18)
- [ ] Global scoreboard and/or ranking (p.18)
- [ ] Administration dashboard (text or web console) (p.18)

#### Data Transmission Efficiency and Reliability (p.18-19)

**Data Packing (p.18-19):**

- [ ] Appropriate data type sizes
- [ ] Space-efficient serialization
- [ ] Bit-level packing
- [ ] Optimization of struct alignment and padding
- [ ] Data quantization

**General-purpose Data Compression (p.19):**

- [ ] RLE (Run Length Encoding)
- [ ] Huffman encoding
- [ ] LZ4
- [ ] zlib
- [ ] Delta snapshot compression (game-specific)

**Network Errors Mitigation (p.19):**

- [ ] Prevention of UDP issues: loss, reordering, duplication
- [ ] "provide means to prevent any kind of issues caused by UDP unreliability"

**Message Reliability (p.19):**

- [ ] Reliable critical messages (connection, player death, etc.)
- [ ] Dedicated TCP channel OR UDP "ACK" patterns OR message duplication

#### High-level Networking Engine Architecture (p.20)

**Techniques to investigate:**

- [ ] Client-side prediction with server reconciliation (p.20)
- [ ] Low frequency server updates with entity state interpolation (p.20)
- [ ] Server-side lag compensation (p.20)
- [ ] Input delaying (p.20)
- [ ] Rollback netcode (p.20)

**Evaluation:**

- [ ] Real measurements: bandwidth usage, responsiveness to lag (p.20)
- [ ] "You are expected to be able to give and demonstrate real measurements"

---

### TRACK #3: Advanced Gameplay (p.21-22)

#### Players: Elements of Gameplay (p.21)

**Monsters (p.21):**

- [ ] Varied movement patterns
- [ ] Varied attacks
- [ ] Examples: snakes (level 2), ground turrets

**Levels (p.21):**

- [ ] Different themes
- [ ] Interesting gameplay twists

**Bosses (p.21):**

- [ ] Example: Dobkeratops (iconic first boss)

**Weapons (p.21):**

- [ ] Example: Force (attachable, detachable, protection, charged shots)

**Gameplay Rules (p.21):**

- [ ] Ability to change rules: friendly-fire, bonuses, difficulty, modes (coop/versus/pvp)

**Sound Design (p.21):**

- [ ] Music (or procedural)
- [ ] Environmental effects
- [ ] Sound effects

**Important note (p.21-22):** "The grading will not evaluate only quantitatively, but also qualitatively: having N or M kind of element is not the only things that matters. Having reusable subsystems to add content easily is equally important."

#### Game Designers: Content Creation Tools (p.22)

- [ ] APIs for runtime extensibility (plugins, DLLs, scripting) (p.22)
- [ ] "Your game engine SHOULD provide well-defined APIs allowing for runtime extensibility"
- [ ] Example: Lua to program entity behaviors (p.22)
- [ ] Ease of adding new content and behaviors (p.22)
- [ ] "how easy it is to add new content and behaviors in the game"
- [ ] Content editor tools (level-editor, monster editor, etc.) (p.22)
- [ ] Documentation: Critical Tutorials and How-To's (p.22)
- [ ] "Documentation is a critical part of any content creation tool"
