#pragma once

#include <cstdint>

namespace sf {

struct Vector2f {
    float x;
    float y;

    constexpr Vector2f(float xx = 0.0f, float yy = 0.0f) : x(xx), y(yy) {}
};

struct Vector2i {
    int x;
    int y;

    constexpr Vector2i(int xx = 0, int yy = 0) : x(xx), y(yy) {}
};

struct IntRect {
    int left;
    int top;
    int width;
    int height;

    constexpr IntRect(int l = 0, int t = 0, int w = 0, int h = 0)
        : left(l), top(t), width(w), height(h) {}
};

struct FloatRect {
    float left;
    float top;
    float width;
    float height;

    constexpr FloatRect(float l = 0.0f, float t = 0.0f, float w = 0.0f, float h = 0.0f)
        : left(l), top(t), width(w), height(h) {}

    bool contains(float x, float y) const {
        return x >= left && x <= (left + width) && y >= top && y <= (top + height);
    }
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    constexpr Color(uint8_t rr = 0, uint8_t gg = 0, uint8_t bb = 0, uint8_t aa = 255)
        : r(rr), g(gg), b(bb), a(aa) {}

    static const Color White;

};

inline constexpr Color Color::White{255, 255, 255, 255};

} // namespace sf
