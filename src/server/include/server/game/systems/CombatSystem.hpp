#pragma once

namespace server {
namespace game {
namespace systems {

class CombatSystem {
public:
    void update(float deltaTime);

private:
    void processWeaponFire();
    void updateProjectiles(float deltaTime);
    void checkProjectileHits();
    void applyDamage();
};

} // namespace systems
} // namespace game
} // namespace server
