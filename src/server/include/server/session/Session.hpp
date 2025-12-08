#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <cstdint>
#include <string>
#include <memory>
#include <chrono>
#include <queue>
#include <map>
#include "../network/Network.hpp"
#include "../../../../common/include/common/protocol/Protocol.hpp"

namespace session {

    /**
     * @brief État d'une session client
     */
    enum class SessionState {
        CONNECTING,      // En cours de connexion
        CONNECTED,       // Connecté et authentifié
        IN_LOBBY,        // Dans un lobby
        IN_GAME,         // En jeu
        DISCONNECTING,   // En cours de déconnexion
        DISCONNECTED     // Déconnecté
    };

    /**
     * @brief Représente une session client individuelle
     */
    class ClientSession {
    public:
        ClientSession(const network::ClientAddress& address, uint32_t clientId);
        ~ClientSession() = default;

        // Getters
        uint32_t getPlayerId() const { return m_playerId; }
        uint32_t getClientId() const { return m_clientId; }
        const network::ClientAddress& getAddress() const { return m_address; }
        SessionState getState() const { return m_state; }
        const std::string& getPlayerName() const { return m_playerName; }
        uint32_t getSequenceNumber() const { return m_sequenceNumber; }

        // Setters
        void setPlayerId(uint32_t id) { m_playerId = id; }
        void setState(SessionState state) { m_state = state; }
        void setPlayerName(const std::string& name) { m_playerName = name; }
        void setGameInstanceId(uint32_t id) { m_gameInstanceId = id; }

        // Sequence management
        void incrementSequence() { m_sequenceNumber++; }
        bool isSequenceValid(uint32_t receivedSeq) const;

        // Heartbeat management
        void updateLastHeartbeat();
        std::chrono::milliseconds getTimeSinceLastHeartbeat() const;
        bool isTimedOut(std::chrono::milliseconds timeout) const;

        // Statistics
        void recordPacketReceived(size_t bytes);
        void recordPacketSent(size_t bytes);
        uint64_t getPacketsReceived() const { return m_packetsReceived; }
        uint64_t getPacketsSent() const { return m_packetsSent; }

        // Connection time
        std::chrono::milliseconds getConnectionDuration() const;

    private:
        // Identity
        uint32_t m_playerId;           // Assigned by server
        uint32_t m_clientId;           // From client's CLIENT_CONNECT
        network::ClientAddress m_address;
        std::string m_playerName;

        // State
        SessionState m_state;
        uint32_t m_gameInstanceId;     // Game instance this player belongs to

        // Protocol
        uint32_t m_sequenceNumber;

        // Timing
        std::chrono::steady_clock::time_point m_connectedAt;
        std::chrono::steady_clock::time_point m_lastHeartbeat;

        // Statistics
        uint64_t m_packetsReceived;
        uint64_t m_packetsSent;
        uint64_t m_bytesReceived;
        uint64_t m_bytesSent;
    };

    /**
     * @brief Gestionnaire de connexions (Layer 3 - Session Layer)
     * Gère toutes les sessions clients actives
     */
    class ConnectionManager {
    public:
        ConnectionManager();
        ~ConnectionManager();

        // Empêcher la copie
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;

        /**
         * @brief Démarre le gestionnaire de connexions
         */
        void start();

        /**
         * @brief Arrête le gestionnaire
         */
        void stop();

        /**
         * @brief Crée une nouvelle session pour un client
         * @param address Adresse du client
         * @param clientId ID client depuis CLIENT_CONNECT
         * @param playerName Nom du joueur
         * @return Pointeur vers la session créée, nullptr si échec
         */
        std::shared_ptr<ClientSession> createSession(
            const network::ClientAddress& address,
            uint32_t clientId,
            const std::string& playerName
        );

        /**
         * @brief Récupère une session par adresse client
         */
        std::shared_ptr<ClientSession> getSession(const network::ClientAddress& address);

        /**
         * @brief Récupère une session par player ID
         */
        std::shared_ptr<ClientSession> getSessionByPlayerId(uint32_t playerId);

        /**
         * @brief Supprime une session
         */
        void removeSession(const network::ClientAddress& address);
        void removeSessionByPlayerId(uint32_t playerId);

        /**
         * @brief Vérifie si une adresse a déjà une session active
         */
        bool hasSession(const network::ClientAddress& address) const;

        /**
         * @brief Récupère toutes les sessions actives
         */
        std::vector<std::shared_ptr<ClientSession>> getAllSessions() const;

        /**
         * @brief Récupère le nombre de sessions actives
         */
        size_t getSessionCount() const;

        /**
         * @brief Vérifie les timeouts et supprime les sessions expirées
         * @return Nombre de sessions supprimées
         */
        size_t checkTimeouts();

        /**
         * @brief Génère un nouveau player ID unique
         */
        uint32_t generatePlayerId();

        // Configuration
        void setHeartbeatTimeout(std::chrono::milliseconds timeout) { m_heartbeatTimeout = timeout; }
        std::chrono::milliseconds getHeartbeatTimeout() const { return m_heartbeatTimeout; }

    private:
        // Sessions map: address -> session
        std::map<network::ClientAddress, std::shared_ptr<ClientSession>> m_sessions;

        // Player ID map: player_id -> session (pour lookup rapide)
        std::map<uint32_t, std::shared_ptr<ClientSession>> m_playerSessions;

        // Player ID counter
        std::atomic<uint32_t> m_nextPlayerId;

        // Configuration
        std::chrono::milliseconds m_heartbeatTimeout;

        // Thread safety
        mutable std::mutex m_sessionsMutex;

        // State
        bool m_running;
    };

} // namespace session

#endif // SESSION_HPP_
