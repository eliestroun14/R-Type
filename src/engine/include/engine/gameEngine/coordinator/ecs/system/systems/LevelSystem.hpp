/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** LevelSystem
*/

#ifndef LEVELSYSTEM_HPP_
#define LEVELSYSTEM_HPP_

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <vector>

class LevelSystem : public System{
    public:
        LevelSystem(Coordinator& coord) : _coordinator(coord) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        Coordinator& _coordinator;

        std::vector<std::vector<bool>> _spawnedEnemies;

        void spawnEnemy(const EnemySpawn& spawn);
        Entity createEnemyByType(EnemyType type, float x, float y);
};

#endif /* !LEVELSYSTEM_HPP_ */
