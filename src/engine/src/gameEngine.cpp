 #include "src/engine/include/engine/gameEngine/GameEngine.hpp"

namespace gameEngine {
    void GameEngine::init() {
        this->_coordinator = std::make_unique<Coordinator>();
    }

    void GameEngine::update(float dt) {
        this->_coordinator->updateSystems(dt);
    }

    void GameEngine::processInput(NetworkType type) {
        if (type == NetworkType::SERVER || type == NetworkType::STANDALONE) {
            //this->_coordinator->processInput();
        } else if (type == NetworkType::SERVER) {
            return;
        }
    }

    void GameEngine::render(NetworkType type) {
        if (type == NetworkType::CLIENT || type == NetworkType::STANDALONE) {
            // this->_coordinator->render();
        } else if (type == NetworkType::SERVER) {
            return;
        }
    }

    void GameEngine::process(float dt, NetworkType type) {
        this->processInput(type);
        this->update(dt);
        this->render(type);
    }
}