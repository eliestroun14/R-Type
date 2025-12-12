#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/ecs/system/MovementSystem.hpp>

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

        _coordinator->registerComponent<Transform>();
        _coordinator->registerComponent<Velocity>();

        _coordinator->registerSystem<MovementSystem>(*_coordinator);

        _coordinator->setSystemSignature<MovementSystem, Transform, Velocity>();

        Entity player = _coordinator->createEntity("Player");
        _coordinator->addComponent<Transform>(player, Transform{0.f, 0.f, 0.f, 1.f});
        _coordinator->addComponent<Velocity>(player, Velocity{50.f, 0.f});


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
        if (type == NetworkType::NETWORK_TYPE_SERVER || type == NetworkType::NETWORK_TYPE_STANDALONE) {
            // this->_coordinator->processInput();
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
            // this->_coordinator->render();
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
     *   - update(dt)
     *   - render(type)
     *
     * This function represents a full iteration of the game loop.
     */
    void GameEngine::process(float dt, NetworkType type) {
        this->processInput(type);
        this->update(dt);
        this->render(type);
    }

}
