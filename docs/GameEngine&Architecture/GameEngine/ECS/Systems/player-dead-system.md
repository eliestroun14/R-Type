# PlayerDeadSystem

The `PlayerDeadSystem` manages the visual and logical consequences when a player character dies. It handles animation transitions, state management, and eventual entity removal.

## Required Components

Entities processed by this system must have:
1. **`DeadPlayer`**: Marks the entity as dead and tracks the death timer.
2. **`Transform`**: Current position in the world.
3. **`Sprite`** *(Optional)*: For visual feedback (fade-out, death animation).

## Logic & Algorithm

1. **Initialization (First Frame):**
   - When an entity gains the `DeadPlayer` component, the system marks it as `initialized = false`.
   - On first update, it performs one-time setup:
     - Hides the player sprite or triggers death animation
     - Disables input controls
     - Records the killer ID (if available)

2. **Timer Management:**
   - Increments `DeadPlayer.timer` by `dt` each frame.
   - Waits for a configurable respawn delay (e.g., 3 seconds).

3. **State Transition:**
   - Once the timer exceeds the respawn delay:
     - Removes the `DeadPlayer` component
     - Entity is ready for respawn or is destroyed

## Usage

```cpp
// Example: Mark a player as dead
gameEngine.addComponent(playerEntity, DeadPlayer());
gameEngine.getComponent<DeadPlayer>(playerEntity)->killerId = enemyId;
```

### Code reference
[src/game/src/systems/PlayerDeadSystem.cpp](src/game/src/systems/PlayerDeadSystem.cpp)

## Related Components

- [`DeadPlayer`](../Components/components.md#deadplayer) - Death marker component
- [`InputComponent`](../Components/components.md#inputcomponent) - Input handling (disabled on death)
