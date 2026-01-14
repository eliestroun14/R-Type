# LifetimeSystem

The `LifetimeSystem` manages entity lifecycles based on a countdown timer. It automatically removes entities when their remaining time expires. This is useful for temporary effects, projectiles with limited lifespan, and timed events.

## Required Components

Entities processed by this system must have:
1. **`Lifetime`**: Contains the remaining time before entity removal.

## Logic & Algorithm

1. **Time Deduction:**
   - Subtracts `dt` (delta time) from `Lifetime.remainingTime` every frame.

2. **Expiration Check:**
   - If `remainingTime <= 0`:
     - Removes the entity from the world
     - Triggers cleanup (component removal, memory deallocation)

3. **No-op for Valid Entities:**
   - Entities with `remainingTime > 0` are unaffected.

## Usage

```cpp
// Example: Create a temporary visual effect that lasts 2 seconds
Entity effect = gameEngine.createEntity("ExplosionEffect");
gameEngine.addComponent(effect, Transform(x, y, 0.f, 1.f));
gameEngine.addComponent(effect, Sprite(Assets::EXPLOSION, IS_GAME));
gameEngine.addComponent(effect, Lifetime(2.0f)); // 2 seconds

// The entity will automatically be destroyed after 2 seconds
```

### Common Use Cases

- **Projectile Cleanup**: Remove bullets that go off-screen after N seconds
- **Visual Effects**: Explosions or particle effects with fixed duration
- **Temporary Powerups**: Invincibility shields that expire
- **Spawned Enemies**: Enemies that despawn after timeout (wave completion)

## Performance Considerations

- LifetimeSystem is one of the cheapest systems (only subtracts float and checks condition)
- Safe to attach Lifetime to many entities without performance impact
- Always prefer Lifetime over manual entity destruction logic

### Code reference
[src/game/src/systems/LifetimeSystem.cpp](src/game/src/systems/LifetimeSystem.cpp)

## Related Components

- [`Lifetime`](../Components/components.md#lifetime) - The countdown timer component
