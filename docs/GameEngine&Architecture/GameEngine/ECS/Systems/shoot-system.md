# ShootSystem

The `ShootSystem` manages weapon firing logic for both Players and AI. It handles the creation of projectile entities, enforces fire rates, and determines shooting direction.

## Required Components

Entities processed by this system must have:
1.  **`Weapon`**: Stores damage, fire rate, and projectile type.
2.  **`Transform`**: Origin point for the projectile.
3.  **`InputComponent`** *(Optional)*: If present, marks the entity as a Player (controlled by keys). If absent, treated as AI.

## Logic & Algorithm

1.  **Fire Trigger:**
    * **Player:** Checks `InputComponent` for `GameAction::SHOOT`.
    * **AI:** Uses a simple temporal pattern (e.g., shoot for 100ms every 2 seconds). *TODO: Integrate with AISystem.*

2.  **Fire Rate Check:**
    * Compares `currentTime` (ms) with `lastShotTime + fireRateMs`.
    * If valid, updates `lastShotTime` and proceeds to spawn.

3.  **Projectile Spawning:**
    * **Direction:**
        * **Player:** Always shoots Right `(1.0, 0.0)`.
        * **Enemy:** Always shoots Left `(-1.0, 0.0)`.
    * **Asset Selection:** Maps `Weapon.projectileType` to specific visual assets (e.g., `MISSILE` -> `DEFAULT_BULLET`).
    * **Component Creation:** Spawns a new Entity with:
        * `Velocity`: Calculated based on direction * speed (1.5 units/ms).
        * `Projectile`: Stores ownership (`shooterId`) and damage.
        * `HitBox`: Enables collision.

## Usage

Attach a `Weapon` component to any entity to give it shooting capabilities.

```cpp
// Example: Giving a weapon to a player
gameEngine.addComponent(player, Weapon(
    200,                // Fire Rate (ms)
    0,                  // Last Shot Time
    10,                 // Damage
    ProjectileType::MISSILE
));
```

### Code reference
[src/game/src/systems/ShootSystem.cpp](src/game/src/systems/ShootSystem.cpp#L1-L140)

```cpp
void ShootSystem::onUpdate(float dt) {
    uint32_t now = ms_since_epoch();
    auto& weapons = _engine.getComponents<Weapon>();
    auto& transforms = _engine.getComponents<Transform>();
    auto& inputs = _engine.getComponents<InputComponent>();
    for (size_t e : _entities) {
        if (!weapons[e] || !transforms[e]) continue;
        auto& weapon = weapons[e].value();
        auto& transform = transforms[e].value();
        bool isPlayer = inputs[e].has_value();
        bool shouldShoot = isPlayer ? inputs[e]->activeActions[GameAction::SHOOT]
                                    : (now % 2000) < 100;
        if (shouldShoot && canShoot(weapon, now)) {
            auto [dx, dy] = calculateDirection(Entity::fromId(e));
            weapon.lastShotTime = now;
            spawnProjectile(Entity::fromId(e), transform.x, transform.y, dx, dy, isPlayer, weapon);
        }
    }
}
```