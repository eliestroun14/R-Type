#pragma once

namespace server {
namespace game {
namespace systems {

class CollisionSystem {
public:
    void update();

private:
    void checkPlayerEnemyCollisions();
    void checkProjectileCollisions();
    void checkPowerupCollisions();
    void resolveCollisions();
};

} // namespace systems
} // namespace game
} // namespace server
