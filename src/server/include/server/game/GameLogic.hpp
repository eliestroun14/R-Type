#pragma once

#include "GameWorld.hpp"

namespace server {
namespace game {

class GameLogic {
public:
    explicit GameLogic(GameWorld& world);

    void processPlayerInput(uint32_t playerId, uint8_t actions);
    void updateGame(float deltaTime);

    void spawnPlayer(uint32_t playerId);
    void removePlayer(uint32_t playerId);

private:
    void handleCollisions();
    void updateEnemies();
    void spawnEnemies();

    GameWorld& m_world;
    float m_enemySpawnTimer = 0.0f;
};

} // namespace game
} // namespace server
