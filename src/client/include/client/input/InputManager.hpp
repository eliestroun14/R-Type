#pragma once

#include <unordered_map>
#include <cstdint>

namespace client {
namespace input {

enum class Key : uint8_t {
    W, A, S, D,
    UP, DOWN, LEFT, RIGHT,
    SPACE, ENTER, ESCAPE,
    MOUSE_LEFT, MOUSE_RIGHT
};

enum class InputState : uint8_t {
    RELEASED = 0,
    PRESSED = 1,
    HELD = 2
};

class InputManager {
public:
    void update();
    
    bool isKeyPressed(Key key) const;
    bool isKeyHeld(Key key) const;
    bool isKeyReleased(Key key) const;

    void getMousePosition(float& x, float& y) const;
    void setMousePosition(float x, float y);

private:
    std::unordered_map<Key, InputState> m_keyStates;
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
};

} // namespace input
} // namespace client
