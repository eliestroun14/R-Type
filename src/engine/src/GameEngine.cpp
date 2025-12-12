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
        std::cout << "ici" << std::endl;
        this->_coordinator = std::make_unique<Coordinator>();
        std::cout << "ccaca" << std::endl;
        _coordinator->init();  // Crée EntityManager et SystemManager
        std::cout << "zizi" << std::endl;
        
        // Composants (pas besoin de RenderManager pour ça)
        this->_coordinator->registerComponent<Transform>();
        this->_coordinator->registerComponent<Velocity>();
        
        std::cout << "Components registered" << std::endl;

    }

    void GameEngine::initRender()
    {
        if (!_coordinator) {
            throw std::runtime_error("GameEngine::init() must be called before initRender()");
        }

        // Crée RenderManager
        _coordinator->initRender();

        // Maintenant enregistre les systèmes qui dépendent du rendu
        this->_coordinator->registerSystem<RenderSystem>(*_coordinator);
        this->_coordinator->registerSystem<PlayerSystem>(*_coordinator);
        this->_coordinator->registerSystem<AnimationSystem>(*_coordinator);

        std::cout << "4" << std::endl;

        this->_coordinator->setSystemSignature<RenderSystem, Sprite, Transform>();
        this->_coordinator->setSystemSignature<PlayerSystem, Playable, Velocity>();
        this->_coordinator->setSystemSignature<AnimationSystem, Animation, Sprite>();

        std::cout << "2" << std::endl;

        Entity player = this->_coordinator->createEntity("Player");
        std::cout << "sdq" << std::endl;

        this->_coordinator->addComponent<Sprite>(player, Sprite{PLAYER_1, 1, sf::IntRect(0, 0, 32, 15)});
        std::cout << "pet" << std::endl;
        this->_coordinator->addComponent<Transform>(player, Transform{50.0f, 50.0f, 0.0f, 1.0f});
        this->_coordinator->addComponent<Playable>(player, Playable{});

        _coordinator->onCreateSystems();
        std::cout << "9" << std::endl;
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
