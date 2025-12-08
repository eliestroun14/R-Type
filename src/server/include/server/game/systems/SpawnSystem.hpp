#pragma once

namespace server {
namespace game {
namespace systems {

class SpawnSystem {
public:
    void update(float deltaTime);

    void spawnEnemy(float x, float y, uint8_t enemyType);
    void spawnPowerup(float x, float y, uint8_t powerupType);

private:
    void updateSpawnTimers(float deltaTime);
    void handleWaveProgression();

    float m_enemySpawnTimer = 0.0f;
    uint32_t m_currentWave = 1;
};

} // namespace systems
} // namespace game
} // namespace server
