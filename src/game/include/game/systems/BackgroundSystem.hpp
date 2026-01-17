/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** BackgroundSystem
*/

#ifndef BACKGROUNDSYSTEM_HPP_
#define BACKGROUNDSYSTEM_HPP_

#include <engine/GameEngine.hpp>
#include <engine/ecs/system/System.hpp>

class BackgroundSystem : public System {
    public:
        BackgroundSystem(gameEngine::GameEngine& engine, bool isServer = false) 
            : _engine(engine), _isServer(isServer) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
        bool _isServer;
};

#endif /* !BACKGROUNDSYSTEM_HPP_ */
