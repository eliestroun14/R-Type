#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/ecs/system/MovementSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/RenderSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/PlayerSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/AnimationSystem.hpp>
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

        // Composants (pas besoin de RenderManager pour ça)
        _coordinator->registerComponent<Transform>();
        _coordinator->registerComponent<Velocity>();
        _coordinator->registerComponent<Sprite>();
        _coordinator->registerComponent<Playable>();
        _coordinator->registerComponent<Animation>();

        std::cout << "Components registered" << std::endl;

    }

    void GameEngine::initRender()
    {
        if (!_coordinator) throw std::runtime_error("GameEngine::init() must be called before initRender()");

        _coordinator->initRender();

        _coordinator->registerSystem<MovementSystem>(*_coordinator);
        _coordinator->registerSystem<RenderSystem>(*_coordinator);
        _coordinator->registerSystem<PlayerSystem>(*_coordinator);
        _coordinator->registerSystem<AnimationSystem>(*_coordinator);

        // Set system signatures BEFORE creating entities
        _coordinator->setSystemSignature<MovementSystem, Transform, Velocity>();
        _coordinator->setSystemSignature<RenderSystem, Sprite, Transform>();
        _coordinator->setSystemSignature<PlayerSystem, Playable, Velocity>();
        _coordinator->setSystemSignature<AnimationSystem, Animation, Sprite>();

        Entity player = _coordinator->createEntity("Player");
        Entity background = _coordinator->createEntity("Stars Background");
        
        _coordinator->addComponent<Sprite>(player, Sprite(PLAYER_1, 1, sf::IntRect(0, 0, 33, 15)));
        _coordinator->addComponent<Transform>(player, Transform(100.f, 150.f, 0.f, 5.0f));
        _coordinator->addComponent<Playable>(player, Playable{});
        _coordinator->addComponent<Velocity>(player, Velocity(0.f, 0.f));
        // Animation: frameWidth=33, frameHeight=15, currentFrame=2 (neutral), elapsedTime=0, frameDuration=0.1s
        // startFrame=2, endFrame=2 (neutral frame), loop=true
        _coordinator->addComponent<Animation>(player, Animation(33, 15, 2, 0.f, 0.1f, 2, 2, true));

        _coordinator->addComponent<Sprite>(background, Sprite(STARS_BG, 0, sf::IntRect(0, 0, 755, 448)));
        _coordinator->addComponent<Transform>(background, Transform(0.f, 0.f, 0.f, 2.0f));

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
