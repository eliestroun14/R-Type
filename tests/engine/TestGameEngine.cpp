#include <gtest/gtest.h>

#define private public
#include <engine/GameEngine.hpp>
#undef private

#include <engine/ecs/component/Components.hpp>
#include <engine/ecs/system/System.hpp>

namespace {

class DummySystem : public System {
public:
    int createCount = 0;
    int destroyCount = 0;
    int updateCount = 0;

    void onCreate() override { createCount++; }
    void onDestroy() override { destroyCount++; }
    void onUpdate(float) override { updateCount++; }
};

} // namespace

TEST(GameEngineCoverage, EntityIdUtilities)
{
    gameEngine::GameEngine engine;
    engine.init();

    Entity entity = engine.createEntityWithId(42, "fixed");
    EXPECT_EQ(static_cast<std::size_t>(entity), 42u);

    Entity fetched = engine.getEntityFromId(42);
    EXPECT_EQ(static_cast<std::size_t>(fetched), 42u);

    EXPECT_EQ(engine.getNextEntityId(), 43u);
}

TEST(GameEngineCoverage, ComponentContainers)
{
    gameEngine::GameEngine engine;
    engine.init();

    engine.registerComponent<Sprite>();

    Entity entity = engine.createEntity("sprite");
    engine.addComponent(entity, Sprite(Assets::LOGO_RTYPE, ZIndex::IS_UI_HUD));

    auto& sprites = engine.getComponents<Sprite>();
    bool found = false;
    for (auto& sprite : sprites) {
        if (sprite.has_value()) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    engine.removeComponent<Sprite>(entity);
    EXPECT_FALSE(engine.hasComponent<Sprite>(entity));
}

TEST(GameEngineCoverage, SystemLifecycle)
{
    gameEngine::GameEngine engine;
    engine.init();

    auto& system = engine.registerSystem<DummySystem>();
    engine.setSystemSignature<DummySystem, Transform>();

    engine.onCreateSystems();
    engine.updateSystems(0.1f);
    engine.onDestroySystems();

    EXPECT_EQ(system.createCount, 1);
    EXPECT_EQ(system.updateCount, 1);
    EXPECT_EQ(system.destroyCount, 1);

    engine.removeSystem<DummySystem>();
}

TEST(GameEngineCoverage, PlayerInputActions)
{
    gameEngine::GameEngine engine;
    engine.init();

    engine.registerComponent<InputComponent>();
    engine.registerComponent<Playable>();

    Entity player = engine.createEntity("player");
    engine.addComponent(player, InputComponent(1));
    engine.addComponent(player, Playable{});

    engine.setPlayerInputAction(player, 1, GameAction::SHOOT, true);
    EXPECT_TRUE(engine.isPlayerActionActive(player, GameAction::SHOOT));

    auto& actions = engine.getPlayerActiveActions(player);
    EXPECT_TRUE(actions[GameAction::SHOOT]);

    Entity other = engine.createEntity("other");
    EXPECT_FALSE(engine.isPlayerActionActive(other, GameAction::MOVE_LEFT));
}

TEST(GameEngineCoverage, SyncInputsToPlayableEntities)
{
    gameEngine::GameEngine engine;
    engine.init();

    engine.registerComponent<InputComponent>();
    engine.registerComponent<Playable>();

    engine._renderManager = std::make_shared<RenderManager>();
    engine._renderManager->getActiveActions()[GameAction::SHOOT] = true;

    Entity player = engine.createEntity("player");
    engine.addComponent(player, InputComponent(1));
    engine.addComponent(player, Playable{});

    engine.syncInputsToPlayableEntities();

    auto& input = engine.getComponentEntity<InputComponent>(player);
    ASSERT_TRUE(input.has_value());
    EXPECT_TRUE(input->activeActions[GameAction::SHOOT]);
}

TEST(GameEngineCoverage, AudioManagerUtilities)
{
    gameEngine::GameEngine engine;
    engine.init();

    engine._audioManager = std::make_shared<audio::AudioManager>();

    engine.setMasterVolume(0.2f);
    engine.setSoundVolume(0.3f);
    engine.setMusicVolume(0.4f);
    engine.updateAudio();

    EXPECT_NE(engine.getAudioManager(), nullptr);
}
