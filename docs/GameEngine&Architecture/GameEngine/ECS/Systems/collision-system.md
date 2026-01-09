# CollisionSystem

The `CollisionSystem` handles the physical interactions between entities. It uses **AABB (Axis-Aligned Bounding Box)** logic to detect overlaps and resolves gameplay consequences such as damage application, projectile destruction, and entity death.

## Required Components

Entities processed by this system must have the following components:
1.  **`Transform`**: To verify the entity exists in the world.
2.  **`Sprite`**: Used for the actual collision bounds (`globalBounds`).
3.  **`HitBox`**: Acts as a tag to mark the entity as collidable.
4.  **`Health`** *(Optional)*: Required to take damage.
5.  **`Projectile`** *(Optional)*: Identifies the entity as a bullet/missile.
6.  **`InputComponent`** *(Optional)*: Used to distinguish Players from AI.

## Logic & Algorithm

The system checks every collidable entity against every other.

1.  **Detection:**
    * Uses SFML's `FloatRect::intersects()` on the `Sprite` components to detect AABB overlap.

2.  **Projectile Rules (Friendly Fire):**
    * **Player Projectiles:** Can **not** damage entities with `InputComponent` (other players or self).
    * **Enemy Projectiles:** Can **only** damage entities with `InputComponent` (players).
    * **Resolution:** If a valid hit occurs, the projectile is destroyed immediately.

3.  **Damage Application:**
    * If collision is valid, `Health.currentHealth` is reduced.
    * **Death:** If HP $\le$ 0, the entity is "killed" by removing its core components (`Transform`, `Sprite`, `Health`, `HitBox`).

4.  **Physical Collision (Non-Projectile):**
    * If two solid bodies collide (e.g., Player vs Enemy Ship), a default collision damage (10) is applied to both sides.

## Diagram: AABB Check

```text
      [ Entity A ]
      +----------+
      |          |
      |      +---+------+
      |      |   |///////////|  <-- Overlap Detected
      +------+---+      |
             | Entity B |
             +----------+
```

## Usage

```c++
// Example: Creating a collidable entity
Entity e = gameEngine.createEntity("Asteroid");
gameEngine.addComponent(e, Transform(x, y, E_ROT, E_SCALE));
gameEngine.addComponent(e, Sprite(ASSET_ID, Z_INDEX, sf::IntRect(
    0,
    0,
    RECT_WIDTH,
    RECT_HEIGHT
    )));
gameEngine.addComponent(e, HitBox()); // Essential tag
gameEngine.addComponent(e, Health(CURRENT, MAX));
```
