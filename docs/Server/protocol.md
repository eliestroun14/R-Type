# Network Protocol Specification

## Overview

The R-Type network protocol is a binary protocol designed for real-time multiplayer game communication over UDP. It implements an ECS (Entity Component System) architecture with component-based snapshots for efficient network synchronization.

**Protocol Version:** 1.0  
**Transport Layer:** UDP (User Datagram Protocol)  
**Protocol Identifier:** `0x5254` ('RT')

---

## Packet Structure

Every packet consists of two main parts:

```
+-------------------------------------------+
|  Header (12 bytes)                        |
+-------------------------------------------+
|  Payload (variable, 0-1188 bytes)         |
+-------------------------------------------+
```

### Packet Header

The packet header is a fixed 12-byte structure present in all protocol messages:

```cpp
struct PacketHeader {
    uint16_t magic;           // Protocol identifier: 0x5254 ('RT')
    uint8_t  packet_type;     // Packet type identifier
    uint8_t  flags;           // Control flags
    uint32_t sequence_number; // Monotonically increasing sequence
    uint32_t timestamp;       // Milliseconds since connection
};
```

| Field | Size | Description |
|-------|------|-------------|
| `magic` | 2 bytes | Protocol magic number (0x5254). Packets with invalid magic MUST be discarded. |
| `packet_type` | 1 byte | Identifies the packet type. Valid range: 0x01-0x7F. |
| `flags` | 1 byte | Bitfield for packet control flags. |
| `sequence_number` | 4 bytes | Monotonically increasing packet sequence number for ordering and acknowledgment. |
| `timestamp` | 4 bytes | Timestamp in milliseconds since connection establishment. |

### Packet Flags

```cpp
enum class PacketFlags : uint8_t {
    FLAG_RELIABLE    = 0x01,  // Requires acknowledgment
    FLAG_COMPRESSED  = 0x02,  // Payload is compressed
    FLAG_ENCRYPTED   = 0x04,  // Payload is encrypted
    FLAG_FRAGMENTED  = 0x08,  // Part of fragmented message
    FLAG_PRIORITY    = 0x10,  // High priority processing
    // Bits 5-7 reserved for future use
};
```

---

## Packet Types

Packet types are organized into functional categories:

| Range | Category | Description |
|-------|----------|-------------|
| 0x01-0x0F | Connection | Session establishment and management |
| 0x10-0x1F | Input | Player input and control messages |
| 0x20-0x3F | World State | Entity lifecycle and ECS component snapshots |
| 0x40-0x5F | Game Events | Gameplay events and actions |
| 0x60-0x6F | Game Control | Game lifecycle and state transitions |
| 0x70-0x7F | Protocol Control | Reliability and connection maintenance |

---

## Connection Management (0x01-0x0F)

### CLIENT_CONNECT (0x01)

Sent by the client to initiate a connection.

```cpp
struct ClientConnect {
    PacketHeader header;        // type = 0x01
    uint8_t  protocol_version;  // Version = 1
    char     player_name[32];   // Player name (null-terminated)
    uint32_t client_id;         // Unique client identifier
};
// Total size: 49 bytes
```

**Usage:**
1. Client generates unique `client_id`
2. Client sends `CLIENT_CONNECT` to server
3. Server responds with `SERVER_ACCEPT` or `SERVER_REJECT`

### SERVER_ACCEPT (0x02)

Sent by the server to accept a client connection.

```cpp
struct ServerAccept {
    PacketHeader header;            // type = 0x02
    uint32_t assigned_player_id;    // Unique player ID assigned by server
    uint8_t  max_players;           // Maximum players allowed
    uint32_t game_instance_id;      // Unique game instance ID
    uint16_t server_tickrate;       // Server update rate (Hz)
};
// Total size: 23 bytes
```

### SERVER_REJECT (0x03)

Sent by the server to reject a connection attempt.

```cpp
struct ServerReject {
    PacketHeader header;         // type = 0x03
    uint8_t reject_code;         // Reason code
    char    reason_message[64];  // Human-readable reason
};
// Total size: 77 bytes
```

**Reject Codes:**

```cpp
enum class RejectCodes : uint8_t {
    REJECT_SERVER_FULL                   = 0x00,
    REJECT_INCOMPATIBLE_PROTOCOL_VERSION = 0x01,
    REJECT_INVALID_PLAYER_NAME           = 0x02,
    REJECT_BANNED_CLIENT                 = 0x03,
    REJECT_GENERIC_ERROR                 = 0xFF
};
```

### CLIENT_DISCONNECT (0x04)

Sent by either client or server to terminate the connection.

```cpp
struct ClientDisconnect {
    PacketHeader header;     // type = 0x04, FLAG_RELIABLE
    uint32_t client_id;      // Client identifier
    uint8_t  reason;         // Disconnect reason
};
// Total size: 17 bytes
```

**Disconnect Reasons:**

```cpp
enum class DisconnectReasons : uint8_t {
    REASON_NORMAL_DISCONNECT = 0x00,
    REASON_TIMEOUT          = 0x01,
    REASON_KICKED_BY_SERVER = 0x02,
    REASON_CLIENT_ERROR     = 0x03,
    REASON_GENERIC_ERROR    = 0xFF
};
```

!!! note
    This packet MUST have the `FLAG_RELIABLE` flag set.

### HEARTBEAT (0x05)

Sent periodically to maintain the connection.

```cpp
struct HeartBeat {
    PacketHeader header;  // type = 0x05
    uint32_t player_id;   // Player identifier
};
// Total size: 16 bytes
```

**Usage:**
- Client SHOULD send every 1-2 seconds
- Server SHOULD disconnect clients that don't send heartbeat within 10 seconds

---

## Player Input (0x10-0x1F)

### PLAYER_INPUT (0x10)

Contains player input state. Sent frequently (typically every frame or when input changes).

```cpp
struct PlayerInput {
    PacketHeader header;         // type = 0x10
    uint32_t player_id;          // Player identifier
    uint16_t input_state;        // Bitfield of input states
    uint16_t aim_direction_x;    // Aim direction X (normalized)
    uint16_t aim_direction_y;    // Aim direction Y (normalized)
};
// Total size: 24 bytes
```

**Input Flags:**

```cpp
enum class InputFlags : uint16_t {
    INPUT_MOVE_UP        = 0x0001,
    INPUT_MOVE_DOWN      = 0x0002,
    INPUT_MOVE_LEFT      = 0x0004,
    INPUT_MOVE_RIGHT     = 0x0008,
    INPUT_FIRE_PRIMARY   = 0x0010,
    INPUT_FIRE_SECONDARY = 0x0020,
    INPUT_ACTION_SPECIAL = 0x0040,
    // Bits 7-15 reserved for future actions
};
```

---

## Entity Management (0x20-0x23)

### Entity Types

```cpp
enum class EntityTypes : uint8_t {
    ENTITY_TYPE_PLAYER             = 0x01,
    ENTITY_TYPE_ENEMY              = 0x02,
    ENTITY_TYPE_ENEMY_BOSS         = 0x03,
    ENTITY_TYPE_PROJECTILE_PLAYER  = 0x04,
    ENTITY_TYPE_PROJECTILE_ENEMY   = 0x05,
    ENTITY_TYPE_POWERUP            = 0x06,
    ENTITY_TYPE_OBSTACLE           = 0x07,
    ENTITY_TYPE_BG_ELEMENT         = 0x08
};
```

### ENTITY_SPAWN (0x21)

Notifies clients of a new entity entering the game world.

```cpp
struct EntitySpawn {
    PacketHeader header;           // type = 0x21, FLAG_RELIABLE
    uint32_t entity_id;            // Unique entity identifier
    uint8_t  entity_type;          // Entity type
    uint16_t position_x;           // Initial X position
    uint16_t position_y;           // Initial Y position
    uint8_t  mob_variant;          // Sub-type for mobs
    uint8_t  initial_health;       // Initial health
    uint16_t initial_velocity_x;   // Initial X velocity
    uint16_t initial_velocity_y;   // Initial Y velocity
    uint8_t  is_playable;          // 0=NPC, 1=playable entity
};
// Total size: 28 bytes
```

### ENTITY_DESTROY (0x22)

Notifies clients that an entity has been destroyed or removed.

```cpp
struct EntityDestroy {
    PacketHeader header;          // type = 0x22, FLAG_RELIABLE
    uint32_t entity_id;           // Entity identifier
    uint8_t  destroy_reason;      // Reason for destruction
    uint16_t final_position_x;    // Final X position (for animations)
    uint16_t final_position_y;    // Final Y position (for animations)
};
// Total size: 21 bytes
```

**Destroy Reasons:**

```cpp
enum class EntityDestroyReasons : uint8_t {
    DESTROY_KILLED_BY_PLAYER = 0x00,
    DESTROY_KILLED_BY_ENEMY  = 0x01,
    DESTROY_OUT_OF_BOUNDS    = 0x02,
    DESTROY_TIMEOUT_DESPAWN  = 0x03,
    DESTROY_LEVEL_TRANSITION = 0x04
};
```

---

## ECS Component System

The protocol uses a pure ECS architecture where game state is synchronized through component snapshots.

### Component Types

```cpp
enum class ComponentType : uint8_t {
    COMPONENT_TRANSFORM  = 0x01,  // Position, rotation, scale
    COMPONENT_VELOCITY   = 0x02,  // Linear velocity
    COMPONENT_HEALTH     = 0x03,  // Health/shield
    COMPONENT_WEAPON     = 0x04,  // Weapon state
    COMPONENT_AI         = 0x05,  // AI behavior
    COMPONENT_FORCE      = 0x06,  // R-Type Force attachment
    COMPONENT_COLLISION  = 0x07,  // Collision bounds
    COMPONENT_SPRITE     = 0x08,  // Visual representation
    COMPONENT_ANIMATION  = 0x09,  // Animation state
    COMPONENT_POWERUP    = 0x0A,  // Powerup effect
    COMPONENT_SCORE      = 0x0B,  // Score tracking
    COMPONENT_INPUT      = 0x0C,  // Input state
    COMPONENT_PHYSICS    = 0x0D,  // Physics properties
    COMPONENT_LIFETIME   = 0x0E,  // Auto-destroy after time
    COMPONENT_PARENT     = 0x0F,  // Parent-child relationship
};
```

### Component Data Structures

#### Transform Component

```cpp
struct ComponentTransform {
    int16_t  pos_x;
    int16_t  pos_y;
    uint16_t rotation;  // 0-65535 maps to 0-360 degrees
    uint16_t scale;     // Fixed point: 1000 = 1.0x scale
};
// Size: 8 bytes
```

#### Velocity Component

```cpp
struct ComponentVelocity {
    int16_t vel_x;
    int16_t vel_y;
    int16_t acceleration_x;
    int16_t acceleration_y;
};
// Size: 8 bytes
```

#### Health Component

```cpp
struct ComponentHealth {
    uint8_t current_health;
    uint8_t max_health;
    uint8_t current_shield;
    uint8_t max_shield;
};
// Size: 4 bytes
```

#### Weapon Component

```cpp
struct ComponentWeapon {
    uint8_t  weapon_type;
    uint8_t  ammo_count;
    uint16_t cooldown_remaining;  // Milliseconds
    uint8_t  power_level;         // 1-5 for R-Type
};
// Size: 5 bytes
```

#### AI Component

```cpp
struct ComponentAI {
    uint8_t  ai_state;
    uint8_t  behavior_type;
    uint32_t target_entity_id;  // 0 = no target
    uint16_t state_timer;       // Time in current state (ms)
};
// Size: 8 bytes
```

#### Force Component (R-Type Specific)

```cpp
struct ComponentForce {
    uint32_t parent_ship_id;     // Ship this Force is attached to
    uint8_t  attachment_point;   // 0=front, 1=back, 2=detached
    uint8_t  force_level;        // Power level
    uint8_t  charge_state;       // Charge beam state
};
// Size: 7 bytes
```

---

## Component Snapshots (0x24-0x2F)

!!! info "ECS Architecture"
    This approach is optimal for ECS because:
    
    1. Each component type has its own packet
    2. Server iterates over one component type at a time (cache-friendly)
    3. Allows different update frequencies per component
    4. Optimal compression (homogeneous data)
    5. Aligned with pure ECS architecture (one system = one component type)

### Update Frequencies

| Component | Frequency | Reason |
|-----------|-----------|--------|
| Transform | 60 Hz | Critical: smooth movement |
| Velocity | 30 Hz | Medium: prediction possible |
| Health | 20 Hz | Low: changes are rare |
| Weapon | 10 Hz | Low: changes are rare |
| AI | 5 Hz | Very low: predictable |
| Animation | 15 Hz | Medium: visible but interpolable |

### TRANSFORM_SNAPSHOT (0x24)

Sent frequently (30-60 Hz) for smooth movement.

```cpp
struct TransformSnapshot {
    PacketHeader header;       // type = 0x24
    uint32_t world_tick;       // Server tick number
    uint16_t entity_count;     // Number of entities
    // Followed by entity_count entries of:
    // [uint32_t entity_id][ComponentTransform data (8 bytes)]
};
// Base size: 18 bytes + (entity_count × 12) bytes
// Example: 20 entities = 18 + 240 = 258 bytes
```

### VELOCITY_SNAPSHOT (0x25)

Sent moderately (20-30 Hz).

```cpp
struct VelocitySnapshot {
    PacketHeader header;       // type = 0x25
    uint32_t world_tick;
    uint16_t entity_count;
    // Followed by: [uint32_t entity_id][ComponentVelocity data (8 bytes)]
};
// Base size: 18 bytes + (entity_count × 12) bytes
```

### HEALTH_SNAPSHOT (0x26)

Sent less frequently (10-20 Hz) as it's less critical for display.

```cpp
struct HealthSnapshot {
    PacketHeader header;       // type = 0x26
    uint32_t world_tick;
    uint16_t entity_count;
    // Followed by: [uint32_t entity_id][ComponentHealth data (4 bytes)]
};
// Base size: 18 bytes + (entity_count × 8) bytes
// More compact! 20 entities = 18 + 160 = 178 bytes
```

### WEAPON_SNAPSHOT (0x27)

Sent rarely (5-10 Hz) except on changes.

```cpp
struct WeaponSnapshot {
    PacketHeader header;       // type = 0x27
    uint32_t world_tick;
    uint16_t entity_count;
    // Followed by: [uint32_t entity_id][ComponentWeapon data (5 bytes)]
};
// Base size: 18 bytes + (entity_count × 9) bytes
```

### AI_SNAPSHOT (0x28)

Sent rarely (2-5 Hz) as AI is predictable.

```cpp
struct AISnapshot {
    PacketHeader header;       // type = 0x28
    uint32_t world_tick;
    uint16_t entity_count;
    // Followed by: [uint32_t entity_id][ComponentAI data (8 bytes)]
};
// Base size: 18 bytes + (entity_count × 12) bytes
```

### ANIMATION_SNAPSHOT (0x29)

Sent moderately (10-20 Hz).

```cpp
struct AnimationSnapshot {
    PacketHeader header;       // type = 0x29
    uint32_t world_tick;
    uint16_t entity_count;
    // Followed by: [uint32_t entity_id][ComponentAnimation data (7 bytes)]
};
// Base size: 18 bytes + (entity_count × 11) bytes
```

---

## Component Lifecycle

### COMPONENT_ADD (0x2A)

Used when an entity gains a new component.

```cpp
struct ComponentAdd {
    PacketHeader header;      // type = 0x2A, FLAG_RELIABLE
    uint32_t entity_id;       // Target entity
    uint8_t  component_type;  // ComponentType enum
    uint8_t  data_size;       // Size of component data
    // Followed by component data (variable size)
};
// Base size: 18 bytes + component_data_size
```

**Example:** Player picks up powerup → adds `ComponentPowerup`

### COMPONENT_REMOVE (0x2B)

Used when a component is removed from an entity.

```cpp
struct ComponentRemove {
    PacketHeader header;      // type = 0x2B, FLAG_RELIABLE
    uint32_t entity_id;       // Target entity
    uint8_t  component_type;  // ComponentType to remove
};
// Size: 17 bytes
```

**Example:** Powerup expires → removes `ComponentPowerup`

---

## Delta Snapshots (Advanced Optimization)

Delta snapshots send only the changes since the last snapshot, providing massive bandwidth savings.

### TRANSFORM_SNAPSHOT_DELTA (0x2C)

```cpp
struct TransformSnapshotDelta {
    PacketHeader header;      // type = 0x2C
    uint32_t world_tick;
    uint32_t base_tick;       // Reference tick
    uint16_t entity_count;    // Only modified entities
    // Followed by: [uint32_t entity_id][ComponentTransform data]
};
```

**Bandwidth Savings Example:**
- Full snapshot (40 entities): 18 + 40×12 = 498 bytes
- Delta snapshot (5 changed): 18 + 5×12 = 78 bytes
- **84% bandwidth reduction!**

### HEALTH_SNAPSHOT_DELTA (0x2D)

```cpp
struct HealthSnapshotDelta {
    PacketHeader header;      // type = 0x2D
    uint32_t world_tick;
    uint32_t base_tick;
    uint16_t entity_count;
    // Followed by: [uint32_t entity_id][ComponentHealth data]
};
```

---

## Full Entity State

### ENTITY_FULL_STATE (0x2E)

Used to synchronize multiple components of a specific entity (e.g., when a player reconnects).

```cpp
struct EntityFullState {
    PacketHeader header;        // type = 0x2E, FLAG_RELIABLE
    uint32_t entity_id;
    uint8_t  entity_type;       // EntityTypes enum
    uint8_t  component_count;   // Number of components
    // Followed by component_count entries of:
    // [uint8_t component_type][uint8_t size][component_data]
};
```

---

## Game Events (0x40-0x5F)

### PLAYER_HIT (0x40)

```cpp
struct PlayerHit {
    PacketHeader header;         // type = 0x40, FLAG_RELIABLE
    uint32_t player_id;          // Player who was hit
    uint32_t attacker_id;        // Entity that caused damage
    uint8_t  damage;             // Damage amount
    uint8_t  remaining_health;   // Health after damage
    uint8_t  remaining_shield;   // Shield after damage
    int16_t  hit_pos_x;          // Hit location X
    int16_t  hit_pos_y;          // Hit location Y
};
// Total size: 27 bytes
```

### PLAYER_DEATH (0x41)

```cpp
struct PlayerDeath {
    PacketHeader header;           // type = 0x41, FLAG_RELIABLE
    uint32_t player_id;            // Player who died
    uint32_t killer_id;            // Entity that killed player
    uint32_t score_before_death;   // Player's score
    int16_t  death_pos_x;          // Death location X
    int16_t  death_pos_y;          // Death location Y
};
// Total size: 28 bytes
```

### SCORE_UPDATE (0x42)

```cpp
struct ScoreUpdate {
    PacketHeader header;      // type = 0x42
    uint32_t player_id;       // Player whose score changed
    uint32_t new_score;       // Updated total score
    int16_t  score_delta;     // Change in score (can be negative)
    uint8_t  reason;          // Reason for score change
};
// Total size: 23 bytes
```

**Score Change Reasons:**

```cpp
enum class ScoreChangeReasons : uint8_t {
    SCORE_ENEMY_KILL        = 0x00,
    SCORE_BOSS_KILL         = 0x01,
    SCORE_POWERUP_COLLECTED = 0x02,
    SCORE_LEVEL_COMPLETED   = 0x03,
    SCORE_BONUS_SURVIVAL    = 0x04,
    SCORE_TOOK_DAMAGE       = 0x05
};
```

### POWERUP_PICKUP (0x43)

```cpp
struct PowerupPickup {
    PacketHeader header;      // type = 0x43, FLAG_RELIABLE
    uint32_t player_id;       // Player who picked up powerup
    uint32_t powerup_id;      // Powerup entity ID
    uint8_t  powerup_type;    // Type of powerup
    uint8_t  duration;        // Effect duration (seconds, 0=permanent)
};
// Total size: 22 bytes
```

**Powerup Types:**

```cpp
enum class PowerupTypes : uint8_t {
    POWERUP_SPEED_BOOST     = 0x00,
    POWERUP_WEAPON_UPGRADE  = 0x01,
    POWERUP_FORCE           = 0x02,  // R-Type signature
    POWERUP_SHIELD          = 0x03,
    POWERUP_EXTRA_LIFE      = 0x04,
    POWERUP_INVINCIBILITY   = 0x05
};
```

### WEAPON_FIRE (0x44)

```cpp
struct WeaponFire {
    PacketHeader header;      // type = 0x44
    uint32_t shooter_id;      // Entity that fired
    uint32_t projectile_id;   // New projectile entity ID
    int16_t  origin_x;        // Fire origin X
    int16_t  origin_y;        // Fire origin Y
    int16_t  direction_x;     // Direction vector X (normalized×1000)
    int16_t  direction_y;     // Direction vector Y (normalized×1000)
    uint8_t  weapon_type;     // Weapon type fired
};
// Total size: 29 bytes
```

---

## Game Control (0x60-0x6F)

### GAME_START (0x60)

```cpp
struct GameStart {
    PacketHeader header;        // type = 0x60, FLAG_RELIABLE
    uint32_t game_instance_id;  // Game instance identifier
    uint8_t  player_count;      // Number of players
    uint32_t player_ids[4];     // Player IDs (up to 4)
    uint8_t  level_id;          // Starting level
    uint8_t  difficulty;        // Difficulty setting
};
// Total size: 35 bytes
```

### GAME_END (0x61)

```cpp
struct GameEnd {
    PacketHeader header;        // type = 0x61, FLAG_RELIABLE
    uint8_t  end_reason;        // Reason game ended
    uint32_t final_scores[4];   // Final scores for all players
    uint8_t  winner_id;         // Winning player (if applicable)
    uint32_t play_time;         // Total game time (seconds)
};
// Total size: 34 bytes
```

### LEVEL_COMPLETE (0x62)

```cpp
struct LevelComplete {
    PacketHeader header;         // type = 0x62, FLAG_RELIABLE
    uint8_t  completed_level;    // Level that was completed
    uint8_t  next_level;         // Next level to load (0xFF=game end)
    uint32_t bonus_score;        // Completion bonus
    uint16_t completion_time;    // Time taken (seconds)
};
// Total size: 20 bytes
```

### LEVEL_START (0x63)

```cpp
struct LevelStart {
    PacketHeader header;          // type = 0x63, FLAG_RELIABLE
    uint8_t  level_id;            // Level identifier
    char     level_name[32];      // Level name
    uint16_t estimated_duration;  // Estimated time (seconds)
};
// Total size: 47 bytes
```

---

## Protocol Control (0x70-0x7F)

### ACKNOWLEDGMENT (0x70)

```cpp
struct Acknowledgment {
    PacketHeader header;            // type = 0x70
    uint32_t acked_sequence;        // Sequence number being ACKed
    uint32_t received_timestamp;    // When packet was received
    uint32_t client_id;             // Client identifier
};
// Total size: 24 bytes
```

### PING (0x71)

```cpp
struct Ping {
    PacketHeader header;            // type = 0x71
    uint32_t client_timestamp;      // Client's current timestamp
    uint32_t client_id;             // Client identifier
};
// Total size: 20 bytes
```

### PONG (0x72)

```cpp
struct Pong {
    PacketHeader header;            // type = 0x72
    uint32_t client_timestamp;      // Original client timestamp from PING
    uint32_t server_timestamp;      // Server's timestamp when received
};
// Total size: 20 bytes
```

---

## Effects System (0x50-0x5F)

### VISUAL_EFFECT (0x50)

```cpp
struct VisualEffect {
    PacketHeader header;      // type = 0x50
    uint8_t  effect_type;
    int16_t  pos_x;
    int16_t  pos_y;
    uint16_t duration_ms;     // Effect duration
    uint8_t  scale;           // Scale multiplier (100 = 1.0x)
    uint8_t  color_tint_r;    // Color modulation
    uint8_t  color_tint_g;
    uint8_t  color_tint_b;
};
// Size: 23 bytes
```

### AUDIO_EFFECT (0x51)

```cpp
struct AudioEffect {
    PacketHeader header;      // type = 0x51
    uint8_t  effect_type;
    int16_t  pos_x;           // 3D audio positioning
    int16_t  pos_y;
    uint8_t  volume;          // 0-255
    uint8_t  pitch;           // Pitch modifier (100 = normal)
};
// Size: 19 bytes
```

### PARTICLE_SPAWN (0x52)

```cpp
struct ParticleSpawn {
    PacketHeader header;            // type = 0x52
    uint16_t particle_system_id;    // Particle system type
    int16_t  pos_x;
    int16_t  pos_y;
    int16_t  velocity_x;            // Initial velocity
    int16_t  velocity_y;
    uint16_t particle_count;        // Number of particles
    uint16_t lifetime_ms;           // Particle lifetime
    uint8_t  color_start_r;         // Start color
    uint8_t  color_start_g;
    uint8_t  color_start_b;
    uint8_t  color_end_r;           // End color (for fade)
    uint8_t  color_end_g;
    uint8_t  color_end_b;
};
// Size: 32 bytes
```

---

## R-Type Specific Systems (0x64-0x6F)

### FORCE_STATE (0x64)

Updates the state of the Force (R-Type's signature weapon).

```cpp
struct ForceState {
    PacketHeader header;            // type = 0x64
    uint32_t force_entity_id;       // The Force entity
    uint32_t parent_ship_id;        // Ship it's attached to (0=detached)
    uint8_t  attachment_point;      // Where it's attached
    uint8_t  power_level;           // 1-5
    uint8_t  charge_percentage;     // 0-100 for charge beam
    uint8_t  is_firing;             // Boolean flag
};
// Size: 24 bytes
```

**Attachment Points:**

```cpp
enum class ForceAttachmentPoint : uint8_t {
    FORCE_DETACHED = 0x00,
    FORCE_FRONT    = 0x01,
    FORCE_BACK     = 0x02,
    FORCE_ORBITING = 0x03  // For special modes
};
```

### AI_STATE (0x65)

```cpp
struct AIState {
    PacketHeader header;            // type = 0x65
    uint32_t entity_id;             // AI entity
    uint8_t  current_state;         // Current AI state
    uint8_t  behavior_type;         // Behavior pattern
    uint32_t target_entity_id;      // Current target (0=no target)
    int16_t  waypoint_x;            // Next waypoint or target pos
    int16_t  waypoint_y;
    uint16_t state_timer;           // Time remaining in current state
};
// Size: 30 bytes
```

**AI Behavior Types:**

```cpp
enum class AIBehaviorType : uint8_t {
    AI_IDLE              = 0x00,
    AI_PATROL            = 0x01,
    AI_CHASE             = 0x02,
    AI_FLEE              = 0x03,
    AI_ATTACK_PATTERN_1  = 0x04,  // Straight line
    AI_ATTACK_PATTERN_2  = 0x05,  // Sine wave
    AI_ATTACK_PATTERN_3  = 0x06,  // Circle
    AI_BOSS_PHASE_1      = 0x07,
    AI_BOSS_PHASE_2      = 0x08,
    AI_BOSS_PHASE_3      = 0x09,
    AI_KAMIKAZE          = 0x0A
};
```

---

## Implementation Notes

### Code References (source of truth)
- Packet enum and structs: [src/common/include/common/protocol/Protocol.hpp](src/common/include/common/protocol/Protocol.hpp)
- Validation/serialization handlers: [src/common/include/common/protocol/PacketManager.hpp](src/common/include/common/protocol/PacketManager.hpp) and [src/common/src/protocol/PacketManager.cpp](src/common/src/protocol/PacketManager.cpp)
- Runtime dispatch (client side): [src/game/src/coordinator/Coordinator.cpp](src/game/src/coordinator/Coordinator.cpp)
- Legacy minimalist enum (not used by the handler table): [src/common/include/common/protocol/PacketTypes.hpp](src/common/include/common/protocol/PacketTypes.hpp)

All packet types listed in this document are present in the handler table (see `handlers` array) of `PacketManager` and the Coordinator switch; no documented packet type is missing from the codebase as of this branch.

### Serialization Format

All multi-byte integers are transmitted in **network byte order (big-endian)**.

### Deserialization

When receiving component snapshots, the variable-length arrays are deserialized into `std::vector<std::pair<uint32_t, ComponentXXX>>` for easier processing:

```cpp
// Network format: header + world_tick + entity_count + [entity_id, ComponentTransform][]
// After deserialization: vector<pair<entity_id, ComponentTransform>>
```

### Reliability Mechanism

Packets marked with `FLAG_RELIABLE` MUST be acknowledged:

1. Receiver sends `ACKNOWLEDGMENT` packet
2. Sender retransmits if no ACK received within timeout
3. Timeout period: typically 100-500ms

### Packet Priorities

| Priority | Packet Types | Handling |
|----------|--------------|----------|
| High | Connection, Game Control, Reliable packets | Process immediately |
| Medium | Player Input, Game Events | Process in tick order |
| Low | Component Snapshots | Can drop old packets |

---

## Best Practices

### Client Implementation

1. **Interpolation**: Use Transform snapshots for smooth rendering between updates
2. **Prediction**: Predict movement based on Velocity components
3. **Reconciliation**: Correct predictions when authoritative state arrives
4. **Prioritization**: Process reliable packets first

### Server Implementation

1. **Tick Rate**: Run simulation at fixed rate (typically 60 Hz)
2. **Snapshot Frequency**: Adjust per component type (see table above)
3. **Delta Encoding**: Use delta snapshots when >70% entities unchanged
4. **Culling**: Only send snapshots for entities visible to player

### Bandwidth Optimization

| Technique | Savings | Complexity |
|-----------|---------|------------|
| Delta snapshots | 70-90% | Medium |
| Interest management | 50-80% | High |
| Component frequency tuning | 30-50% | Low |
| Quantization | 10-20% | Low |

---

## Troubleshooting

### Common Issues

**High Latency**
- Check for packet loss
- Verify server tick rate
- Consider reducing snapshot frequencies

**Desync Issues**
- Ensure reliable packets are acknowledged
- Check for dropped delta snapshots
- Implement full state resync mechanism

**Bandwidth Problems**
- Enable delta snapshots
- Reduce non-critical component update rates
- Implement interest management
