#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include "network/Network.hpp"
#include "session/Session.hpp"
#include "protocol/Protocol.hpp"

namespace server {

    /**
     * @brief Configuration du serveur
     */
    struct ServerConfig {
        uint16_t port = 4242;
        uint32_t maxPlayers = 16;
        uint32_t tickRate = 60;  // Hz
        std::chrono::milliseconds heartbeatTimeout = std::chrono::milliseconds(10000);
        bool enableLogging = true;
    };

    /**
     * @brief Serveur principal R-Type
     * Orchestre les différentes couches (Network, Protocol, Session, Game Logic)
     */
    class Server {
    public:
        explicit Server(const ServerConfig& config = ServerConfig{});
        ~Server();

        // Empêcher la copie
        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;

        /**
         * @brief Initialise le serveur
         * @return true si succès, false sinon
         */
        bool initialize();

        /**
         * @brief Démarre le serveur et toutes ses couches
         * @return true si succès, false sinon
         */
        bool start();

        /**
         * @brief Arrête le serveur
         */
        void stop();

        /**
         * @brief Boucle principale du serveur (bloquante)
         * Traite les paquets et met à jour les systèmes
         */
        void run();

        /**
         * @brief Vérifie si le serveur est en cours d'exécution
         */
        bool isRunning() const { return m_running.load(); }

        /**
         * @brief Récupère les statistiques du serveur
         */
        void printStatistics() const;

    private:
        // Configuration
        ServerConfig m_config;

        // Layers
        std::unique_ptr<network::UDPSocketManager> m_networkLayer;
        std::unique_ptr<session::ConnectionManager> m_sessionLayer;

        // State
        std::atomic<bool> m_running;
        std::atomic<bool> m_initialized;

        // Threads
        std::unique_ptr<std::thread> m_heartbeatThread;
        void heartbeatThreadFunction();

        // Packet Processing (Protocol Layer)
        void processPackets();
        void processPacket(const network::RawPacket& rawPacket);
        
        // Packet Handlers
        void handleClientConnect(const network::RawPacket& rawPacket);
        void handleClientDisconnect(const network::RawPacket& rawPacket);
        void handleHeartbeat(const network::RawPacket& rawPacket);
        void handlePlayerInput(const network::RawPacket& rawPacket);
        
        // Sending helpers
        bool sendServerAccept(const network::ClientAddress& address, uint32_t playerId);
        bool sendServerReject(const network::ClientAddress& address, 
                             protocol::RejectCodes reason, 
                             const std::string& message);
        bool sendPacket(const void* data, size_t size, const network::ClientAddress& destination);

        // Validation
        bool validatePacketHeader(const protocol::PacketHeader* header, size_t packetSize) const;
        bool canAcceptNewPlayer() const;

        // Statistics
        std::atomic<uint64_t> m_totalPacketsProcessed;
        std::atomic<uint64_t> m_invalidPackets;
        std::chrono::steady_clock::time_point m_startTime;
    };

} // namespace server

#endif // SERVER_HPP_
