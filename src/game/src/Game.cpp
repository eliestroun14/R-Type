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

        _coordinator->initEngine();

        // Initialize render only for client and standalone
        if (_type == Type::CLIENT || _type == Type::STAND_ALONE) {
            _coordinator->initEngineRender();
        }

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
        // Calculate elapsed time since last tick
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - _lastTickTime
        ).count();
        _lastTickTime = currentTime;
        _accumulatedTime += elapsed;

        // Fixed timestep loop - may execute multiple ticks if frame took too long
        // or skip if not enough time has passed
        while (_accumulatedTime >= TICK_RATE_MS) {
            _accumulatedTime -= TICK_RATE_MS;

            // ====================================================================
            // ROLE-BASED GAME TICK
            // ====================================================================
            if (_type == Type::SERVER) {
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

        // STEP 3: Generate Authoritative State Packets
        std::vector<common::protocol::Packet> outgoingPackets;
        _coordinator->buildServerPacketBasedOnStatus(outgoingPackets, elapsedMs);

        // STEP 4: Queue Outgoing Packets
        for (const auto& packet : outgoingPackets) {
            // Broadcast to all clients (target = nullopt)
            addOutgoingPacket(packet, std::nullopt);
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
        _coordinator->processClientPackets(packetsToProcess, elapsedMs);

        // STEP 2: Client-Side Prediction
        // Apply local player inputs immediately for responsive feel
        // (Server will validate and correct if needed in next update)

        // STEP 3: Generate Input Packets to Server
        std::vector<common::protocol::Packet> outgoingPackets;
        _coordinator->buildClientPacketBasedOnStatus(outgoingPackets, elapsedMs);

        // STEP 4: Queue Outgoing Packets
        for (const auto& packet : outgoingPackets) {
            // Send to server (target will be handled by network layer)
            addOutgoingPacket(packet, std::nullopt);
        }

    } catch (const Error& e) {
        LOG_ERROR("Client tick error: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error in client tick: {}", e.what());
        throw Error(ErrorType::ClientError, "Client tick failed: " + std::string(e.what()));
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

    } catch (const Error& e) {
        LOG_ERROR("Failed to handle player {} connection: {}", playerId, e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected error handling player {} connection: {}", playerId, e.what());
        throw Error(ErrorType::ServerError, "Failed to connect player: " + std::string(e.what()));
    }
}
