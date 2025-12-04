#pragma once

#include <cmath>

namespace engine {
namespace core {

struct Vector2D;

class Math {
public:
    static constexpr float PI = 3.14159265358979323846f;

    // Vector operations
    static float distance(const Vector2D& a, const Vector2D& b);
    static float length(const Vector2D& v);
    static Vector2D normalize(const Vector2D& v);
    static float dot(const Vector2D& a, const Vector2D& b);

    // Utilities
    static float clamp(float value, float min, float max);
    static float lerp(float a, float b, float t);
};

} // namespace core
} // namespace engine
