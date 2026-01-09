# ECS Components Reference

In our Entity Component System (ECS), **Components** are pure data structures. They contain **no logic**. Their sole purpose is to hold the state of an Entity, which is then processed by specific **Systems**.

## Core Components

Essential components for existence in the game world and network synchronization.

### `Transform`
Represents the spatial state of an entity.
* **Used by**: `RenderSystem`, `PhysicsSystem`, `CollisionSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **x** | World position on X axis. |
| `float` | **y** | World position on Y axis. |
| `float` | **rotation** | Rotation angle in degrees. |
| `float` | **scale** | Scale factor (1.0 = normal size). |

### `Velocity`
Defines the movement vector.
* **Used by**: `PhysicsSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **vx** | Velocity vector on X axis. |
| `float` | **vy** | Velocity vector on Y axis. |

### `NetworkId`
Unique identifier used to synchronize entities between Server and Client.
* **Used by**: `NetworkSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `uint32_t` | **id** | The Server-authoritative ID. |
| `bool` | **isLocal** | `true` if this entity is controlled by the local client (prediction). |

---

## Physics Components

### `HitBox`
Defines a rectangular area for collision detection.
* **Used by**: `CollisionSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **width** | Width of the box. |
| `float` | **height** | Height of the box. |
| `bool` | **isTrigger** | If `true`, detects overlap but does not block movement (e.g., Powerups). |

---

## Render Components

### `Sprite`
Defines the visual representation using an asset ID.
* **Used by**: `RenderSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `Assets` | **assetId** | Enum ID of the texture resource. |
| `int` | **zIndex** | Render layer (0: Background, 1: Game, 2: UI). |
| `sf::Rect` | **rect** | Sub-rectangle of the texture to display. |

### `Animation`
Manages sprite sheet animation state.
* **Used by**: `AnimationSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `size_t` | **numFrames** | Total frames in the animation. |
| `size_t` | **frameSpeed** | Delay between frames. |
| `size_t` | **currentFrame** | Current frame index. |
| `uint32_t` | **lastFrameTime** | Timestamp of last update. |

### `Text`
Wrapper for displaying strings.
* **Used by**: `RenderSystem` (UI).

| Type | Name | Description |
| :--- | :--- | :--- |
| `string` | **data** | The text content to display. |

---

## Logic & Gameplay Components

### `Health`
Manages hit points.
* **Used by**: `DamageSystem`, `UISystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `int` | **currentHealth** | Current HP value. |
| `int` | **maxHp** | Maximum HP capacity. |

### `Weapon`
Defines shooting capabilities.
* **Used by**: `WeaponSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `uint32_t` | **fireRateMs** | Minimum delay between shots (ms). |
| `uint32_t` | **lastShotTime** | Timestamp of last shot. |
| `int` | **damage** | Damage dealt per projectile. |
| `ProjectileType` | **projectileType** | Type of entity to spawn (Missile, Laser...). |

### `Powerup`
Represents a collectible item effect.
* **Used by**: `CollisionSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `PowerupType` | **powerupType** | Effect type (Shield, Speed, etc.). |
| `float` | **duration** | Duration of effect in seconds. |

---

## Tag Components

These components contain little to no data and act as **flags** or **labels** for Systems.

| Component | Description |
| :--- | :--- |
| **`Drawable`** | Marks the entity as eligible for rendering. |
| **`Playable`** | Marks the entity as the local player character (Input target). |
| **`Clickable`** | Contains `isClicked` state. Used for UI interaction. |

---

## Behaviour Components (AI)

### `MovementPattern`
Defines algorithmic movement (Server-side).
* **Used by**: `MovementSystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `PatternType` | **patternType** | LINEAR, SINE_WAVE, CIRCULAR, etc. |
| `float` | **amplitude** | Wave height / Circle radius. |
| `float` | **frequency** | Wave speed / Rotation speed. |
| `vector` | **positions** | Waypoints for complex paths. |

### `AI`
Controls high-level decision making.
* **Used by**: `AISystem`.

| Type | Name | Description |
| :--- | :--- | :--- |
| `AiBehaviour` | **aiBehaviour** | KAMIKAZE, SHOOTER, ZIGZAG, etc. |
| `float` | **detectionRange** | Range to start engaging. |
| `float` | **aggroRange** | Range to maintain/attack. |