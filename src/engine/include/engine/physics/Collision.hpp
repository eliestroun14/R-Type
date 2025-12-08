#pragma once

#include "../core/Transform.hpp"

namespace engine {
namespace physics {

struct AABB {
    float x, y;
    float width, height;
};

struct Circle {
    float x, y;
    float radius;
};

class Collision {
public:
    static bool checkAABB(const AABB& a, const AABB& b);
    static bool checkCircle(const Circle& a, const Circle& b);
    static bool checkAABBCircle(const AABB& box, const Circle& circle);
};

} // namespace physics
} // namespace engine
