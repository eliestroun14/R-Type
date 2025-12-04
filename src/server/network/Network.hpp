#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <asio.hpp>

namespace network {

    constexpr size_t MAX_PACKET_SIZE = 1200;  // Safe UDP size
    constexpr size_t RECEIVE_BUFFER_SIZE = 2048;

    /**
     * @brief Structure représentant une adresse client (IP + Port)
     * Utilise asio::ip::udp::endpoint en interne
     */
    class ClientAddress {
    public:
        ClientAddress() = default;
        
        explicit ClientAddress(const asio::ip::udp::endpoint& ep) : m_endpoint(ep) {}
        
        std::string toString() const {
            return m_endpoint.address().to_string() + ":" + std::to_string(m_endpoint.port());
        }
        
        const asio::ip::udp::endpoint& getEndpoint() const { return m_endpoint; }
        asio::ip::udp::endpoint& getEndpoint() { return m_endpoint; }
        
        bool operator==(const ClientAddress& other) const {
            return m_endpoint == other.m_endpoint;
        }
        
        bool operator<(const ClientAddress& other) const {
            if (m_endpoint.address() != other.m_endpoint.address())
                return m_endpoint.address() < other.m_endpoint.address();
            return m_endpoint.port() < other.m_endpoint.port();
        }

    private:
        asio::ip::udp::endpoint m_endpoint;
    };

    /**
     * @brief Structure représentant un paquet brut reçu
     */
    struct RawPacket {
        std::vector<uint8_t> data;
        ClientAddress sender;
        uint32_t timestamp;  // Milliseconds
        
        RawPacket() : timestamp(0) {}
        
        RawPacket(const uint8_t* buffer, size_t size, const ClientAddress& from)
            : data(buffer, buffer + size), sender(from), timestamp(0) {}
    };

    /**
     * @brief UDP Socket Manager - Layer 1 de l'architecture
     * Gère le socket UDP unique avec ASIO (cross-platform)
     */
    class UDPSocketManager {
    public:
        UDPSocketManager();
        ~UDPSocketManager();

        // Empêcher la copie
        UDPSocketManager(const UDPSocketManager&) = delete;
        UDPSocketManager& operator=(const UDPSocketManager&) = delete;

        /**
         * @brief Initialise et bind le socket sur le port spécifié
         * @param port Port d'écoute (par défaut 4242)
         * @return true si succès, false sinon
         */
        bool initialize(uint16_t port = 4242);

        /**
         * @brief Démarre les threads de réception et d'envoi
         * @return true si succès, false sinon
         */
        bool start();

        /**
         * @brief Arrête les threads et ferme le socket
         */
        void stop();

        /**
         * @brief Envoie un paquet à une adresse client
         * @param data Données à envoyer
         * @param size Taille des données
         * @param destination Adresse de destination
         * @return Nombre d'octets envoyés, -1 si erreur
         */
        int sendPacket(const uint8_t* data, size_t size, const ClientAddress& destination);

        /**
         * @brief Récupère le prochain paquet reçu (thread-safe)
         * @param packet Packet à remplir
         * @return true si un paquet était disponible, false sinon
         */
        bool getNextPacket(RawPacket& packet);

        /**
         * @brief Vérifie si le socket est actif
         */
        bool isRunning() const { return m_running.load(); }

        /**
         * @brief Récupère le nombre de paquets en attente
         */
        size_t getPendingPacketCount() const;

    private:
        // ASIO context and socket
        std::unique_ptr<asio::io_context> m_ioContext;
        std::unique_ptr<asio::ip::udp::socket> m_socket;
        
        std::atomic<bool> m_running;
        std::atomic<bool> m_initialized;

        // Thread de réception
        std::unique_ptr<std::thread> m_receiveThread;
        void receiveThreadFunction();
        
        // Buffer pour la réception asynchrone
        std::array<uint8_t, RECEIVE_BUFFER_SIZE> m_receiveBuffer;
        asio::ip::udp::endpoint m_remoteEndpoint;
        
        // Démarrage de la réception asynchrone
        void startReceive();
        void handleReceive(const asio::error_code& error, size_t bytesReceived);

        // Queue de paquets reçus (thread-safe)
        std::queue<RawPacket> m_receiveQueue;
        mutable std::mutex m_receiveMutex;

        // Statistics
        std::atomic<uint64_t> m_packetsReceived;
        std::atomic<uint64_t> m_packetsSent;
        std::atomic<uint64_t> m_bytesReceived;
        std::atomic<uint64_t> m_bytesSent;

        // Helpers
        uint32_t getCurrentTimestamp() const;
    };

} // namespace network

// Hash function for ClientAddress (pour utiliser dans std::unordered_map)
namespace std {
    template<>
    struct hash<network::ClientAddress> {
        size_t operator()(const network::ClientAddress& addr) const {
            const auto& ep = addr.getEndpoint();
            size_t h1 = std::hash<std::string>{}(ep.address().to_string());
            size_t h2 = std::hash<uint16_t>{}(ep.port());
            return h1 ^ (h2 << 1);
        }
    };
}

#endif // NETWORK_HPP_
