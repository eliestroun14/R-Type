# Server Architecture

## Overview

The R-Type server implements an authoritative game server architecture using UDP for network communication. It manages game state, processes player inputs, and synchronizes the game world across all connected clients.

## Key Features

- **ECS-Based Architecture**: Pure Entity Component System for game logic
- **UDP Protocol**: Low-latency network communication
- **Component Snapshots**: Efficient state synchronization
- **Delta Encoding**: Bandwidth optimization for large game worlds
- **Authoritative Server**: Server-side validation
- **Bidirectional Simulation**: Both server and client run simulations

## Components

### Network Layer

The server uses a custom binary protocol over UDP for real-time communication:

- **Connection Management**: Handle client connections and disconnections
- **Reliability Layer**: Optional reliable delivery for critical packets
- **Packet Processing**: Efficient serialization and deserialization

See [Protocol Documentation](protocol.md) for detailed specifications.

### Game Simulation

The server runs a fixed-timestep simulation loop:

```
┌─────────────────────────────────┐
│   Server Tick (60 Hz)           │
├─────────────────────────────────┤
│ 1. Process Input Packets        │
│ 2. Update ECS Systems           │
│ 3. Detect Collisions            │
│ 4. Generate Component Snapshots │
│ 5. Send to Clients              │
└─────────────────────────────────┘
```

### State Synchronization

The server uses component-based snapshots with different update frequencies:

| Component | Frequency | Reason |
|-----------|-----------|--------|
| Transform | 60 Hz | Critical for smooth movement |
| Velocity | 30 Hz | Predictable, can interpolate |
| Health | 20 Hz | Changes infrequently |
| Weapon | 10 Hz | Changes on events only |
| AI | 5 Hz | Highly predictable |

## Architecture Diagrams

### Client-Server Communication Flow

```
Client                          Server
  │                               │
  ├─── CLIENT_CONNECT ──────────>│
  │                               ├─── Validate
  │<────── SERVER_ACCEPT ─────────┤
  │                               │
  ├─── PLAYER_INPUT (60Hz) ─────>│
  │                               ├─── Update Simulation
  │<── TRANSFORM_SNAPSHOT (60Hz)─┤
  │<── HEALTH_SNAPSHOT (20Hz) ───┤
  │<── GAME_EVENTS ──────────────┤
  │                               │
  ├─── HEARTBEAT (1Hz) ─────────>│
  │                               │
```

### ECS Component Updates

```
┌──────────────────────────────────────────────┐
│          Server ECS Systems                  │
├──────────────────────────────────────────────┤
│                                              │
│  InputSystem ────> TransformSystem           │
│                          │                   │
│  PhysicsSystem ──────────┤                   │
│                          │                   │
│  AISystem ───────────────┤                   │
│                          ▼                   │
│                    CollisionSystem           │
│                          │                   │
│  WeaponSystem ───────────┤                   │
│                          │                   │
│                          ▼                   │
│                  Network Snapshot            │
│                     Generator                │
│                          │                   │
│                          ▼                   │
│                  Send to Clients             │
└──────────────────────────────────────────────┘
```

## Performance Considerations

### Bandwidth Optimization

The server implements several bandwidth optimization techniques:

1. **Delta Snapshots**: Only send changed entities
   - Example: 5/40 entities moving = 84% bandwidth reduction

2. **Component Frequency Tuning**: Update rates based on criticality
   - Critical (Transform): 60 Hz
   - Non-critical (AI): 5 Hz

3. **Interest Management**: Send only relevant entities to each client
   - Spatial partitioning
   - Visibility culling

### Scalability

The server is designed to handle:

- **Players**: Up to 4 concurrent players per game instance
- **Entities**: 100-200 active entities per game
- **Tick Rate**: 60 updates per second
- **Bandwidth**: ~50 KB/s per client (optimized)

## Configuration

Server configuration includes:

```cpp
struct ServerConfig {
    uint16_t port;                  // Default: 4242
    uint8_t  max_players;           // Default: 2
    uint16_t tick_rate;             // Default: 60 Hz
    uint16_t snapshot_rate;         // Default: 30 Hz
    uint32_t timeout_ms;            // Default: 10000 ms
    bool     enable_delta_encoding; // Default: false
};
```

## Related Documentation

- [Network Protocol Specification](protocol.md) - Complete protocol reference
- [ECS Architecture](../GameEngine&Architecture/GameEngine/Coordinator/ECS/index.md) - Entity Component System details
- [Testing Guide](../Test/justification.md) - How to test server components

## Getting Started

For information on how to build and run the server, see the main [README](../../README.md).
