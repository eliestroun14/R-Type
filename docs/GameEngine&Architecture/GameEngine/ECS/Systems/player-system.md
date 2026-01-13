# PlayerSystem

The `PlayerSystem` translates high-level inputs (from `InputComponent`) into concrete game mechanics for player entities. It handles movement velocity and updates sprite animation states based on direction.

## Required Components

Entities processed by this system must have:
1.  **`Velocity`**: To apply movement speed.
2.  **`InputComponent`**: To read the player's current actions (Move Up, Down, Left, Right).
3.  **`Animation`** *(Optional)*: To update visual state based on movement direction.

## Logic & Algorithm

1.  **Velocity Application:**
    * Reads `activeActions` from the Input component.
    * Sets `vx` and `vy` based on `baseSpeed` (0.6f) multiplied by the global `ScaleFactor`.
    * **Note:** Supports diagonal movement by setting both `vx` and `vy` independently.

2.  **Animation State Machine:**
    * Detects vertical movement direction:
        * **UP:** Sets `currentDirection = 1`.
        * **DOWN:** Sets `currentDirection = 0`.
        * **NEUTRAL:** Sets `currentDirection = 2`.
    * **Change Detection:** Only updates animation if the direction has changed since the last frame (`_lastDirection`).
    * **Frame Mapping:**
        * **DOWN:** Frames 0 to 1.
        * **UP:** Frames 3 to 4.
        * **NEUTRAL:** Frames 2 to 2 (Static).
    * **One-Shot:** Sets `anim.loop = false` because R-Type animations (tilting ship) are transitions, not cycles.

### Code reference
[src/game/src/systems/PlayerSystem.cpp](src/game/src/systems/PlayerSystem.cpp#L1-L74)

```cpp
void PlayerSystem::onUpdate(float dt) {
    auto& velocities = _engine.getComponents<Velocity>();
    auto& animations = _engine.getComponents<Animation>();
    auto& inputs = _engine.getComponents<InputComponent>();
    float scaleFactor = _engine.getScaleFactor();
    for (size_t e : _entities) {
        if (!velocities[e] || !inputs[e]) continue;
        auto& vel = velocities[e].value();
        auto& input = inputs[e].value();
        float baseSpeed = 0.6f;
        float vx = (input.activeActions[GameAction::MOVE_RIGHT] - input.activeActions[GameAction::MOVE_LEFT]) * baseSpeed * scaleFactor;
        float vy = (input.activeActions[GameAction::MOVE_DOWN] - input.activeActions[GameAction::MOVE_UP]) * baseSpeed * scaleFactor;
        vel.vx = vx; vel.vy = vy;
        if (animations[e]) { /* direction-to-frames remap, non-looping */ }
    }
}
```