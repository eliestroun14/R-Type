# All Components Reference

Quick reference for all ECS components organized by category. Click on any component name for full documentation.

## Core / Transform

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`Transform`](components.md#transform) | Position, rotation, scale in world space | MovementSystem, RenderSystem, CollisionSystem |
| [`Velocity`](components.md#velocity) | Movement direction and speed vector | MovementSystem, PlayerSystem |
| [`NetworkId`](components.md#networkid) | Link to server entity for synchronization | Network layer |

---

## Rendering

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`Sprite`](components.md#sprite) | Texture, asset ID, Z-ordering, collision bounds | RenderSystem, CollisionSystem, AnimationSystem |
| [`Animation`](components.md#animation) | Frame data, timing, looping for sprite sheets | AnimationSystem, RenderSystem |
| [`Text`](components.md#text) | String data for UI rendering | RenderSystem |
| [`ScrollingBackground`](components.md#scrollingbackground) | Infinite loop background configuration | BackgroundSystem, RenderSystem |
| [`VisualEffect`](components.md#visualeffect) | Temporary visual overlays (flashes, colors) | RenderSystem |

---

## Physics & Collision

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`HitBox`](components.md#hitbox) | **Tag**: Marks entity as collidable | CollisionSystem |
| *(Collision bounds stored in `Sprite.globalBounds`)* | AABB collision rectangles | CollisionSystem |

---

## Gameplay / Combat

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`Health`](components.md#health) | Current and max HP | CollisionSystem, DamageSystem |
| [`Weapon`](components.md#weapon) | Fire rate, damage, projectile type | ShootSystem |
| [`Projectile`](components.md#projectile) | Shooter ID, damage, friendly-fire flag | ShootSystem, CollisionSystem |
| [`Powerup`](components.md#powerup) | Collectible type and duration | CollisionSystem |
| [`Force`](force-component.md) | R-Type iconic force weapon | ShootSystem, RenderSystem, CollisionSystem |

---

## Audio

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`AudioSource`](audio-components.md#audiosource) | Playback configuration (looping, 3D, volume) | AudioSystem |
| [`AudioEffect`](audio-components.md#audioeffect) | Trigger audio event (SFX playback) | AudioSystem, CollisionSystem |

---

## Player & Input

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`InputComponent`](components.md#inputcomponent) | Player ID and active keyboard actions | PlayerSystem, ShootSystem, InputSystem |
| [`Playable`](components.md#playable) | **Tag**: Marks entity as player-controlled | AISystem (to find targets), PlayerSystem |
| [`DeadPlayer`](components.md#deadplayer) | Death state and respawn timer | PlayerDeadSystem |
| [`Score`](components.md#score) | Player score | UISystem, NetworkSync |

---

## Enemy & AI

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`Enemy`](components.md#enemy) | **Tag**: Marks entity as hostile NPC | CollisionSystem, AISystem |
| [`AI`](components.md#ai) | Behavior mode, detection range, aggro range | AISystem |
| [`MovementPattern`](components.md#movementpattern) | Complex movement algorithms (sine wave, circular) | MovementSystem |

---

## Level & Waves

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`Level`](components.md#level) | Wave configuration, spawn timeline | LevelSystem |
| [`Wave`](components.md#wave) | List of enemies to spawn with delays | LevelSystem |
| [`EnemySpawn`](components.md#enemyspawn) | Enemy type, position, spawn timing | LevelSystem |

---

## Lifecycle

| Component | Purpose | Used By |
|-----------|---------|---------|
| [`Lifetime`](components.md#lifetime) | Countdown timer for auto-removal | LifetimeSystem |

---

## Summary

- **Total Components**: 40+
- **Tag Components** (empty, presence-only): HitBox, Drawable, Playable, Enemy
- **Data Components**: All others
- **Network Sync**: NetworkId, Projectile, DeadPlayer
- **Audio System**: AudioSource, AudioEffect
- **R-Type Special**: Force

See [components.md](components.md) for complete type definitions.
