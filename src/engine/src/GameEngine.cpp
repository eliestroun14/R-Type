#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/ecs/system/MovementSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/RenderSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/PlayerSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/AnimationSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/CollisionSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/ShootSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/BackgroundSystem.hpp>
#include <common/constants/render/Assets.hpp>

namespace gameEngine {

    /**
     * @brief Initialise the GameEngine instance.
     *
     * Instantiates a new Coordinator and assigns it to the internal pointer.
     * This must be called before any update, rendering, or input processing.
     */
    void GameEngine::init() {
        this->_coordinator = std::make_unique<Coordinator>();
        _coordinator->init();

        registerComponents();
    }

    void GameEngine::initRender()
    {
        if (!_coordinator) throw std::runtime_error("GameEngine::init() must be called before initRender()");

        _coordinator->initRender();

        registerSystems();

        createBackground();
        createPlayer();
        createEnemies();

        _coordinator->onCreateSystems();
    }

    /**
     * @brief Updates all registered systems using the Coordinator.
     *
     * @param dt Delta time in seconds since the last update.
     *
     * Delegates to Coordinator::updateSystems(dt).
     * This function assumes that init() has already been called.
     */
    void GameEngine::update(float dt) {
        this->_coordinator->updateSystems(dt);
    }

    /**
     * @brief Processes input depending on the network execution mode.
     *
     * @param type Execution mode (SERVER, CLIENT, STANDALONE).
     *
     * - SERVER mode: currently no input is processed.
     * - CLIENT and STANDALONE modes: would delegate to
     *   Coordinator::processInput().
     */
    void GameEngine::processInput(NetworkType type) {
        if (type == NetworkType::NETWORK_TYPE_CLIENT || type == NetworkType::NETWORK_TYPE_STANDALONE) {
            this->_coordinator->processInput();
        } else if (type == NetworkType::NETWORK_TYPE_SERVER) {
            return;
        }
    }

    /**
     * @brief Renders the game depending on the network execution mode.
     *
     * @param type Execution mode (SERVER, CLIENT, STANDALONE).
     *
     * - CLIENT and STANDALONE modes: would delegate to Coordinator::render(),
     * - SERVER mode: render operations are skipped entirely.
     */
    void GameEngine::render(NetworkType type) {
        if (type == NetworkType::NETWORK_TYPE_CLIENT || type == NetworkType::NETWORK_TYPE_STANDALONE) {
            this->_coordinator->render();
        } else if (type == NetworkType::NETWORK_TYPE_SERVER) {
            return;
        }
    }

    void GameEngine::registerComponents()
    {
        _coordinator->registerComponent<Transform>();
        _coordinator->registerComponent<Velocity>();
        _coordinator->registerComponent<Sprite>();
        _coordinator->registerComponent<Playable>();
        _coordinator->registerComponent<Animation>();
        _coordinator->registerComponent<HitBox>();
        _coordinator->registerComponent<Weapon>();
        _coordinator->registerComponent<Projectile>();
        _coordinator->registerComponent<Health>();
        _coordinator->registerComponent<ScrollingBackground>();

        std::cout << "Components registered" << std::endl;
    }

    void GameEngine::registerSystems()
    {
        // Set system signatures BEFORE creating entities
        _coordinator->registerSystem<MovementSystem>(*_coordinator);
        _coordinator->setSystemSignature<MovementSystem, Transform, Velocity>();

        _coordinator->registerSystem<RenderSystem>(*_coordinator);
        _coordinator->setSystemSignature<RenderSystem, Sprite, Transform>();

        _coordinator->registerSystem<PlayerSystem>(*_coordinator);
        _coordinator->setSystemSignature<PlayerSystem, Playable, Velocity>();

        _coordinator->registerSystem<AnimationSystem>(*_coordinator);
        _coordinator->setSystemSignature<AnimationSystem, Animation, Sprite>();

        _coordinator->registerSystem<CollisionSystem>(*_coordinator);
        _coordinator->setSystemSignature<CollisionSystem, Transform, Sprite, HitBox>();

        _coordinator->registerSystem<ShootSystem>(*_coordinator);
        _coordinator->setSystemSignature<ShootSystem, Weapon, Transform>();

        _coordinator->registerSystem<BackgroundSystem>(*_coordinator);
        _coordinator->setSystemSignature<BackgroundSystem, Transform, Sprite, ScrollingBackground>();
    }

    void GameEngine::createPlayer()
    {
        Entity player = _coordinator->createEntity("Player");

        _coordinator->addComponent<Sprite>(player, Sprite(PLAYER_1, 1, sf::IntRect(0, 0, 33, 15)));
        _coordinator->addComponent<Transform>(player, Transform(100.f, 150.f, 0.f, 2.5f));
        _coordinator->addComponent<Playable>(player, Playable());
        _coordinator->addComponent<Velocity>(player, Velocity(0.f, 0.f));
        _coordinator->addComponent<Health>(player, Health(100, 100));
        _coordinator->addComponent<Animation>(player, Animation(33, 15, 2, 0.f, 0.1f, 2, 2, true));
        _coordinator->addComponent<HitBox>(player, HitBox());
        _coordinator->addComponent<Weapon>(player, Weapon(200, 0, 10, ProjectileType::MISSILE));  // 200ms fire rate, 10 damage
    }

    void GameEngine::createBackground()
    {
        Entity backgroundStars = _coordinator->createEntity("Stars Background");

        _coordinator->addComponent<Sprite>(backgroundStars, Sprite(GAME_BG, 0, sf::IntRect(0, 0, 1920, 1080)));
        _coordinator->addComponent<Transform>(backgroundStars, Transform(0.f, 0.f, 0.f, 1.0f));
        _coordinator->addComponent<ScrollingBackground>(backgroundStars, ScrollingBackground(0.05f, true, true));
    }

    void GameEngine::createEnemies()
    {
        // Create a static enemy for collision testing
        Entity staticEnemy = _coordinator->createEntity("StaticEnemy");

        _coordinator->addComponent<Sprite>(staticEnemy, Sprite(BASE_ENEMY, 1, sf::IntRect(0, 0, 33, 36)));
        _coordinator->addComponent<Transform>(staticEnemy, Transform(400.f, 200.f, 0.f, 2.0f));
        _coordinator->addComponent<Health>(staticEnemy, Health(50, 50));
        _coordinator->addComponent<HitBox>(staticEnemy, HitBox());
        _coordinator->addComponent<Velocity>(staticEnemy, Velocity(0.f, 0.f));
        _coordinator->addComponent<Weapon>(staticEnemy, Weapon(300, 0, 8, ProjectileType::MISSILE));  // 300ms fire rate, 8 damage
    }


    /**
       * @brief Processes a full engine step: input → update → render.
       *
       * @param dt Delta time in seconds.
       * @param type Network execution mode.
       *
       * Executes:
       *   - processInput(type)
       *   - beginFrame(type) [clear buffer]
       *   - update(dt) [systems draw here]
       *   - render(type) [display frame]
       *
       * This function represents a full iteration of the game loop.
       */
    void GameEngine::process(float dt, NetworkType type) {
        this->processInput(type);
        if (type == NetworkType::NETWORK_TYPE_CLIENT || type == NetworkType::NETWORK_TYPE_STANDALONE) {
            this->_coordinator->beginFrame();
        }
        this->update(dt);
        this->render(type);
    }

    void GameEngine::processInput()
    {
        this->_coordinator->processInput();
    }
}
