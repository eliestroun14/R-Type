# LevelSystem

The `LevelSystem` is the **Game Master**. It manages the timeline of the level, spawning waves of enemies at precise intervals defined in the `Level` component. It also serves as an Entity Factory for enemies.

## Required Components

Entities processed by this system must have:
1.  **`Level`**: Stores the configuration of waves, enemy types, and timings.

## Logic & Algorithm

1.  **Time Tracking:**
    * Increments `level.elapsedTime` by `dt` every frame.

2.  **Wave Processing:**
    * Iterates through the list of `Wave` structs.
    * **Delay Calculation:** Computes the spawn time for an enemy by summing `wave.startTime` + `cumulativeDelay`.
    * **Spawning:** If `elapsedTime >= spawnTime` and the enemy hasn't spawned yet, it calls `createEnemyByType`.

3.  **Factory Logic (`createEnemyByType`):**
    * **BASIC:** Standard Kamikaze logic.
    * **FAST:** Higher velocity, lower HP.
    * **TANK:** High HP, slow velocity, larger sprite.
    * **BOSS:** *(Work in Progress)*.

4.  **Level Completion:**
    * Checks if the timer has passed the spawning time of the very last enemy in the last wave.
    * Sets `level.completed = true`.

## Timeline Visualization

```text
Time: 0s        5s              10s
      |         |               |
      [Wave 1 Start]            [Wave 2 Start]
      |-> Enemy A               |-> Enemy C
        |-> (+1s) Enemy B         |-> (+0.5s) Enemy D
```


## Usage

```c++
// Example: Configuring a simple level
Entity levelEntity = gameEngine->createEntity("Level1");

Level level;

// Wave 1: 3 basic enemies
Wave wave1;
wave1.startTime = 0.0f;  // Start after 0 seconds
wave1.enemies.push_back({EnemyType::BASIC, 800.f, 100.f, 0.f});
wave1.enemies.push_back({EnemyType::BASIC, 800.f, 200.f, 1.0f});  // 1 sec after previous
wave1.enemies.push_back({EnemyType::BASIC, 800.f, 300.f, 1.0f});  // 1 sec after previous

// Wave 2: Mix of enemies
Wave wave2;
wave2.startTime = 10000.0f;  // Start after 10 seconds
wave2.enemies.push_back({EnemyType::FAST, 800.f, 150.f, 0.f});
wave2.enemies.push_back({EnemyType::BASIC, 800.f, 250.f, 0.5f});
wave2.enemies.push_back({EnemyType::FAST, 800.f, 350.f, 0.5f});

// Wave 3: Tank
Wave wave3;
wave3.startTime = 10000.0f;
wave3.enemies.push_back({EnemyType::TANK, 800.f, 200.f, 0.f});


level.waves.push_back(wave1);
level.waves.push_back(wave2);
level.waves.push_back(wave3);

gameEngine->addComponent<Level>(levelEntity, level);
```

### Code reference
[src/game/src/systems/LevelSystem.cpp](src/game/src/systems/LevelSystem.cpp#L1-L140)

```cpp
void LevelSystem::onUpdate(float dt) {
    auto& levels = _engine.getComponents<Level>();
    for (size_t e : _entities) {
        if (!levels[e]) continue;
        auto& level = levels[e].value();
        if (level.completed) continue;
        level.elapsedTime += dt;
        if (_spawnedEnemies.size() != level.waves.size()) {
            _spawnedEnemies.resize(level.waves.size());
            for (size_t i = 0; i < level.waves.size(); i++)
                _spawnedEnemies[i].resize(level.waves[i].enemies.size(), false);
        }
        for (size_t waveIdx = 0; waveIdx < level.waves.size(); ++waveIdx) {
            const Wave& wave = level.waves[waveIdx];
            if (level.elapsedTime < wave.startTime) continue;
            float timeInWave = level.elapsedTime - wave.startTime;
            float cumulativeDelay = 0.f;
            for (size_t enemyIdx = 0; enemyIdx < wave.enemies.size(); ++enemyIdx) {
                const EnemySpawn& enemy = wave.enemies[enemyIdx];
                cumulativeDelay += enemy.delayAfterPrevious;
                if (timeInWave >= cumulativeDelay && !_spawnedEnemies[waveIdx][enemyIdx]) {
                    spawnEnemy(enemy);
                    _spawnedEnemies[waveIdx][enemyIdx] = true;
                }
            }
        }
        // completion check against last wave end time
    }
}
```
```

