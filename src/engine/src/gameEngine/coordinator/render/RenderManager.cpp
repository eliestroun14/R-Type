/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#include "../../../../include/engine/gameEngine/coordinator/render/RenderManager.hpp"

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
}

void RenderManager::init()
{
    this->_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "R-Type", sf::Style::Fullscreen);
    this->_window.setFramerateLimit(FRAMERATE_LIMIT);

    auto desktop = sf::VideoMode::getDesktopMode();

    // // to center position. Maybe not necessary if the window is in fullScreen
    // this->_window.setPosition(sf::Vector2i(
    //     desktop.width / 2 - this->_window.getSize().x / 2,
    //     desktop.height / 2 - this->_window.getSize().y / 2));

}

void RenderManager::render()
{
    this->_window.clear();
    this->_window.display();
}

void RenderManager::processInput()
{
    sf::Event event;

    if (event.type == sf::Event::Closed)
        this->_window.close();

    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code)
        {
        case sf::Keyboard::Left:
            /* code */
            break;

        case sf::Keyboard::Right:
            /* code */
            break;

        case sf::Keyboard::Up:
            /* code */
            break;

        case sf::Keyboard::Down:
            /* code */
            break;

        case sf::Keyboard::Space:
        /* code */
        break;

        default:
            break;
        }
    }
}
