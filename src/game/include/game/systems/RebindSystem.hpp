/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** RebindSystem
*/

#ifndef REBINDSYSTEM_HPP_
#define REBINDSYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/ecs/entity/EntityManager.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/GameEngine.hpp>

class RebindSystem : public System {
public:
    RebindSystem(gameEngine::GameEngine& engine)
        : _engine(engine)
    {}

    void onCreate() override {}

    void onUpdate(float dt) override;

    void applyNewKeybind(GameConfig& config, sf::Keyboard::Key key, GameAction action);

private:
    gameEngine::GameEngine& _engine;
};

#endif /* !REBINDSYSTEM_HPP_ */
