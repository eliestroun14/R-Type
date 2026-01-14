# MovementSystem

The `MovementSystem` is the physics integrator. It applies velocity to position over time. It is the fundamental engine that makes things move in the game world.

## 📦 Required Components

Entities processed by this system must have:
1.  **`Transform`**: The current position $(x, y)$.
2.  **`Velocity`**: The direction and speed vector $(v_x, v_y)$.

## ⚙️ Logic & Algorithm

The system uses explicit Euler integration. It also applies a global `ScaleFactor` retrieved from the `GameEngine` to ensure movement speed remains consistent regardless of window resizing or resolution scaling.

### Formula

$$
P_{new} = P_{old} + (V \times dt \times S)
$$

### Code reference
[src/game/src/systems/MovementSystem.cpp](src/game/src/systems/MovementSystem.cpp#L1-L27)

```cpp
void MovementSystem::onUpdate(float dt) {
	auto& positions = _engine.getComponents<Transform>();
	auto& velocities = _engine.getComponents<Velocity>();
	float scaleFactor = _engine.getScaleFactor();
	for (size_t e : _entities) {
		if (!positions[e] || !velocities[e]) continue;
		auto& pos = positions[e].value();
		auto& vel = velocities[e].value();
		pos.x += vel.vx * scaleFactor * dt;
		pos.y += vel.vy * scaleFactor * dt;
	}
}
```

Where:
* $P$: Position (`Transform.x`, `Transform.y`)
* $V$: Velocity (`Velocity.vx`, `Velocity.vy`)
* $dt$: Delta Time (Time elapsed since last frame)
* $S$: Scale Factor (Global window scaling)

## Usage

Simply add `Velocity` to any entity with a `Transform`.

```cpp
// Example: Moving a bullet
Entity bullet = gameEngine.createEntity("Bullet");
gameEngine.addComponent(bullet, Transform(x, y, BULLET_ROT, BULLET_SCALE));
// Move 500 pixels/sec to the right
gameEngine.addComponent(bullet, Velocity(0.f, 0.f));
```