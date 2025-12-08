#include "Server.hpp"
#include <iostream>
#include <cstring>

namespace server {

Server::Server(const ServerConfig& config)
    : m_config(config)
    , m_running(false)
    , m_initialized(false)
    , m_totalPacketsProcessed(0)
    , m_invalidPackets(0)
{
}

Server::~Server() {
    stop();
}

bool Server::initialize() {
    if (m_initialized.load()) {
        std::cerr << "[Server] Already initialized" << std::endl;
        return false;
    }

    std::cout << "[Server] Initializing R-Type Server..." << std::endl;
    std::cout << "[Server] Port: " << m_config.port << std::endl;
    std::cout << "[Server] Max Players: " << m_config.maxPlayers << std::endl;
    std::cout << "[Server] Tick Rate: " << m_config.tickRate << " Hz" << std::endl;

    // Initialiser la couche réseau
    m_networkLayer = std::make_unique<network::UDPSocketManager>();
    if (!m_networkLayer->initialize(m_config.port)) {
        std::cerr << "[Server] Failed to initialize network layer" << std::endl;
        return false;
    }

    // Initialiser la couche session
    m_sessionLayer = std::make_unique<session::ConnectionManager>();
    m_sessionLayer->setHeartbeatTimeout(m_config.heartbeatTimeout);

    m_initialized = true;
    std::cout << "[Server] Initialization complete" << std::endl;
    return true;
}

bool Server::start() {
    if (!m_initialized.load()) {
        std::cerr << "[Server] Cannot start: not initialized" << std::endl;
        return false;
    }

    if (m_running.load()) {
        std::cerr << "[Server] Already running" << std::endl;
        return false;
    }

    std::cout << "[Server] Starting server..." << std::endl;

    // Démarrer les couches
    if (!m_networkLayer->start()) {
        std::cerr << "[Server] Failed to start network layer" << std::endl;
        return false;
    }

    m_sessionLayer->start();

    // Démarrer le thread de heartbeat monitoring
    m_running = true;
    m_heartbeatThread = std::make_unique<std::thread>(&Server::heartbeatThreadFunction, this);

    m_startTime = std::chrono::steady_clock::now();
    std::cout << "[Server] Server started successfully!" << std::endl;
    std::cout << "[Server] Waiting for players to connect..." << std::endl;

    return true;
}

void Server::stop() {
    if (!m_running.load()) {
        return;
    }

    std::cout << "[Server] Stopping server..." << std::endl;
    m_running = false;

    // Arrêter le thread de heartbeat
    if (m_heartbeatThread && m_heartbeatThread->joinable()) {
        m_heartbeatThread->join();
    }

    // Arrêter les couches
    m_sessionLayer->stop();
    m_networkLayer->stop();

    std::cout << "[Server] Server stopped" << std::endl;
    printStatistics();
}

void Server::run() {
    if (!m_running.load()) {
        std::cerr << "[Server] Cannot run: server not started" << std::endl;
        return;
    }

    std::cout << "[Server] Entering main loop..." << std::endl;

    // Boucle principale du serveur
    while (m_running.load()) {
        // Traiter les paquets reçus
        processPackets();

        // Pour l'instant, on fait une petite pause pour ne pas surcharger le CPU
        // Plus tard, on utilisera un fixed timestep pour la simulation du jeu
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "[Server] Exiting main loop" << std::endl;
}

void Server::processPackets() {
    network::RawPacket packet;

    // Traiter tous les paquets disponibles
    while (m_networkLayer->getNextPacket(packet)) {
        m_totalPacketsProcessed++;
        processPacket(packet);
    }
}

void Server::processPacket(const network::RawPacket& rawPacket) {
    // Vérifier la taille minimale
    if (rawPacket.data.size() < sizeof(protocol::PacketHeader)) {
        m_invalidPackets++;
        if (m_config.enableLogging) {
            std::cerr << "[Server] Received packet too small from " 
                      << rawPacket.sender.toString() << std::endl;
        }
        return;
    }

    // Lire le header
    const protocol::PacketHeader* header = 
        reinterpret_cast<const protocol::PacketHeader*>(rawPacket.data.data());

    // Valider le header
    if (!validatePacketHeader(header, rawPacket.data.size())) {
        m_invalidPackets++;
        return;
    }

    // Router selon le type de paquet
    switch (static_cast<protocol::PacketTypes>(header->packet_type)) {
        case protocol::PacketTypes::TYPE_CLIENT_CONNECT:
            handleClientConnect(rawPacket);
            break;

        case protocol::PacketTypes::TYPE_CLIENT_DISCONNECT:
            handleClientDisconnect(rawPacket);
            break;

        case protocol::PacketTypes::TYPE_HEARTBEAT:
            handleHeartbeat(rawPacket);
            break;

        case protocol::PacketTypes::TYPE_PLAYER_INPUT:
            handlePlayerInput(rawPacket);
            break;

        default:
            if (m_config.enableLogging) {
                std::cout << "[Server] Unhandled packet type: 0x" 
                          << std::hex << static_cast<int>(header->packet_type) 
                          << std::dec << " from " << rawPacket.sender.toString() << std::endl;
            }
            break;
    }
}

void Server::handleClientConnect(const network::RawPacket& rawPacket) {
    // Vérifier la taille
    if (rawPacket.data.size() < sizeof(protocol::ClientConnect)) {
        m_invalidPackets++;
        return;
    }

    const protocol::ClientConnect* connectPacket = 
        reinterpret_cast<const protocol::ClientConnect*>(rawPacket.data.data());

    std::string playerName(connectPacket->player_name);
    playerName = playerName.substr(0, playerName.find('\0')); // Trim null characters

    if (m_config.enableLogging) {
        std::cout << "[Server] Connection request from '" << playerName 
                  << "' at " << rawPacket.sender.toString() << std::endl;
    }

    // Vérifier si le serveur peut accepter de nouveaux joueurs
    if (!canAcceptNewPlayer()) {
        std::cout << "[Server] Rejecting player: server is full" << std::endl;
        sendServerReject(rawPacket.sender, protocol::RejectCodes::REJECT_SERVER_FULL, 
                        "Server is full");
        return;
    }

    // Vérifier la version du protocole
    if (connectPacket->protocol_version != 1) {
        std::cout << "[Server] Rejecting player: incompatible protocol version" << std::endl;
        sendServerReject(rawPacket.sender, 
                        protocol::RejectCodes::REJECT_INCOMPATIBLE_PROTOCOL_VERSION,
                        "Incompatible protocol version");
        return;
    }

    // Créer une session pour ce client
    auto session = m_sessionLayer->createSession(
        rawPacket.sender,
        connectPacket->client_id,
        playerName
    );

    if (!session) {
        std::cerr << "[Server] Failed to create session" << std::endl;
        sendServerReject(rawPacket.sender, protocol::RejectCodes::REJECT_GENERIC_ERROR,
                        "Failed to create session");
        return;
    }

    // Envoyer SERVER_ACCEPT
    if (sendServerAccept(rawPacket.sender, session->getPlayerId())) {
        std::cout << "[Server] Player '" << playerName << "' (ID: " 
                  << session->getPlayerId() << ") connected successfully!" << std::endl;
        std::cout << "[Server] Active players: " << m_sessionLayer->getSessionCount() 
                  << "/" << m_config.maxPlayers << std::endl;
    }
}

void Server::handleClientDisconnect(const network::RawPacket& rawPacket) {
    if (rawPacket.data.size() < sizeof(protocol::ClientDisconnect)) {
        return;
    }

    const protocol::ClientDisconnect* disconnectPacket = 
        reinterpret_cast<const protocol::ClientDisconnect*>(rawPacket.data.data());

    auto session = m_sessionLayer->getSession(rawPacket.sender);
    if (!session) {
        return;
    }

    if (m_config.enableLogging) {
        std::cout << "[Server] Player " << session->getPlayerId() 
                  << " disconnected (reason: " << static_cast<int>(disconnectPacket->reason) 
                  << ")" << std::endl;
    }

    m_sessionLayer->removeSession(rawPacket.sender);

    std::cout << "[Server] Active players: " << m_sessionLayer->getSessionCount() 
              << "/" << m_config.maxPlayers << std::endl;
}

void Server::handleHeartbeat(const network::RawPacket& rawPacket) {
    if (rawPacket.data.size() < sizeof(protocol::HeartBeat)) {
        return;
    }

    auto session = m_sessionLayer->getSession(rawPacket.sender);
    if (!session) {
        if (m_config.enableLogging) {
            std::cout << "[Server] Heartbeat from unknown client " 
                      << rawPacket.sender.toString() << std::endl;
        }
        return;
    }

    // Mettre à jour le timestamp du dernier heartbeat
    session->updateLastHeartbeat();
    session->recordPacketReceived(rawPacket.data.size());

    // Pour l'instant, on ne répond pas aux heartbeats
    // Plus tard, on pourrait envoyer des PONG ou des stats
}

void Server::handlePlayerInput(const network::RawPacket& rawPacket) {
    // TODO: Cette fonction sera implémentée dans l'étape 3 (gérer une partie)
    if (rawPacket.data.size() < sizeof(protocol::PlayerInput)) {
        return;
    }

    auto session = m_sessionLayer->getSession(rawPacket.sender);
    if (!session) {
        return;
    }

    session->recordPacketReceived(rawPacket.data.size());

    // Pour l'instant, on ne fait rien avec les inputs
    // Ils seront traités par la GameLogic Layer plus tard
}

bool Server::sendServerAccept(const network::ClientAddress& address, uint32_t playerId) {
    protocol::ServerAccept acceptPacket{};
    acceptPacket.header.magic = 0x5254;
    acceptPacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT);
    acceptPacket.header.flags = 0;
    acceptPacket.header.sequence_number = 0;
    acceptPacket.header.timestamp = 0;

    acceptPacket.assigned_player_id = playerId;
    acceptPacket.max_players = m_config.maxPlayers;
    acceptPacket.game_instance_id = 0;  // Pour l'instant, pas de game instance
    acceptPacket.server_tickrate = m_config.tickRate;

    return sendPacket(&acceptPacket, sizeof(acceptPacket), address);
}

bool Server::sendServerReject(const network::ClientAddress& address, 
                              protocol::RejectCodes reason,
                              const std::string& message) {
    protocol::ServerReject rejectPacket{};
    rejectPacket.header.magic = 0x5254;
    rejectPacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_REJECT);
    rejectPacket.header.flags = 0;
    rejectPacket.header.sequence_number = 0;
    rejectPacket.header.timestamp = 0;

    rejectPacket.reject_code = static_cast<uint8_t>(reason);
    
    // Copier le message (limité à 63 caractères + null terminator)
    std::strncpy(rejectPacket.reason_message, message.c_str(), 63);
    rejectPacket.reason_message[63] = '\0';

    return sendPacket(&rejectPacket, sizeof(rejectPacket), address);
}

bool Server::sendPacket(const void* data, size_t size, const network::ClientAddress& destination) {
    int bytesSent = m_networkLayer->sendPacket(
        static_cast<const uint8_t*>(data),
        size,
        destination
    );

    if (bytesSent < 0) {
        std::cerr << "[Server] Failed to send packet to " << destination.toString() << std::endl;
        return false;
    }

    // Mettre à jour les stats de la session si elle existe
    auto session = m_sessionLayer->getSession(destination);
    if (session) {
        session->recordPacketSent(size);
    }

    return true;
}

bool Server::validatePacketHeader(const protocol::PacketHeader* header, size_t packetSize) const {
    // Vérifier le magic number
    if (header->magic != 0x5254) {
        if (m_config.enableLogging) {
            std::cerr << "[Server] Invalid magic number: 0x" 
                      << std::hex << header->magic << std::dec << std::endl;
        }
        return false;
    }

    // Vérifier que le packet type est dans une plage valide
    if (header->packet_type < 0x01 || header->packet_type > 0x7F) {
        if (m_config.enableLogging) {
            std::cerr << "[Server] Invalid packet type: 0x" 
                      << std::hex << static_cast<int>(header->packet_type) << std::dec << std::endl;
        }
        return false;
    }

    return true;
}

bool Server::canAcceptNewPlayer() const {
    return m_sessionLayer->getSessionCount() < m_config.maxPlayers;
}

void Server::heartbeatThreadFunction() {
    std::cout << "[Server] Heartbeat monitoring thread started" << std::endl;

    while (m_running.load()) {
        // Attendre 1 seconde
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Vérifier les timeouts
        size_t timedOut = m_sessionLayer->checkTimeouts();

        if (timedOut > 0) {
            std::cout << "[Server] Removed " << timedOut << " timed out sessions" << std::endl;
            std::cout << "[Server] Active players: " << m_sessionLayer->getSessionCount() 
                      << "/" << m_config.maxPlayers << std::endl;
        }
    }

    std::cout << "[Server] Heartbeat monitoring thread stopped" << std::endl;
}

void Server::printStatistics() const {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);

    std::cout << "\n=== Server Statistics ===" << std::endl;
    std::cout << "Uptime: " << uptime.count() << " seconds" << std::endl;
    std::cout << "Total packets processed: " << m_totalPacketsProcessed.load() << std::endl;
    std::cout << "Invalid packets: " << m_invalidPackets.load() << std::endl;
    std::cout << "Active sessions: " << m_sessionLayer->getSessionCount() << std::endl;

    // Afficher les sessions actives
    auto sessions = m_sessionLayer->getAllSessions();
    if (!sessions.empty()) {
        std::cout << "\nActive Players:" << std::endl;
        for (const auto& session : sessions) {
            auto duration = session->getConnectionDuration();
            std::cout << "  - Player " << session->getPlayerId() 
                      << " (" << session->getPlayerName() << ")"
                      << " from " << session->getAddress().toString()
                      << " - Connected for " << (duration.count() / 1000) << "s"
                      << " - Packets: RX=" << session->getPacketsReceived()
                      << " TX=" << session->getPacketsSent()
                      << std::endl;
        }
    }

    std::cout << "========================\n" << std::endl;
}

} // namespace server
