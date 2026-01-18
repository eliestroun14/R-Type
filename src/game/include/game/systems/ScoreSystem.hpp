#ifndef SCORESYSTEM_HPP_
#define SCORESYSTEM_HPP_

#include <engine/ecs/system/System.hpp>
#include <engine/GameEngine.hpp>
#include <engine/ecs/component/Components.hpp>
#include <optional>

class ScoreSystem : public System {
public:
    explicit ScoreSystem(gameEngine::GameEngine& engine) : _engine(engine) {}

    void onStartRunning() override;
    void onUpdate(float dt) override;

private:
    gameEngine::GameEngine& _engine;
};

#endif // SCORESYSTEM_HPP_