#pragma once

namespace engine {
namespace core {

class Time {
public:
    // Delta time management
    static float getDeltaTime();
    static void setDeltaTime(float dt);

    // Timer utilities
    static double getElapsedTime();
    static void reset();

private:
    static float s_deltaTime;
    static double s_elapsedTime;
};

} // namespace core
} // namespace engine
