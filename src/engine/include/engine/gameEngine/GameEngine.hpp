/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** GameEngine
*/

#ifndef GAMEENGINE_HPP_
#define GAMEENGINE_HPP_

#include <SFML/Graphics.hpp>
#include <memory>
#include "../ecs/Coordinator.hpp"

class GameEngine {
    private:
        sf::RenderWindow _window;
        std::unique_ptr<Coordinator> _coordinator;
        // INetworkClient *_client;

        bool _isRunning;


        void processInput() {}

        void update(float dt) {}

    public:
        // void init(INetworkClient *networkImpl) {}

        void run() {}
};

#endif /* !GAMEENGINE_HPP_ */
