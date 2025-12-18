/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** LevelSystem
*/

#ifndef LEVELSYSTEM_HPP_
#define LEVELSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>
#include <engine/ecs/component/Components.hpp>
#include <vector>

class LevelSystem : public System{
    public:
        LevelSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;

        std::vector<std::vector<bool>> _spawnedEnemies;

        void spawnEnemy(const EnemySpawn& spawn);
        Entity createEnemyByType(EnemyType type, float x, float y);
};

#endif /* !LEVELSYSTEM_HPP_ */
