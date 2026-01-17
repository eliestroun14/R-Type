/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game
*/

#include <game/Game.hpp>
#include <iostream>
#include <common/protocol/Protocol.hpp>
#include <common/constants/defines.hpp>
#include <random>
#include <common/protocol/PacketManager.hpp>
#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>

Game::Game(Type type)
    : _type(type), _isRunning(true), _isConnected(false)
{
    try {
        // Initialize coordinator
        _coordinator = std::make_shared<Coordinator>();
        if (!_coordinator) {
            throw Error(ErrorType::GameplayError, "Failed to create Coordinator instance");
        }

        // Set server/client mode
        _coordinator->setIsServer(_type == Type::SERVER);

        _coordinator->initEngine();

        // Initialize render only for client and standalone
        if (_type == Type::CLIENT || _type == Type::STAND_ALONE) {
            _coordinator->initEngineRender();
        }

        //Entity configurationEntity = this->getCoordinator()->getEngine()->createEntity("Configuration Game Entity");
        //this->getCoordinator()->getEngine()->addComponent<GameConfig>(configurationEntity, GameConfig(FontAssets::DEFAULT_FONT, true, true));

        // Initialize timing
        _lastTickTime = std::chrono::steady_clock::now();
        _accumulatedTime = 0;

        LOG_INFO("Game initialized as {}", 
                 _type == Type::SERVER ? "SERVER" : 
                 (_type == Type::CLIENT ? "CLIENT" : "STAND_ALONE"));
    } catch (const Error& e) {
        LOG_ERROR("Game initialization failed: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error during Game initialization: {}", e.what());
        throw Error(ErrorType::GameplayError, "Failed to initialize Game: " + std::string(e.what()));
    }
}

Game::~Game()
{
    _isRunning = false;
    LOG_INFO("Game destroyed");
}

bool Game::runGameLoop()
{
    // ============================================================================
    // FIXED TIMESTEP GAME LOOP
    // ============================================================================
    // This method implements a deterministic game loop with fixed timestep
    // for authoritative server simulation and client-side prediction.
    // ============================================================================

    try {
        //LOG_INFO("runGameLoop: CALLED for type={}", static_cast<int>(_type));

        // Calculate elapsed time since last tick
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - _lastTickTime
        ).count();
        _lastTickTime = currentTime;
        _accumulatedTime += elapsed;

        //LOG_INFO("runGameLoop: elapsed={} accumulated={} tickRate={}", elapsed, _accumulatedTime, TICK_RATE_MS);

        // Fixed timestep loop - may execute multiple ticks if frame took too long
        // or skip if not enough time has passed
        while (_accumulatedTime >= TICK_RATE_MS) {
            _accumulatedTime -= TICK_RATE_MS;

            //LOG_INFO("runGameLoop: executing tick for type={}", static_cast<int>(_type));

            // ====================================================================
            // ROLE-BASED GAME TICK
            // ====================================================================
            if (_type == Type::SERVER) {
                //LOG_INFO("runGameLoop: calling serverTick");
                serverTick(TICK_RATE_MS);
            } else if (_type == Type::CLIENT) {
                clientTick(TICK_RATE_MS);
            } else {
                serverTick(TICK_RATE_MS); // Use server logic locally
            }
        }

        // ========================================================================
        // INPUT PROCESSING & RENDERING (Client Only)
        // ========================================================================
        if (_type == Type::CLIENT || _type == Type::STAND_ALONE) {
            auto engine = _coordinator->getEngine();
            if (!engine) {
                LOG_WARN("Engine instance is null, skipping render");
                return _isRunning;
            }

            //if (_menu) {
            //    _menu->update();
            //}

            // Process window events (CRITICAL: prevents window from freezing)
            engine->processInput();

            // Check if window was closed
            if (!engine->isWindowOpen()) {
                LOG_INFO("Window closed, stopping game loop");
                _isRunning = false;
                return false;
            }

            // Clear the window and prepare for rendering
            engine->beginFrame();

            // Update systems (including RenderSystem which draws entities)
            float deltaSeconds = TICK_RATE_MS / 1000.0f;
            engine->updateSystems(deltaSeconds);

            // Display the rendered frame
            engine->render();
        }

        return _isRunning;

    } catch (const Error& e) {
        LOG_ERROR("Game loop error: {}", e.what());
        _isRunning = false;
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error in game loop: {}", e.what());
        _isRunning = false;
        throw Error(ErrorType::GameplayError, "Game loop failed: " + std::string(e.what()));
    }
}

void Game::serverTick(uint64_t elapsedMs)
{
    // ============================================================================
    // SERVER TICK: Authoritative Simulation
    // ============================================================================
    // 1. Process incoming client input packets
    // 2. Validate and apply inputs to ECS
    // 3. Update all game systems (physics, collision, AI, etc.) = simulate
    // 4. Generate authoritative state packets = buildPacketBasedOnStatus
    // 5. Queue outgoing packets to clients
    // ============================================================================
    LOG_DEBUG("Server tick: elapsedMs={}", elapsedMs);

    try {
        // STEP 1: Process Incoming Packets (Client Inputs)
        std::vector<common::protocol::Packet> packetsToProcess;
        while (true) {
            auto maybePacket = popIncomingPacket();
            if (!maybePacket.has_value())
                break;
            packetsToProcess.push_back(maybePacket.value().packet);
        }

        if (!packetsToProcess.empty()) {
            LOG_INFO("Server tick: processing {} packets before coordinator", packetsToProcess.size());
        }

        // Let coordinator handle packet processing (validation, input queuing)
        _coordinator->processServerPackets(packetsToProcess, elapsedMs);

        // STEP 2: Update ECS Systems (Deterministic Order)
        // Server simulates the authoritative game state
        auto engine = _coordinator->getEngine();
        if (!engine) {
            LOG_ERROR("Engine instance is null in serverTick");
            throw Error(ErrorType::GameplayError, "Engine unavailable during server tick");
        }

        float deltaSeconds = elapsedMs / 1000.0f;
        engine->updateSystems(deltaSeconds);

        // STEP 2.5: Check for level completion (server-side only)
        if (_levelStarted && static_cast<std::size_t>(_currentLevelEntity) != 0) {
            auto& levels = engine->getComponents<Level>();
            if (levels[_currentLevelEntity] && levels[_currentLevelEntity]->completed) {
                LOG_INFO("Game: Level completed! Sending LEVEL_COMPLETE packets to all clients");

                // Send LEVEL_COMPLETE packet to all clients
                common::protocol::Packet levelCompletePacket;
                levelCompletePacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_COMPLETE);
                levelCompletePacket.header.sequence_number = 0;
                levelCompletePacket.header.timestamp = 0;

                protocol::LevelComplete levelCompletePayload;
                levelCompletePayload.completed_level = LEVEL_1_NUMBER;
                levelCompletePayload.next_level = 0xFF;  // 0xFF = game end
                levelCompletePayload.bonus_score = 0;
                levelCompletePayload.completion_time = 0;

                levelCompletePacket.data.resize(sizeof(protocol::LevelComplete));
                std::memcpy(levelCompletePacket.data.data(), &levelCompletePayload, sizeof(protocol::LevelComplete));

                // Send to all connected clients
                for (uint32_t playerId : _connectedPlayers) {
                    addOutgoingPacket(levelCompletePacket, playerId);
                }

                // Stop the game after a short delay (let clients process the completion)
                LOG_INFO("Game: Level complete, preparing to end game...");
                // We'll end the game loop after this tick
                _isRunning = false;
            }
        }

        // STEP 3: Generate Authoritative State Packets
        std::vector<common::protocol::Packet> outgoingPackets;
        _coordinator->buildServerPacketBasedOnStatus(outgoingPackets, elapsedMs);

        // STEP 4: Queue Outgoing Packets
        // For PLAYER_INPUT packets, exclude the source player to avoid double-processing
        // For all other packets, broadcast to all connected players
        auto allPlayerIds = _coordinator->getAllConnectedPlayerIds();

        for (const auto& packet : outgoingPackets) {
            // Check if this is a PLAYER_INPUT packet
            if (packet.header.packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_PLAYER_INPUT)) {
                // Relay to all players EXCEPT the source
                for (uint32_t targetPlayer : allPlayerIds) {
                    if (_coordinator->shouldSendPacketToPlayer(packet, targetPlayer)) {
                        LOG_DEBUG_CAT("Game", "Queuing PLAYER_INPUT packet for player {}", targetPlayer);
                        addOutgoingPacket(packet, targetPlayer);
                    }
                }
            } else {
                // Broadcast other packets to all players
                addOutgoingPacket(packet, std::nullopt);
            }
        }

    } catch (const Error& e) {
        LOG_ERROR("Server tick error: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error in server tick: {}", e.what());
        throw Error(ErrorType::ServerError, "Server tick failed: " + std::string(e.what()));
    }
}

void Game::clientTick(uint64_t elapsedMs)
{
    // ============================================================================
    // CLIENT TICK: Prediction and Reconciliation
    // ============================================================================
    // 1. Process incoming server state packets
    // 2. Apply server state to ECS (reconciliation - hard overwrite for now)
    // 3. Perform client-side prediction using local inputs
    // 4. Update all game systems locally
    // 5. Generate and queue input packets to server
    // ============================================================================
    LOG_DEBUG("Client tick: elapsedMs={}", elapsedMs);

    try {
        // STEP 1: Process Incoming Packets (Server State Updates)
        std::vector<common::protocol::Packet> packetsToProcess;
        while (true) {
            auto maybePacket = popIncomingPacket();
            if (!maybePacket.has_value())
                break;
            packetsToProcess.push_back(maybePacket.value().packet);
        }

        // Let coordinator handle server state updates
        // This includes reconciliation: direct replacement of local state with server state
        try {
            _coordinator->processClientPackets(packetsToProcess, elapsedMs);
        } catch (const std::exception& e) {
            LOG_ERROR("Error processing client packets: {}, continuing...", e.what());
            // Don't crash, just skip this tick's packet processing
        }

        // STEP 2: Client-Side Prediction
        // Apply local player inputs immediately for responsive feel
        // (Server will validate and correct if needed in next update)

        // STEP 3: Generate Input Packets to Server
        std::vector<common::protocol::Packet> outgoingPackets;
        try {
            _coordinator->buildClientPacketBasedOnStatus(outgoingPackets, elapsedMs);
        } catch (const std::exception& e) {
            LOG_ERROR("Error building client packets: {}, continuing...", e.what());
            // Don't crash, just skip packet generation this tick
        }

        // STEP 4: Queue Outgoing Packets
        for (const auto& packet : outgoingPackets) {
            // Send to server (target will be handled by network layer)
            addOutgoingPacket(packet, std::nullopt);
        }

    } catch (const Error& e) {
        LOG_ERROR("Client tick error: {}, continuing...", e.what());
        // Don't throw - let the game continue running
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error in client tick: {}, continuing...", e.what());
        // Don't throw - let the game continue running
    }
}

void Game::addIncomingPacket(const common::network::ReceivedPacket &packet)
{
    try {
        std::lock_guard<std::mutex> guard(_incomingMutex);
        _incoming.push_back(packet);
        LOG_TRACE("Game: incoming packet queued, queue size={}", _incoming.size());
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to add incoming packet: {}", e.what());
    }
}

std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> Game::popOutgoingPacket()
{
    try {
        std::lock_guard<std::mutex> guard(_outgoingMutex);
        if (!_outgoing.empty()) {
            auto packet = _outgoing.front();
            _outgoing.pop_front();
            LOG_TRACE("Game: popped outgoing packet type={}, remaining={}", 
                     static_cast<int>(packet.first.header.packet_type), _outgoing.size());
            return packet;
        }
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to pop outgoing packet: {}", e.what());
        return std::nullopt;
    }
}

void Game::addOutgoingPacket(const common::protocol::Packet &packet, std::optional<uint32_t> target)
{
    try {
        std::lock_guard<std::mutex> guard(_outgoingMutex);
        _outgoing.emplace_back(packet, target);
        LOG_TRACE("Game: outgoing packet queued, queue size={}", _outgoing.size());
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to add outgoing packet: {}", e.what());
    }
}

std::optional<common::network::ReceivedPacket> Game::popIncomingPacket()
{
    try {
        std::lock_guard<std::mutex> guard(_incomingMutex);
        if (!_incoming.empty()) {
            auto packet = _incoming.front();
            _incoming.pop_front();
            LOG_TRACE("Game: popped incoming packet type={}, remaining={}", 
                     static_cast<int>(packet.packet.header.packet_type), _incoming.size());
            return packet;
        }
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to pop incoming packet: {}", e.what());
        return std::nullopt;
    }
}

void Game::sendExistingPlayersToNewClient(uint32_t newPlayerId)
{
    for (uint32_t existingPlayerId : _connectedPlayers) {
        LOG_INFO("Game: Sending existing player {} to new client {}", existingPlayerId, newPlayerId);

        auto engine = _coordinator->getEngine();
        if (!engine) {
            LOG_ERROR("Engine instance is null while sending existing players");
            continue;
        }

        try {
            Entity existingEntity = engine->getEntityFromId(existingPlayerId);
            auto& transforms = engine->getComponents<Transform>();

            // Use implicit conversion from Entity to size_t
            std::size_t entityId = existingEntity;
            if (entityId >= transforms.size() || !transforms[entityId].has_value()) {
                LOG_WARN("Game: No transform found for existing player {}", existingPlayerId);
                continue;
            }

            const auto& transform = transforms[entityId].value();

            // Create ENTITY_SPAWN packet manually (entity already exists!)
            std::vector<uint8_t> args;

            // flags_count (1 byte)
            args.push_back(1);
            // flags (FLAG_RELIABLE)
            args.push_back(static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE));

            // sequence_number (4 bytes)
            static uint32_t sequence = ENTITY_SPAWN_SEQUENCE_BASE;
            sequence++;
            args.push_back(static_cast<uint8_t>(sequence & 0xFF));
            args.push_back(static_cast<uint8_t>((sequence >> 8) & 0xFF));
            args.push_back(static_cast<uint8_t>((sequence >> 16) & 0xFF));
            args.push_back(static_cast<uint8_t>((sequence >> 24) & 0xFF));

            // timestamp (4 bytes)
            uint32_t timestamp = static_cast<uint32_t>(TIMESTAMP);
            args.push_back(static_cast<uint8_t>(timestamp & 0xFF));
            args.push_back(static_cast<uint8_t>((timestamp >> 8) & 0xFF));
            args.push_back(static_cast<uint8_t>((timestamp >> 16) & 0xFF));
            args.push_back(static_cast<uint8_t>((timestamp >> 24) & 0xFF));

            // entity_id (4 bytes)
            args.push_back(static_cast<uint8_t>(existingPlayerId & 0xFF));
            args.push_back(static_cast<uint8_t>((existingPlayerId >> 8) & 0xFF));
            args.push_back(static_cast<uint8_t>((existingPlayerId >> 16) & 0xFF));
            args.push_back(static_cast<uint8_t>((existingPlayerId >> 24) & 0xFF));

            // entity_type (1 byte)
            args.push_back(static_cast<uint8_t>(protocol::EntityTypes::ENTITY_TYPE_PLAYER));

            // position_x (2 bytes)
            uint16_t px = static_cast<uint16_t>(transform.x);
            args.push_back(static_cast<uint8_t>(px & 0xFF));
            args.push_back(static_cast<uint8_t>((px >> 8) & 0xFF));

            // position_y (2 bytes)
            uint16_t py = static_cast<uint16_t>(transform.y);
            args.push_back(static_cast<uint8_t>(py & 0xFF));
            args.push_back(static_cast<uint8_t>((py >> 8) & 0xFF));

            // mob_variant (1 byte)
            args.push_back(0);

            // initial_health (1 byte)
            args.push_back(PLAYER_INITIAL_HEALTH);

            // initial_velocity_x (2 bytes)
            args.push_back(0);
            args.push_back(0);

            // initial_velocity_y (2 bytes)
            args.push_back(0);
            args.push_back(0);

            // is_playable (1 byte) - NOT playable for the new client
            args.push_back(0);

            auto existingPlayerPacket = PacketManager::createEntitySpawn(args);
            if (existingPlayerPacket.has_value()) {
                addOutgoingPacket(existingPlayerPacket.value(), newPlayerId);
                // Mark this entity as broadcasted to prevent duplicate ENTITY_SPAWN from Coordinator
                _coordinator->markEntityAsBroadcasted(existingPlayerId);
                LOG_INFO("Game: Sent existing player {} info to new client {}", 
                         existingPlayerId, newPlayerId);
            } else {
                LOG_ERROR("Game: Failed to create entity spawn packet for existing player {}", 
                         existingPlayerId);
            }

        } catch (const std::exception& e) {
            LOG_ERROR("Game: Failed to get entity for existing player {}: {}", 
                     existingPlayerId, e.what());
        }
    }
}

void Game::onPlayerConnected(uint32_t playerId)
{
    if (_type != Type::SERVER) {
        LOG_WARN("onPlayerConnected called on non-server instance");
        throw Error(ErrorType::GameplayError, "onPlayerConnected can only be called on server");
    }

    try {
        LOG_INFO("Game: Player {} connected, spawning entity", playerId);

        // Send existing players to the new client
        sendExistingPlayersToNewClient(playerId);

        // Calculate spawn position (stagger players horizontally)
        static uint32_t playerCount = 0;
        playerCount++;

        float spawnX = PLAYER_SPAWN_BASE_X + (playerCount * PLAYER_SPAWN_HORIZONTAL_SPACING);
        float spawnY = PLAYER_SPAWN_BASE_Y;

        // Spawn the player on the server and get the spawn packet
        auto spawnPacket = _coordinator->spawnPlayerOnServer(playerId, spawnX, spawnY);

        if (!spawnPacket.has_value()) {
            LOG_ERROR("Game: Failed to create spawn packet for player {}", playerId);
            throw Error(ErrorType::GameplayError, "Failed to spawn player entity");
        }

        // Create a special version for the owning client with is_playable=1
        auto ownerPacket = spawnPacket.value();
        if (ownerPacket.data.size() > PLAYER_IS_PLAYABLE_FLAG_INDEX) {
            ownerPacket.data[PLAYER_IS_PLAYABLE_FLAG_INDEX] = 1;  // Set is_playable=1
        } else {
            LOG_WARN("Game: Spawn packet data too small to set is_playable flag");
        }

        // Send to the owning client specifically
        addOutgoingPacket(ownerPacket, playerId);
        LOG_INFO("Game: Sent playable ENTITY_SPAWN to owner (player {})", playerId);

        // Send the non-playable version to all OTHER existing clients
        for (uint32_t existingPlayerId : _connectedPlayers) {
            if (existingPlayerId != playerId) {
                addOutgoingPacket(spawnPacket.value(), existingPlayerId);
                LOG_INFO("Game: Sent non-playable ENTITY_SPAWN for player {} to existing player {}", 
                         playerId, existingPlayerId);
            }
        }

        // Add to connected players list
        _connectedPlayers.push_back(playerId);
        LOG_INFO("Game: Player {} successfully spawned. Total players: {}", playerId, _connectedPlayers.size());

        // Check if we should start the level now
        checkAndStartLevel();

    } catch (const Error& e) {
        LOG_ERROR("Failed to handle player {} connection: {}", playerId, e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error handling player {} connection: {}", playerId, e.what());
        throw Error(ErrorType::ServerError, "Failed to connect player: " + std::string(e.what()));
    }
}

void Game::checkAndStartLevel()
{
    if (_type != Type::SERVER) {
        return;  // Only server controls level start
    }

    // Check if level already started
    if (_levelStarted) {
        return;
    }

    // Check if we have enough players
    if (_connectedPlayers.size() < _maxPlayers) {
        LOG_INFO("Game: Waiting for more players ({}/{}) before starting level", 
                 _connectedPlayers.size(), _maxPlayers);
        return;
    }

    LOG_INFO("Game: Max players reached ({}/{}), starting level!", 
             _connectedPlayers.size(), _maxPlayers);

    // Create the level entity
    _currentLevelEntity = _coordinator->createLevelEntity(
        LEVEL_1_NUMBER,
        LEVEL_1_DURATION,
        LEVEL_1_BACKGROUND_ASSET,
        LEVEL_1_MUSIC_ASSET
    );

    // Mark level as started in the component
    auto& levels = _coordinator->getEngine()->getComponents<Level>();
    if (levels[_currentLevelEntity]) {
        levels[_currentLevelEntity]->started = true;
        _levelStarted = true;

        // Send LEVEL_START packet to all clients
        // Payload: level_id(1) + level_name(32) + estimated_duration(2) = 35 bytes
        common::protocol::Packet levelStartPacket;
        levelStartPacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_LEVEL_START);
        levelStartPacket.header.sequence_number = 0;
        levelStartPacket.header.timestamp = static_cast<uint32_t>(TIMESTAMP);
        levelStartPacket.header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);

        // Build payload (35 bytes total)
        levelStartPacket.data.resize(LEVEL_START_PAYLOAD_SIZE);
        uint8_t* ptr = levelStartPacket.data.data();

        // level_id (1 byte)
        uint8_t level_id = LEVEL_1_NUMBER;
        std::memcpy(ptr, &level_id, sizeof(level_id));
        ptr += sizeof(level_id);

        // level_name (32 bytes)
        char level_name[32] = {0};
        std::strncpy(level_name, LEVEL_1_BACKGROUND_ASSET, 31);
        std::memcpy(ptr, level_name, 32);
        ptr += 32;

        // estimated_duration (2 bytes)
        uint16_t estimated_duration = static_cast<uint16_t>(LEVEL_1_DURATION);
        std::memcpy(ptr, &estimated_duration, sizeof(estimated_duration));

        // Send to all connected clients
        for (uint32_t playerId : _connectedPlayers) {
            addOutgoingPacket(levelStartPacket, playerId);
        }

        LOG_INFO("Game: Level started and LEVEL_START packets sent to all {} players", _connectedPlayers.size());
    } else {
        LOG_ERROR("Game: Failed to start level - level component not found on entity");
    }
}
