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

### Code reference
[src/game/src/systems/CollisionSystem.cpp](src/game/src/systems/CollisionSystem.cpp#L1-L120)

```cpp
void CollisionSystem::onUpdate(float dt) {
    auto& transforms = _engine.getComponents<Transform>();
    auto& sprites = _engine.getComponents<Sprite>();
    auto& healths = _engine.getComponents<Health>();
    auto& hitboxes = _engine.getComponents<HitBox>();
    auto& projectiles = _engine.getComponents<Projectile>();
    auto& inputs = _engine.getComponents<InputComponent>();

    std::vector<size_t> entities(_entities.begin(), _entities.end());
    for (size_t i = 0; i < entities.size(); ++i) {
        size_t e1 = entities[i];
        if (!transforms[e1] || !sprites[e1] || !hitboxes[e1]) continue;
        for (size_t j = i + 1; j < entities.size(); ++j) {
            size_t e2 = entities[j];
            if (!transforms[e2] || !sprites[e2] || !hitboxes[e2]) continue;
            if (!checkAABBCollision(sprites[e1].value(), sprites[e2].value())) continue;

            bool e1Projectile = e1 < projectiles.size() && projectiles[e1].has_value();
            bool e2Projectile = e2 < projectiles.size() && projectiles[e2].has_value();
            auto canHit = [&](const Projectile& proj, size_t targetId) {
                bool targetIsPlayer = targetId < inputs.size() && inputs[targetId].has_value();
                bool shooterIsPlayer = static_cast<size_t>(proj.shooterId) < inputs.size() && inputs[proj.shooterId].has_value();
                if (shooterIsPlayer) return !targetIsPlayer; // no friendly fire
                return targetIsPlayer; // enemy bullets hit players only
            };
            // projectile vs entity
            // ... applyDamage/destroyProjectile helpers in code
        }
    }
}
```

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
