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

Game::Game(Type type)
    : _type(type), _isRunning(true), _isConnected(false)
{
    // Initialize coordinator
    _coordinator = std::make_shared<Coordinator>();
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
            // ========================================================================
        // ROLE-BASED GAME TICK
        // ========================================================================
            if (_type == Type::SERVER) {


            serverTick(TICK_RATE_MS);
                } else if (_type == Type::CLIENT) {

                    
            clientTick(TICK_RATE_MS);
                } else {

            serverTick(TICK_RATE_MS); // Use server logic locally
        }
    }
    // ============================================================================
    // RENDERING (Client Only)
    // ============================================================================
    if (_type == Type::CLIENT || _type == Type::STAND_ALONE) {
        auto engine = _coordinator->getEngine();
        if (engine) {
            engine->beginFrame();
            engine->render();
        }
    }
    return _isRunning;
}

void Game::serverTick(uint64_t elapsedMs)
{
    // ============================================================================
    // SERVER TICK: Authoritative Simulation
    // ============================================================================
    // 1. Process incoming client input packets
    // 2. Validate and apply inputs to ECS
    // 3. Update all game systems (physics, collision, AI, etc.)
    // 4. Generate authoritative state packets
    // 5. Queue outgoing packets to clients
    // ============================================================================
    LOG_DEBUG("Server tick: elapsedMs={}", elapsedMs);


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
    if (engine) {
        float deltaSeconds = elapsedMs / 1000.0f;
        engine->updateSystems(deltaSeconds);
    }


    // STEP 3: Generate Authoritative State Packets

    std::vector<common::protocol::Packet> outgoingPackets;
    _coordinator->buildSeverPacketBasedOnStatus(outgoingPackets, elapsedMs);


    // STEP 4: Queue Outgoing Packets

    for (const auto& packet : outgoingPackets) {
        // Broadcast to all clients (target = nullopt)
        addOutgoingPacket(packet, std::nullopt);
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


    // STEP 3: Update ECS Systems Locally

    auto engine = _coordinator->getEngine();
    if (engine) {
        float deltaSeconds = elapsedMs / 1000.0f;
        engine->updateSystems(deltaSeconds);
    }


    // STEP 4: Generate Input Packets to Server

    std::vector<common::protocol::Packet> outgoingPackets;
    _coordinator->buildClientPacketBasedOnStatus(outgoingPackets, elapsedMs);


    // STEP 5: Queue Outgoing Packets

    for (const auto& packet : outgoingPackets) {
        // Send to server (target will be handled by network layer)
        addOutgoingPacket(packet, std::nullopt);
    }
}

void Game::addIncomingPacket(const common::network::ReceivedPacket &packet)
{
    std::lock_guard<std::mutex> guard(_incomingMutex);
    _incoming.push_back(packet);
    LOG_DEBUG("Game: incoming queue size={}", _incoming.size());
}

std::optional<std::pair<common::protocol::Packet, std::optional<uint32_t>>> Game::popOutgoingPacket()
{
    std::lock_guard<std::mutex> guard(_outgoingMutex);
    if (!_outgoing.empty()) {
        auto packet = _outgoing.front();
        _outgoing.pop_front();
        LOG_DEBUG("Game: popped outgoing packet type={} remaining={}", static_cast<int>(packet.first.header.packet_type), _outgoing.size());
        return packet;
    }
    return std::nullopt;
}

void Game::addOutgoingPacket(const common::protocol::Packet &packet, std::optional<uint32_t> target)
{
    std::lock_guard<std::mutex> guard(_outgoingMutex);
    _outgoing.emplace_back(packet, target);
    LOG_DEBUG("Game: outgoing queue size={}", _outgoing.size());
}

std::optional<common::network::ReceivedPacket> Game::popIncomingPacket()
{
    std::lock_guard<std::mutex> guard(_incomingMutex);
    if (!_incoming.empty()) {
        auto packet = _incoming.front();
        _incoming.pop_front();
        LOG_DEBUG("Game: popped incoming packet type={} remaining={}", static_cast<int>(packet.packet.header.packet_type), _incoming.size());
        return packet;
    }
    return std::nullopt;
}
