#pragma once

#include "UIManager.hpp"

namespace client {
namespace ui {

class HUD {
public:
    HUD();

    void update(float deltaTime);
    void render();

    void setHealth(uint16_t current, uint16_t max);
    void setScore(uint32_t score);
    void setWeapon(uint8_t weaponType);
    void showMessage(const std::string& message, float duration);

private:
    uint16_t m_currentHealth = 100;
    uint16_t m_maxHealth = 100;
    uint32_t m_score = 0;
    uint8_t m_weaponType = 0;
    std::string m_message;
    float m_messageTimer = 0.0f;
};

} // namespace ui
} // namespace client
