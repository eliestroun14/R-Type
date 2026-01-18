// ScoreSystem.hpp

#ifndef SCORESYSTEM_HPP_
#define SCORESYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/GameEngine.hpp>
#include <engine/ecs/component/Components.hpp>
#include <cstdint>

class ScoreSystem : public System {
public:
    explicit ScoreSystem(gameEngine::GameEngine& e) : _engine(e) {}

    void setHudEntity(Entity e) { _hud = e; }
    void pushEvent(uint32_t amount) { _engine.scoreEvents().push_back({amount}); }

    void onUpdate(float) override;

private:
    gameEngine::GameEngine& _engine;
    Entity _hud = Entity::fromId(0);
};

#endif // SCORESYSTEM_HPP_
