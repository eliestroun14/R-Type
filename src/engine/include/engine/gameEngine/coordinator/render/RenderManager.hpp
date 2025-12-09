/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#ifndef RENDERMANAGER_HPP_
#define RENDERMANAGER_HPP_

#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FRAMERATE_LIMIT 60

// can change on updates
enum class GameAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    SHOOT,
    SWITCH_WEAPON,
    USE_POWERUP,
    SPECIAL,
    OPTIONS,
    EXIT,
    UNKNOW
};

class RenderManager {
    public:
        RenderManager();
        ~RenderManager();

        void init();

        void render();

        void processInput();
        bool isActionActive(GameAction action) const;

        sf::Vector2i getMousePosition() const;

        bool isOpen() const;

        void handleEvent(const sf::Event& event);

    private:
        sf::RenderWindow _window;

        // To know which key do what;
        std::map<sf::Keyboard::Key, GameAction> _keyBindings;

        // Which action is pressed at the moment
        std::map<GameAction, bool> _activeActions;

        sf::Vector2i _mousePos;
};

#endif /* !RENDERMANAGER_HPP_ */
