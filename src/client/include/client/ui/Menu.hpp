#pragma once

#include "UIManager.hpp"
#include <functional>

namespace client {
namespace ui {

enum class MenuState : uint8_t {
    MAIN_MENU,
    OPTIONS,
    CREDITS
};

class Menu {
public:
    Menu();

    void update(float deltaTime);
    void render();
    bool handleInput(float mouseX, float mouseY, bool clicked);

    void setOnPlay(std::function<void()> callback) { m_onPlay = callback; }
    void setOnOptions(std::function<void()> callback) { m_onOptions = callback; }
    void setOnQuit(std::function<void()> callback) { m_onQuit = callback; }

private:
    MenuState m_state;
    std::function<void()> m_onPlay;
    std::function<void()> m_onOptions;
    std::function<void()> m_onQuit;
};

} // namespace ui
} // namespace client
