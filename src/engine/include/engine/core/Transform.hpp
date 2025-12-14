#pragma once

namespace engine {
namespace core {

struct Vector2D {
    float x;
    float y;

    Vector2D(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct Transform {
    Vector2D position;
    float rotation;
    Vector2D scale;

    Transform()
        : position(0.0f, 0.0f)
        , rotation(0.0f)
        , scale(1.0f, 1.0f)
    {}
};

} // namespace core
} // namespace engine
