/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ButtonSystem
*/

#ifndef BUTTONSYSTEM_HPP_
#define BUTTONSYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

class ButtonSystem : public System {
    public:
        ButtonSystem(gameEngine::GameEngine& engine, bool isServer = false)
            : _engine(engine), _isServer(isServer)
        {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
        bool _isServer;

};

#endif /* !BUTTONSYSTEM_HPP_ */
