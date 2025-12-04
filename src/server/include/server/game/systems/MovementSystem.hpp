#pragma once

namespace server {
namespace game {
namespace systems {

class MovementSystem {
public:
    void update(float deltaTime);

private:
    void updateVelocities();
    void applyMovement(float deltaTime);
    void clampToBounds();
};

} // namespace systems
} // namespace game
} // namespace server
