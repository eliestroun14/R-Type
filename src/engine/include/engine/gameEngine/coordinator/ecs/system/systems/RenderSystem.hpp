/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderSystem
*/

#ifndef RENDERSYSTEM_HPP_
#define RENDERSYSTEM_HPP_

#include <engine/gameEngine/coordinator/Coordinator.hpp>
#include <engine/gameEngine/coordinator/ecs/system/System.hpp>

class RenderSystem : public System {
    public:
        RenderSystem(Coordinator& coordinator) : _coordinator(coordinator) {}

        void onCreate() override {}

        void onUpdate(float dt) override;

    private:
        Coordinator& _coordinator;
        std::vector<size_t> _sortedEntities;
};

#endif /* !RENDERSYSTEM_HPP_ */
