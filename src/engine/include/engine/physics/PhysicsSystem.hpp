#pragma once

#include "../ecs/System.hpp"

namespace engine {
namespace physics {

class PhysicsSystem {
public:
    void update(float deltaTime);

    // Physics utilities
    void applyGravity(float gravity);
    void resolveCollisions();
};

} // namespace physics
} // namespace engine
