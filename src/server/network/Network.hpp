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

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

namespace network {

    constexpr size_t MAX_PACKET_SIZE = 1200;  // Safe UDP size
    constexpr size_t RECEIVE_BUFFER_SIZE = 2048;

    /**
     * @brief Structure représentant une adresse client (IP + Port)
     */
    struct ClientAddress {
        sockaddr_in addr;
        
        ClientAddress() {
            std::memset(&addr, 0, sizeof(addr));
        }
        
        explicit ClientAddress(const sockaddr_in& address) : addr(address) {}
        
        std::string toString() const {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
            return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
        }
        
        bool operator==(const ClientAddress& other) const {
            return addr.sin_addr.s_addr == other.addr.sin_addr.s_addr &&
                   addr.sin_port == other.addr.sin_port;
        }
        
        bool operator<(const ClientAddress& other) const {
            if (addr.sin_addr.s_addr != other.addr.sin_addr.s_addr)
                return addr.sin_addr.s_addr < other.addr.sin_addr.s_addr;
            return addr.sin_port < other.addr.sin_port;
        }
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
     * Gère le socket UDP unique, les threads de réception/envoi
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
        SOCKET m_socket;
        sockaddr_in m_serverAddr;
        std::atomic<bool> m_running;
        std::atomic<bool> m_initialized;

        // Thread de réception
        std::unique_ptr<std::thread> m_receiveThread;
        void receiveThreadFunction();

        // Queue de paquets reçus (lock-free serait idéal, mais pour MVP on utilise mutex)
        std::queue<RawPacket> m_receiveQueue;
        std::mutex m_receiveMutex;

        // Statistics
        std::atomic<uint64_t> m_packetsReceived;
        std::atomic<uint64_t> m_packetsSent;
        std::atomic<uint64_t> m_bytesReceived;
        std::atomic<uint64_t> m_bytesSent;

        // Helpers
        bool initializeSocket();
        void cleanupSocket();
        uint32_t getCurrentTimestamp() const;
    };

} // namespace network

// Hash function for ClientAddress (pour utiliser dans std::unordered_map)
namespace std {
    template<>
    struct hash<network::ClientAddress> {
        size_t operator()(const network::ClientAddress& addr) const {
            size_t h1 = std::hash<uint32_t>{}(addr.addr.sin_addr.s_addr);
            size_t h2 = std::hash<uint16_t>{}(addr.addr.sin_port);
            return h1 ^ (h2 << 1);
        }
    };
}

#endif // NETWORK_HPP_
