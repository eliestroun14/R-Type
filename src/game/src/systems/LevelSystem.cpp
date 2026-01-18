/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** LevelSystem
*/

#include <game/systems/LevelSystem.hpp>
#include <game/coordinator/Coordinator.hpp>
#include <common/constants/render/Assets.hpp>
#include <common/constants/defines.hpp>

void LevelSystem::onUpdate(float dt)
{
    auto& levels = this->_engine.getComponents<Level>();

    LOG_DEBUG_CAT("LevelSystem", "onUpdate called with dt={}, processing {} level entities", dt, this->_entities.size());

    for (size_t e : this->_entities) {
        if (!levels[e]) {
            LOG_WARN_CAT("LevelSystem", "Entity {} has no Level component", e);
            continue;
        }

        auto& level = levels[e].value();

        LOG_DEBUG_CAT("LevelSystem", "Level entity {}: started={}, completed={}, elapsedTime={}s", 
            e, level.started, level.completed, level.elapsedTime);

        // Only process if level has started and is not completed
        if (!level.started || level.completed)
            continue;

        level.elapsedTime += dt;

        LOG_DEBUG_CAT("LevelSystem", "Level entity {} after dt: elapsedTime={}s", e, level.elapsedTime);

        // Check if level duration exceeded (if duration is set and > 0)
        if (level.levelDuration > 0.f && level.elapsedTime >= level.levelDuration) {
            level.completed = true;
            std::cout << "Level Completed - Duration limit reached (" << level.levelDuration << "s)" << std::endl;
            continue;
        }

        // init spawn tracking
        if (this->_spawnedEnemies.size() != level.waves.size()) {
            this->_spawnedEnemies.resize(level.waves.size());
            for (size_t i = 0; i < level.waves.size(); i++)
                _spawnedEnemies[i].resize(level.waves[i].enemies.size(), false);
        }

        // process each waves of the level hereeeeeeee
        for (size_t waveIdx = 0; waveIdx < level.waves.size(); waveIdx++) {
            const Wave &wave = level.waves[waveIdx];

            // to know if it's the time to process a wave
            if (level.elapsedTime < wave.startTime)
                continue;

            float timeInWave = level.elapsedTime - wave.startTime;
            float cumulativeDelay = 0.f;

            LOG_DEBUG_CAT("LevelSystem", "Processing wave {} with {} enemies, timeInWave={}s", 
                waveIdx, wave.enemies.size(), timeInWave);

            for (size_t enemyIdx = 0; enemyIdx < wave.enemies.size(); enemyIdx++) {
                const EnemySpawn &enemy = wave.enemies[enemyIdx];
                cumulativeDelay += enemy.delayAfterPrevious;

                LOG_DEBUG_CAT("LevelSystem", "  Enemy {}: delayAfterPrevious={}, cumulativeDelay={}, timeInWave={}, spawned={}",
                    enemyIdx, enemy.delayAfterPrevious, cumulativeDelay, timeInWave, 
                    this->_spawnedEnemies[waveIdx][enemyIdx]);

                // check if its time to spawn the enenmy and it hasn't been spawned yet
                if (timeInWave >= cumulativeDelay && !this->_spawnedEnemies[waveIdx][enemyIdx]) {
                    spawnEnemy(enemy);
                    LOG_DEBUG_CAT("LevelSystem", "Spawned enemy of type {} at ({}, {}) from wave {} after {}s",
                        static_cast<int>(enemy.type), enemy.spawnX, enemy.spawnY, waveIdx, cumulativeDelay);
                    this->_spawnedEnemies[waveIdx][enemyIdx] = true;
                }
            }
        }


        // check level is completed (all waves finished spawning)
        if (!level.completed && level.currentWaveIndex >= static_cast<int>(level.waves.size()) - 1) {
            const Wave &lastWave = level.waves.back();
            float lastWaveEndTime = lastWave.startTime;

            for (const auto& spawn : lastWave.enemies)
                lastWaveEndTime += spawn.delayAfterPrevious;

            if (level.elapsedTime >= lastWaveEndTime) {
                // Check if duration-based completion applies, or just wave completion
                if (level.levelDuration == 0.f) {
                    // No duration limit, level completes when all waves spawn
                    level.completed = true;
                    std::cout << "Level Completed - All waves finished!" << std::endl;
                }
                // Otherwise, duration check above will handle completion
            }
        }
    }
}

void LevelSystem::spawnEnemy(const EnemySpawn& spawn)
{
    Entity entity = createEnemyByType(spawn.type, spawn.spawnX, spawn.spawnY);
    std::cout << "Spawned enemy at (" << spawn.spawnX << ", " << spawn.spawnY << ")" << std::endl << std::endl;
}

Entity LevelSystem::createEnemyByType(EnemyType type, float x, float y)
{
    // if the debug name is not enough precise, add the position of the entity from parameters
    std::string enemyName;
    switch (type)
    {
    case EnemyType::BASIC:
        enemyName = "Basic Enemy";
        break;

    case EnemyType::FAST:
        enemyName = "Fast Enemy";
        break;

    case EnemyType::TANK :
        enemyName = "Tank Enemy";
        break;

    case EnemyType::BOSS :
        enemyName = "Boss Enemy";
        break;

    default:
        enemyName = "Unknown Enemy";
        break;
    }

    // Get next NETWORKED entity ID (enemies must be synchronized over network)
    uint32_t enemyId = this->_engine.getNextNetworkedEntityId();

    // Determine enemy stats based on type
    uint16_t health = BASE_ENEMY_HEALTH_START;
    float velX = BASE_ENEMY_VELOCITY_X;
    float velY = BASE_ENEMY_VELOCITY_Y;

    switch (type)
    {
    case EnemyType::BASIC:
        health = BASE_ENEMY_HEALTH_START;
        velX = BASE_ENEMY_VELOCITY_X;
        velY = BASE_ENEMY_VELOCITY_Y;
        break;

        case EnemyType::FAST:
        health = FAST_ENEMY_HEALTH;
        velX = FAST_ENEMY_VELOCITY_X;
        velY = FAST_ENEMY_VELOCITY_Y;
        break;

        case EnemyType::TANK:
        health = TANK_ENEMY_HEALTH;
        velX = TANK_ENEMY_VELOCITY_X;
        velY = TANK_ENEMY_VELOCITY_Y;
        break;

        case EnemyType::BOSS:
        // TODO: Implement boss stats
        break;

        default:
        break;
    }

    // Determine scale based on enemy type
    float enemyScale = 10.0f;  // Default scale
    switch (type)
    {
    case EnemyType::FAST:
        enemyScale = FAST_ENEMY_SCALE;
        break;
    case EnemyType::TANK:
        enemyScale = TANK_ENEMY_SCALE;
        break;
    default:
        enemyScale = 10.0f;
        break;
    }

    // Use Coordinator to create enemy with proper network ID
    Entity enemy = this->_coordinator->createEnemyEntity(
        enemyId,
        x, y,
        velX, velY,
        health,
        true  // withRenderComponents - assume server has render for now
    );

    // Update enemy scale after creation
    auto& transforms = _engine.getComponents<Transform>();
    if (transforms[enemy]) {
        transforms[enemy]->scale = enemyScale;
    }

    // Add enemy-specific components based on type
    switch (type)
    {
    case EnemyType::BASIC:
        _engine.addComponent<Weapon>(enemy, Weapon(BASE_ENEMY_WEAPON_FIRE_RATE, 0, BASE_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
        _engine.addComponent<AI>(enemy, AI(AiBehaviour::SHOOTER_TACTIC, 50.f, 50.f));
        break;

        case EnemyType::FAST:
        _engine.addComponent<Weapon>(enemy, Weapon(FAST_ENEMY_WEAPON_FIRE_RATE, 0, FAST_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
        _engine.addComponent<AI>(enemy, AI(AiBehaviour::KAMIKAZE, 50.f, 50.f));
        break;

        case EnemyType::TANK:
        _engine.addComponent<Weapon>(enemy, Weapon(TANK_ENEMY_WEAPON_FIRE_RATE, 0, TANK_ENEMY_WEAPON_DAMAGE, ProjectileType::MISSILE));
        _engine.addComponent<AI>(enemy, AI(AiBehaviour::SHOOTER_TACTIC, 50.f, 50.f));
        break;

    case EnemyType::BOSS: {
        // TODO: Implement boss
        break;
    }

    default:
        break;
    }

    return enemy;
}


