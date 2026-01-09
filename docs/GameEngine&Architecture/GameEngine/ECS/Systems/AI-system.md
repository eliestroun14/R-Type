# AISystem

The `AISystem` is responsible for controlling the behavior of non-player entities (enemies). Currently, it implements a "Kamikaze" tracking behavior where entities actively seek and move towards the nearest player.

## Required Components

Entities processed by this system must have the following components:
1.  **`AI`**: Holds internal state and timers.
2.  **`Transform`**: To know where the enemy is.
3.  **`Velocity`**: To apply movement towards the player.

> **Note:** The system also queries for **`Playable`** entities to determine the target (The Player).

## Logic & Algorithm

The system runs on every frame (`onUpdate`) and performs the following steps:

1.  **Target Acquisition:**
    * Iterates through all entities to find the first entity possessing a `Playable` tag and a `Transform`.
    * *Current limitation:* Targets the first player found in the memory array.

2.  **Vector Calculation:**
    * Calculates the difference vector between the Enemy and the Player.
    * Computes the Inverse Square Root of the distance (`safeInvSqrt`) to normalize the vector efficiently.

3.  **Velocity Application:**
    * Applies a constant `KAMIKAZE_SPEED` along the normalized vector.
    * Adjusts the velocity using the global `ScaleFactor` to ensure consistent speed across different window resolutions.


## Usage

Simply attach an `AI` component to an entity to have it chase the player.

```cpp
Entity enemy = gameEngine.createEntity("Enemy");

gameEngine.addComponent<Transform>(enemy, Transform(x, y, ENEMY_ROT, ENEMY_SCALE));
gameEngine.addComponent<Velocity>(enemy, Velocity(0.f, 0.f));
gameEngine.addComponent<AI>(enemy, AI(AiBehaviour::KAMIKAZE, KAMIKAZE_DETECTION, KAMIKAZE_RANGE.f));
```


