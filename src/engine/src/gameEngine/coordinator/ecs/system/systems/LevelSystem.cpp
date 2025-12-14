/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** LevelSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/LevelSystem.hpp>
#include <common/constants/render/Assets.hpp>

void LevelSystem::onUpdate(float dt)
{
    auto& levels = this->_coordinator.getComponents<Level>();

    for (size_t e : this->_entities) {
        if (!levels[e])
            continue;

        auto& level = levels[e].value();

        if (level.completed)
            continue;

        level.elapsedTime += dt;


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

            for (size_t enemyIdx = 0; enemyIdx < wave.enemies.size(); enemyIdx++) {
                const EnemySpawn &enemy = wave.enemies[enemyIdx];
                cumulativeDelay += enemy.delayAfterPrevious;

                // check if its time to spawn the enenmy and it hasn't been spawned yet
                if (timeInWave >= cumulativeDelay && !this->_spawnedEnemies[waveIdx][enemyIdx]) {
                    spawnEnemy(enemy);
                    this->_spawnedEnemies[waveIdx][enemyIdx] = true;
                }
            }
        }


        // check level in completed
        if (level.currentWaveIndex >= level.waves.size() - 1) {
            const Wave &lastWave = level.waves.back();
            float lastWaveEndTime = lastWave.startTime;

            for (const auto& spawn : lastWave.enemies)
                lastWaveEndTime += spawn.delayAfterPrevious;

            if (level.elapsedTime >= lastWaveEndTime) {
                level.completed = true;
                std::cout << "Level Completed gros BG va" << std::endl;
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

    case EnemyType::FAST:
        enemyName = "Fast Enemy";

    case EnemyType::TANK :
        enemyName = "Tank Enemy";

    case EnemyType::BOSS :
        enemyName = "Boss Enemy";

    default:
        break;
    }

    Entity enemy = this->_coordinator.createEntity(enemyName);

    switch (type)
    {
    case EnemyType::BASIC : {
        this->_coordinator.addComponent<Sprite>(enemy, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 36)));
        _coordinator.addComponent<Transform>(enemy, Transform(x, y, 0.f, 2.0f));
        _coordinator.addComponent<Health>(enemy, Health(50, 50));
        _coordinator.addComponent<HitBox>(enemy, HitBox());
        _coordinator.addComponent<Velocity>(enemy, Velocity(0.0f, 0.f));
        _coordinator.addComponent<Weapon>(enemy, Weapon(1000, 0, 8, ProjectileType::MISSILE));
        _coordinator.addComponent<AI>(enemy, AI(AiBehaviour::KAMIKAZE, 50.f, 50.f));
        break;
    }

    case EnemyType::FAST: {
        _coordinator.addComponent<Sprite>(enemy, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 36)));
        _coordinator.addComponent<Transform>(enemy, Transform(x, y, 0.f, 2.0f));
        _coordinator.addComponent<Health>(enemy, Health(30, 30));
        _coordinator.addComponent<HitBox>(enemy, HitBox());
        _coordinator.addComponent<Velocity>(enemy, Velocity(-0.3f, 0.f));  // Move left faster
        _coordinator.addComponent<Weapon>(enemy, Weapon(800, 0, 5, ProjectileType::MISSILE));
        _coordinator.addComponent<AI>(enemy, AI(AiBehaviour::KAMIKAZE, 50.f, 50.f));
        break;
    }

    case EnemyType::TANK: {
        _coordinator.addComponent<Sprite>(enemy, Sprite(BASE_ENEMY, ZIndex::IS_GAME, sf::IntRect(0, 0, 33, 36)));
        _coordinator.addComponent<Transform>(enemy, Transform(x, y, 0.f, 3.0f));  // Bigger
        _coordinator.addComponent<Health>(enemy, Health(150, 150));
        _coordinator.addComponent<HitBox>(enemy, HitBox());
        _coordinator.addComponent<Velocity>(enemy, Velocity(-0.05f, 0.f));  // Move left very slowly
        _coordinator.addComponent<Weapon>(enemy, Weapon(500, 0, 15, ProjectileType::MISSILE));
        _coordinator.addComponent<AI>(enemy, AI(AiBehaviour::KAMIKAZE, 50.f, 50.f));
        break;
    }

    case EnemyType::BOSS: {
        // TODO: Implement boss
        break;
    }

    default:
        break;
    }

    return enemy;
}


