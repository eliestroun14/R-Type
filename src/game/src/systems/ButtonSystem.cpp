/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ButtonSystem
*/

#include <game/systems/ButtonSystem.hpp>

void ButtonSystem::onUpdate(float deltaTime) {
    auto& buttons = this->_engine.getComponents<ButtonComponent>();
    auto& sprites = this->_engine.getComponents<Sprite>();
    sf::Vector2i mousePos = this->_engine.getMousePosition();

    // get state only one time before the loop
    bool clickHeld = this->_engine.isActionActive(GameAction::LEFT_CLICK);
    bool clickReleased = this->_engine.isActionJustReleased(GameAction::LEFT_CLICK);

    for (size_t e = 0; e < buttons.size(); e++) {
        if (!buttons[e] || !sprites[e]) continue;

        auto& button = buttons[e].value();
        auto& sprite = sprites[e].value();

        bool isHover = sprite.globalBounds.contains(static_cast<float>(mousePos.x),
                                                   static_cast<float>(mousePos.y));
        ButtonState newState = ButtonState::NONE;

        if (isHover) {
            if (clickReleased && button.state == ButtonState::CLICKED) {
                button.onClick();
                newState = ButtonState::HOVER;
            } else if (clickHeld)
                newState = ButtonState::CLICKED;
            else
                newState = ButtonState::HOVER;
        } else
            newState = ButtonState::NONE;

        if (newState != button.state) {
            button.state = newState;
            if (newState == ButtonState::NONE)
                sprite.assetId = button.textures.none;
            if (newState == ButtonState::HOVER)
                sprite.assetId = button.textures.hover;
            if (newState == ButtonState::CLICKED)
                sprite.assetId = button.textures.clicked;
        }
    }
}
