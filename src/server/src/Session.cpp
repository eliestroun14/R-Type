#include "../include/server/session/Session.hpp"
#include <iostream>

namespace session {

// ============================================================================
// ClientSession Implementation
// ============================================================================

ClientSession::ClientSession(const network::ClientAddress& address, uint32_t clientId)
    : m_playerId(0)
    , m_clientId(clientId)
    , m_address(address)
    , m_playerName("")
    , m_state(SessionState::CONNECTING)
    , m_gameInstanceId(0)
    , m_sequenceNumber(0)
    , m_connectedAt(std::chrono::steady_clock::now())
    , m_lastHeartbeat(std::chrono::steady_clock::now())
    , m_packetsReceived(0)
    , m_packetsSent(0)
    , m_bytesReceived(0)
    , m_bytesSent(0)
{
}

bool ClientSession::isSequenceValid(uint32_t receivedSeq) const {
    // Pour l'instant, on accepte toutes les séquences
    // TODO: Implémenter une vraie validation de séquence (avec fenêtre)
    return true;
}

void ClientSession::updateLastHeartbeat() {
    m_lastHeartbeat = std::chrono::steady_clock::now();
}

std::chrono::milliseconds ClientSession::getTimeSinceLastHeartbeat() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastHeartbeat);
}

bool ClientSession::isTimedOut(std::chrono::milliseconds timeout) const {
    return getTimeSinceLastHeartbeat() > timeout;
}

void ClientSession::recordPacketReceived(size_t bytes) {
    m_packetsReceived++;
    m_bytesReceived += bytes;
}

void ClientSession::recordPacketSent(size_t bytes) {
    m_packetsSent++;
    m_bytesSent += bytes;
}

std::chrono::milliseconds ClientSession::getConnectionDuration() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_connectedAt);
}

// ============================================================================
// ConnectionManager Implementation
// ============================================================================

ConnectionManager::ConnectionManager()
    : m_nextPlayerId(1000)  // Commencer à 1000 pour éviter confusion avec 0
    , m_heartbeatTimeout(std::chrono::milliseconds(10000))  // 10 secondes par défaut
    , m_running(false)
{
}

ConnectionManager::~ConnectionManager() {
    stop();
}

void ConnectionManager::start() {
    if (m_running) {
        return;
    }
    
    m_running = true;
    std::cout << "[ConnectionManager] Started" << std::endl;
}

void ConnectionManager::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;

    // Nettoyer toutes les sessions
    {
        std::lock_guard<std::mutex> lock(m_sessionsMutex);
        m_sessions.clear();
        m_playerSessions.clear();
    }

    std::cout << "[ConnectionManager] Stopped" << std::endl;
}

std::shared_ptr<ClientSession> ConnectionManager::createSession(
    const network::ClientAddress& address,
    uint32_t clientId,
    const std::string& playerName)
{
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    // Vérifier si une session existe déjà pour cette adresse
    auto it = m_sessions.find(address);
    if (it != m_sessions.end()) {
        std::cout << "[ConnectionManager] Session already exists for " << address.toString() << std::endl;
        return it->second;
    }

    // Créer la nouvelle session
    auto session = std::make_shared<ClientSession>(address, clientId);
    session->setPlayerId(generatePlayerId());
    session->setPlayerName(playerName);
    session->setState(SessionState::CONNECTED);

    // Ajouter aux maps
    m_sessions[address] = session;
    m_playerSessions[session->getPlayerId()] = session;

    std::cout << "[ConnectionManager] Created session for player '" << playerName 
              << "' (ID: " << session->getPlayerId() << ") from " << address.toString() << std::endl;

    return session;
}

std::shared_ptr<ClientSession> ConnectionManager::getSession(const network::ClientAddress& address) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    auto it = m_sessions.find(address);
    if (it != m_sessions.end()) {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<ClientSession> ConnectionManager::getSessionByPlayerId(uint32_t playerId) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    auto it = m_playerSessions.find(playerId);
    if (it != m_playerSessions.end()) {
        return it->second;
    }

    return nullptr;
}

void ConnectionManager::removeSession(const network::ClientAddress& address) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    auto it = m_sessions.find(address);
    if (it != m_sessions.end()) {
        uint32_t playerId = it->second->getPlayerId();

        std::cout << "[ConnectionManager] Removing session for player ID " << playerId 
                  << " from " << address.toString() << std::endl;

        // Supprimer des deux maps
        m_playerSessions.erase(playerId);
        m_sessions.erase(it);
    }
}

void ConnectionManager::removeSessionByPlayerId(uint32_t playerId) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    auto it = m_playerSessions.find(playerId);
    if (it != m_playerSessions.end()) {
        network::ClientAddress address = it->second->getAddress();

        std::cout << "[ConnectionManager] Removing session for player ID " << playerId << std::endl;

        // Supprimer des deux maps
        m_sessions.erase(address);
        m_playerSessions.erase(it);
    }
}

bool ConnectionManager::hasSession(const network::ClientAddress& address) const {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    return m_sessions.find(address) != m_sessions.end();
}

std::vector<std::shared_ptr<ClientSession>> ConnectionManager::getAllSessions() const {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    std::vector<std::shared_ptr<ClientSession>> sessions;
    sessions.reserve(m_sessions.size());

    for (const auto& pair : m_sessions) {
        sessions.push_back(pair.second);
    }

    return sessions;
}

size_t ConnectionManager::getSessionCount() const {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    return m_sessions.size();
}

size_t ConnectionManager::checkTimeouts() {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    std::vector<network::ClientAddress> toRemove;

    for (const auto& pair : m_sessions) {
        if (pair.second->isTimedOut(m_heartbeatTimeout)) {
            std::cout << "[ConnectionManager] Player " << pair.second->getPlayerId() 
                      << " timed out" << std::endl;
            toRemove.push_back(pair.first);
        }
    }

    // Supprimer les sessions expirées
    for (const auto& address : toRemove) {
        auto it = m_sessions.find(address);
        if (it != m_sessions.end()) {
            m_playerSessions.erase(it->second->getPlayerId());
            m_sessions.erase(it);
        }
    }

    return toRemove.size();
}

uint32_t ConnectionManager::generatePlayerId() {
    return m_nextPlayerId.fetch_add(1);
}

} // namespace session
