#include <engine/gameEngine/GameEngine.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/MovementSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/RenderSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/PlayerSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/AnimationSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/CollisionSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/ShootSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/BackgroundSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/AISystem.hpp>
#include <engine/gameEngine/coordinator/ecs/system/systems/LevelSystem.hpp>
#include <engine/gameEngine/coordinator/network/PacketManager.hpp>
#include <common/constants/render/Assets.hpp>

namespace gameEngine {

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
        // createEnemies();
        createLevel();

        _coordinator->onCreateSystems();
    }


    void GameEngine::update(float dt) {
        this->_coordinator->updateSystems(dt);
    }


    void GameEngine::processInput(NetworkType type) {
        if (type == NetworkType::NETWORK_TYPE_CLIENT || type == NetworkType::NETWORK_TYPE_STANDALONE) {
            this->_coordinator->processInput();
        } else if (type == NetworkType::NETWORK_TYPE_SERVER) {
            return;
        }
    }


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
        _coordinator->registerComponent<InputComponent>();
        _coordinator->registerComponent<Animation>();
        _coordinator->registerComponent<HitBox>();
        _coordinator->registerComponent<Weapon>();
        _coordinator->registerComponent<Projectile>();
        _coordinator->registerComponent<Health>();
        _coordinator->registerComponent<ScrollingBackground>();
        _coordinator->registerComponent<AI>();
        _coordinator->registerComponent<Level>();


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
        _coordinator->setSystemSignature<PlayerSystem, Velocity, InputComponent>();

        _coordinator->registerSystem<AnimationSystem>(*_coordinator);
        _coordinator->setSystemSignature<AnimationSystem, Animation, Sprite>();

        _coordinator->registerSystem<CollisionSystem>(*_coordinator);
        _coordinator->setSystemSignature<CollisionSystem, Transform, Sprite, HitBox>();

        _coordinator->registerSystem<ShootSystem>(*_coordinator);
        _coordinator->setSystemSignature<ShootSystem, Weapon, Transform, InputComponent>();

        _coordinator->registerSystem<BackgroundSystem>(*_coordinator);
        _coordinator->setSystemSignature<BackgroundSystem, Transform, Sprite, ScrollingBackground>();

        _coordinator->registerSystem<AISystem>(*_coordinator);
        _coordinator->setSystemSignature<AISystem, AI>();

        _coordinator->registerSystem<LevelSystem>(*_coordinator);
        _coordinator->setSystemSignature<LevelSystem, Level>();
    }

    void GameEngine::createPlayer()
    {
        // Player is now created via ENTITY_SPAWN packet from the server
        // This function is kept for backwards compatibility but does nothing
        // The server will send an ENTITY_SPAWN packet with is_playable=1 to indicate the local player
    }

    void GameEngine::createBackground()
    {
        Entity backgroundStars = _coordinator->createEntity("Stars Background");

        _coordinator->addComponent<Sprite>(backgroundStars, Sprite(GAME_BG, ZIndex::IS_BACKGROUND, sf::IntRect(0, 0, 1920, 1080)));
        _coordinator->addComponent<Transform>(backgroundStars, Transform(0.f, 0.f, 0.f, 1.0f));
        _coordinator->addComponent<ScrollingBackground>(backgroundStars, ScrollingBackground(0.05f, true, true));
    }

    void GameEngine::createEnemies()
    {
        // Create a static enemy for collision testing
        Entity staticEnemy = _coordinator->createEntity("StaticEnemy");

        //_coordinator->addComponent<Sprite>(staticEnemy, Sprite(BASE_ENEMY, 1, sf::IntRect(0, 0, 33, 36)));
        //_coordinator->addComponent<Transform>(staticEnemy, Transform(400.f, 200.f, 0.f, 2.0f));
        //_coordinator->addComponent<Health>(staticEnemy, Health(50, 50));
        //_coordinator->addComponent<HitBox>(staticEnemy, HitBox());
        //_coordinator->addComponent<Velocity>(staticEnemy, Velocity(0.f, 0.f));
        //_coordinator->addComponent<Weapon>(staticEnemy, Weapon(300, 0, 8, ProjectileType::MISSILE));  // 300ms fire rate, 8 damage
    }

    void GameEngine::createLevel()
    {
        Entity levelEntity = _coordinator->createEntity("Level1");

        Level level;

        // Wave 1: 3 basic enemies
        Wave wave1;
        wave1.startTime = 2.0f;  // Start after 2 seconds
        wave1.enemies.push_back({EnemyType::BASIC, 800.f, 100.f, 0.f});
        wave1.enemies.push_back({EnemyType::BASIC, 800.f, 200.f, 1.0f});  // 1 sec after previous
        wave1.enemies.push_back({EnemyType::BASIC, 800.f, 300.f, 1.0f});  // 1 sec after previous

        // Wave 2: Mix of enemies
        Wave wave2;
        wave2.startTime = 5000.0f;  // Start after 10 seconds
        wave2.enemies.push_back({EnemyType::FAST, 800.f, 150.f, 0.f});
        wave2.enemies.push_back({EnemyType::BASIC, 800.f, 250.f, 0.5f});
        wave2.enemies.push_back({EnemyType::FAST, 800.f, 350.f, 0.5f});

        // Wave 3: Tank
        Wave wave3;
        wave3.startTime = 10000.0f;
        wave3.enemies.push_back({EnemyType::TANK, 800.f, 200.f, 0.f});

        level.waves.push_back(wave1);
        level.waves.push_back(wave2);
        level.waves.push_back(wave3);

        _coordinator->addComponent<Level>(levelEntity, level);
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
    void GameEngine::process(float dt, NetworkType type, std::vector<common::protocol::Packet> &packetsToProcess, uint64_t elapsedMs) { // + packetsToProcess + elapsedMs

        // handlePacket forwarded

        handlePacket(type, packetsToProcess, elapsedMs);
        // if (_coordinator->_gameRunning == false) {
        //     return;
        // }
        this->processInput(type);
        if (type == NetworkType::NETWORK_TYPE_CLIENT || type == NetworkType::NETWORK_TYPE_STANDALONE) {
            this->_coordinator->beginFrame();
        }

        // Update systems for all network types (CLIENT, STANDALONE, and SERVER)
        this->update(dt);

        this->render(type);
    }

    void GameEngine::processInput()
    {
        this->_coordinator->processInput();
    }

    void GameEngine::handlePacket(NetworkType type, const std::vector<common::protocol::Packet>& packetsToProcess, uint64_t elapsedMs)
    {
        if (type == NetworkType::NETWORK_TYPE_SERVER) {
            // Handle server-specific packet processing
            _coordinator->processServerPackets(packetsToProcess, elapsedMs);
        } else if (type == NetworkType::NETWORK_TYPE_CLIENT) {
            // Handle client-specific packet processing
            _coordinator->processCLientPackets(packetsToProcess, elapsedMs);
        }
    }

    void GameEngine::buildPacketBasedOnStatus(NetworkType type, uint64_t elapsedMs, std::vector<common::protocol::Packet> &outgoingPackets)
    {
        // TODO
        if (type == NetworkType::NETWORK_TYPE_SERVER) {
            _coordinator->buildSeverPacketBasedOnStatus(outgoingPackets, elapsedMs);
        }
        else if (type == NetworkType::NETWORK_TYPE_CLIENT) {
            _coordinator->buildClientPacketBasedOnStatus(outgoingPackets, elapsedMs);
    }

}
    void GameEngine::broadcastSnapshots(common::network::INetworkManager& networkManager, NetworkType type)
    {
        // Only broadcast snapshots on the server
        if (type != NETWORK_TYPE_SERVER) {
            return;
        }

        broadcastTransformSnapshots(networkManager);
        broadcastVelocitySnapshots(networkManager);
        broadcastHealthSnapshots(networkManager);
        broadcastAnimationSnapshots(networkManager);
        broadcastWeaponSnapshots(networkManager);
    }

    void GameEngine::broadcastTransformSnapshots(common::network::INetworkManager& networkManager)
    {
        auto& transforms = _coordinator->getComponents<Transform>();
        for (size_t entityId = 0; entityId < transforms.size(); ++entityId) {
            auto& transform = transforms[entityId];
            if (!transform.has_value()) continue;

            // TODO: Create and queue TRANSFORM_SNAPSHOT packet
            // TODO: auto packet = PacketManager::createTransformSnapshot(entityId, transform.value());
            // TODO: networkManager.queueOutgoing(packet, std::nullopt);
        }
    }

    void GameEngine::broadcastVelocitySnapshots(common::network::INetworkManager& networkManager)
    {
        auto& velocities = _coordinator->getComponents<Velocity>();
        for (size_t entityId = 0; entityId < velocities.size(); ++entityId) {
            auto& velocity = velocities[entityId];
            if (!velocity.has_value()) continue;

            // TODO: Create and queue VELOCITY_SNAPSHOT packet
            // TODO: auto packet = PacketManager::createVelocitySnapshot(entityId, velocity.value());
            // TODO: networkManager.queueOutgoing(packet, std::nullopt);
        }
    }

    void GameEngine::broadcastHealthSnapshots(common::network::INetworkManager& networkManager)
    {
        auto& healths = _coordinator->getComponents<Health>();
        for (size_t entityId = 0; entityId < healths.size(); ++entityId) {
            auto& health = healths[entityId];
            if (!health.has_value()) continue;

            // TODO: Create and queue HEALTH_SNAPSHOT packet
            // TODO: auto packet = PacketManager::createHealthSnapshot(entityId, health.value());
            // TODO: networkManager.queueOutgoing(packet, std::nullopt);
        }
    }

    void GameEngine::broadcastAnimationSnapshots(common::network::INetworkManager& networkManager)
    {
        auto& animations = _coordinator->getComponents<Animation>();
        for (size_t entityId = 0; entityId < animations.size(); ++entityId) {
            auto& animation = animations[entityId];
            if (!animation.has_value()) continue;

            // TODO: Create and queue ANIMATION_SNAPSHOT packet
            // TODO: auto packet = PacketManager::createAnimationSnapshot(entityId, animation.value());
            // TODO: networkManager.queueOutgoing(packet, std::nullopt);
        }
    }

    void GameEngine::broadcastWeaponSnapshots(common::network::INetworkManager& networkManager)
    {
        auto& weapons = _coordinator->getComponents<Weapon>();
        for (size_t entityId = 0; entityId < weapons.size(); ++entityId) {
            auto& weapon = weapons[entityId];
            if (!weapon.has_value()) continue;

            // TODO: Create and queue WEAPON_SNAPSHOT packet
            // TODO: auto packet = PacketManager::createWeaponSnapshot(entityId, weapon.value());
            // TODO: networkManager.queueOutgoing(packet, std::nullopt);
        }
    }

}
