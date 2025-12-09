/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#ifndef RENDERMANAGER_HPP_
#define RENDERMANAGER_HPP_

#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define FRAMERATE_LIMIT 60

enum class GameAction {

    
};

class RenderManager {
    public:
        RenderManager();
        ~RenderManager();

        void init();

        void render();

        void processInput();

    private:
        sf::RenderWindow _window;
};

#endif /* !RENDERMANAGER_HPP_ */
