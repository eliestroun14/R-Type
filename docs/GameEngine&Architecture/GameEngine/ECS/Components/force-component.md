# Force Component

The `Force` component represents the iconic "Force" weapon system from R-Type. It's a specialized attachment that orbits, docks, or extends from the player ship, providing advanced firing capabilities and gameplay mechanics.

## Fields

```cpp
struct Force {
    uint32_t parentShipId;                  // Ship this Force is attached to (0 = detached)
    ForceAttachmentPoint attachmentPoint;   // Current attachment location
    uint8_t powerLevel;                     // Power level (1-5)
    uint8_t chargePercentage;               // Beam charge state (0-100)
    bool isFiring;                          // Whether currently firing
};

enum class ForceAttachmentPoint : uint8_t {
    FORCE_DETACHED   = 0x00,                // Freely orbiting
    FORCE_FRONT      = 0x01,                // Docked at ship front
    FORCE_BACK       = 0x02,                // Docked at ship back
    FORCE_ORBITING   = 0x03                 // Orbiting the ship
};
```

## Field Descriptions

### parentShipId

- **Type**: `uint32_t`
- **Meaning**: 
  - `0` = Force is detached (freely moving, can be retrieved by player)
  - Non-zero = ID of the ship the Force is attached to
- **Usage**: Determines whether the Force moves independently or follows the parent ship

### attachmentPoint

- **Type**: `ForceAttachmentPoint`
- **Values**:
  - `FORCE_DETACHED (0x00)`: Force moves independently, orbits autonomously, player can collect it
  - `FORCE_FRONT (0x01)`: Docked at the front of the ship, fires forward
  - `FORCE_BACK (0x02)`: Docked at the back of the ship, fires backward
  - `FORCE_ORBITING (0x03)`: Orbits around the ship, fires in multiple directions
- **Gameplay Impact**: Determines firing patterns and positioning

### powerLevel

- **Type**: `uint8_t` (1-5)
- **Meaning**: Strength and damage of the Force beam
  - Level 1: Weak, rapid-fire laser
  - Level 5: Powerful charged beam
- **Upgrade Source**: Collected via `WEAPON_UPGRADE` powerups

### chargePercentage

- **Type**: `uint8_t` (0-100)
- **Meaning**: Charge state of the Force beam
  - `0-50%`: Charging
  - `50-100%`: Ready to fire maximum power
- **Mechanics**:
  - Increases when player holds fire button without shooting
  - Resets after firing
  - Higher percentage = more damage on discharge

### isFiring

- **Type**: `bool`
- **Meaning**: Whether the Force is currently shooting
- **Duration**: Set by `ShootSystem` and cleared after fire animation

## Gameplay Mechanics

### Attachment Lifecycle

1. **Detached**: Force spawns as separate entity when player dies or collects power-up
2. **Player Retrieval**: Player passes through detached Force to re-attach
3. **Active**: Force docks at specified position, inherits parent velocity
4. **Loss**: If player dies, Force becomes detached again

### Firing Patterns

- **FORCE_FRONT**: Direct forward beam
- **FORCE_BACK**: Backward beam (defensive)
- **FORCE_ORBITING**: 360° coverage (omnidirectional bursts)

### Charging System

```
Hold Fire → chargePercentage increases (0→100%)
          ↓
       50% threshold reached → Ready state
          ↓
       Release Fire → Discharge at current charge level
                    → Reset to 0%
```

## Used By

- **ShootSystem**: Manages firing logic and beam spawning
- **AnimationSystem**: Animates Force attachment/detachment
- **CollisionSystem**: Force-ship collision for attachment
- **RenderSystem**: Draws Force sprite at correct position relative to ship

## Example Usage

```cpp
// Create a newly acquired Force
Entity force = gameEngine.createEntity("Force");
gameEngine.addComponent(force, Transform(x, y, 0.f, 1.f));
gameEngine.addComponent(force, Sprite(Assets::FORCE, IS_GAME));

// Detached state - freely moving
gameEngine.addComponent(force, Force(
    0,                              // No parent (detached)
    ForceAttachmentPoint::FORCE_DETACHED,
    3,                              // Power level 3
    0,                              // Not charging
    false                           // Not firing
));

// Later: Player collects it
Force& forceComp = gameEngine.getComponent<Force>(force).value();
forceComp.parentShipId = playerId;
forceComp.attachmentPoint = ForceAttachmentPoint::FORCE_FRONT;
// Force now moves with player, docked at front
```

### Charging Example

```cpp
if (playerInputs[GameAction::SHOOT]) {
    Force& force = gameEngine.getComponent<Force>(forceEntity).value();
    
    // Increase charge while held
    if (force.chargePercentage < 100) {
        force.chargePercentage += 30 * dt; // 30% per second
    }
} else if (force.chargePercentage > 0) {
    // Fire at current charge
    spawnBeam(forceEntity, force.chargePercentage);
    force.chargePercentage = 0;
}
```

## R-Type Historical Context

In the original R-Type arcade game, the Force was the signature power-up:
- Enhanced defense when attached to the back
- Enhanced offense when attached to the front
- Could be detached to serve as a mobile shield
- Returning to collect it was a risk-reward mechanic

Our implementation preserves this core gameplay while adapting to networked multiplayer.

### Code reference

[src/engine/include/engine/ecs/component/Components.hpp](src/engine/include/engine/ecs/component/Components.hpp#L450-L475) - Component definition
