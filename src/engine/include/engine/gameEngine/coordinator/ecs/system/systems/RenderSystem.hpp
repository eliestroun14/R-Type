/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderSystem
*/

#ifndef RENDERSYSTEM_HPP_
#define RENDERSYSTEM_HPP_

#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

const sf::Vector2f REFERENCE_RESOLUTION(1920.f, 1080.f);

class RenderSystem : public System {
    public:
        RenderSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        gameEngine::GameEngine& _engine;
        std::vector<size_t> _sortedEntities;
};

#endif /* !RENDERSYSTEM_HPP_ */
