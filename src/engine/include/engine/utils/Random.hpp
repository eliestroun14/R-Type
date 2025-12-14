#pragma once

#include <random>

namespace engine {
namespace utils {

class Random {
public:
    static void init();

    static int range(int min, int max);
    static float range(float min, float max);
    static bool boolean(float probability = 0.5f);

private:
    static std::mt19937 s_generator;
};

} // namespace utils
} // namespace engine
