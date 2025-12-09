/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** GameEngine
*/

#ifndef GAMEENGINE_HPP_
#define GAMEENGINE_HPP_

#include "src/engine/include/engine/ecs/Coordinator.hpp"
#include "src/server/include/server/network/NetworkType.hpp"

class GameEngine {
    private:
        std::unique_ptr<Coordinator> _coordinator;

        void update(float dt) {}
        void processInput(NetworkType type) {}
        void render(NetworkType type) {}

    public:
        void init() {}
        void process(float dt, NetworkType type) {}
};

#endif /* !GAMEENGINE_HPP_ */
