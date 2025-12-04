#include "Network.hpp"
#include <iostream>
#include <cstring>
#include <chrono>

namespace network {

UDPSocketManager::UDPSocketManager()
    : m_socket(INVALID_SOCKET)
    , m_running(false)
    , m_initialized(false)
    , m_packetsReceived(0)
    , m_packetsSent(0)
    , m_bytesReceived(0)
    , m_bytesSent(0)
{
    std::memset(&m_serverAddr, 0, sizeof(m_serverAddr));
}

UDPSocketManager::~UDPSocketManager() {
    stop();
    cleanupSocket();
}

bool UDPSocketManager::initialize(uint16_t port) {
    if (m_initialized.load()) {
        std::cerr << "[Network] Already initialized" << std::endl;
        return false;
    }

#ifdef _WIN32
    // Initialize Winsock on Windows
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "[Network] WSAStartup failed: " << result << std::endl;
        return false;
    }
#endif

    if (!initializeSocket()) {
        cleanupSocket();
        return false;
    }

    // Bind socket
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(port);

    if (bind(m_socket, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[Network] Failed to bind socket on port " << port << std::endl;
        cleanupSocket();
        return false;
    }

    std::cout << "[Network] Socket initialized and bound to port " << port << std::endl;
    m_initialized = true;
    return true;
}

bool UDPSocketManager::start() {
    if (!m_initialized.load()) {
        std::cerr << "[Network] Cannot start: not initialized" << std::endl;
        return false;
    }

    if (m_running.load()) {
        std::cerr << "[Network] Already running" << std::endl;
        return false;
    }

    m_running = true;

    // Démarrer le thread de réception
    m_receiveThread = std::make_unique<std::thread>(&UDPSocketManager::receiveThreadFunction, this);

    std::cout << "[Network] Socket manager started" << std::endl;
    return true;
}

void UDPSocketManager::stop() {
    if (!m_running.load()) {
        return;
    }

    std::cout << "[Network] Stopping socket manager..." << std::endl;
    m_running = false;

    // Attendre la fin du thread de réception
    if (m_receiveThread && m_receiveThread->joinable()) {
        m_receiveThread->join();
    }

    std::cout << "[Network] Socket manager stopped" << std::endl;
    std::cout << "[Network] Statistics:" << std::endl;
    std::cout << "  - Packets received: " << m_packetsReceived.load() << std::endl;
    std::cout << "  - Packets sent: " << m_packetsSent.load() << std::endl;
    std::cout << "  - Bytes received: " << m_bytesReceived.load() << std::endl;
    std::cout << "  - Bytes sent: " << m_bytesSent.load() << std::endl;
}

int UDPSocketManager::sendPacket(const uint8_t* data, size_t size, const ClientAddress& destination) {
    if (!m_running.load() || size > MAX_PACKET_SIZE) {
        return -1;
    }

    int bytesSent = sendto(
        m_socket,
        reinterpret_cast<const char*>(data),
        static_cast<int>(size),
        0,
        (struct sockaddr*)&destination.addr,
        sizeof(destination.addr)
    );

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "[Network] sendto failed" << std::endl;
        return -1;
    }

    m_packetsSent++;
    m_bytesSent += bytesSent;

    return bytesSent;
}

bool UDPSocketManager::getNextPacket(RawPacket& packet) {
    std::lock_guard<std::mutex> lock(m_receiveMutex);
    
    if (m_receiveQueue.empty()) {
        return false;
    }

    packet = std::move(m_receiveQueue.front());
    m_receiveQueue.pop();
    return true;
}

size_t UDPSocketManager::getPendingPacketCount() const {
    return m_receiveQueue.size();
}

void UDPSocketManager::receiveThreadFunction() {
    std::cout << "[Network] Receive thread started" << std::endl;

    uint8_t buffer[RECEIVE_BUFFER_SIZE];
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (m_running.load()) {
        std::memset(&clientAddr, 0, sizeof(clientAddr));
        clientAddrLen = sizeof(clientAddr);

        // Réception bloquante avec timeout (via setsockopt SO_RCVTIMEO)
        int bytesReceived = recvfrom(
            m_socket,
            reinterpret_cast<char*>(buffer),
            RECEIVE_BUFFER_SIZE,
            0,
            (struct sockaddr*)&clientAddr,
            &clientAddrLen
        );

        if (bytesReceived == SOCKET_ERROR) {
#ifdef _WIN32
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT || error == WSAEINTR) {
                continue;  // Timeout ou interruption, continuer
            }
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;  // Timeout ou interruption, continuer
            }
#endif
            if (m_running.load()) {
                std::cerr << "[Network] recvfrom error" << std::endl;
            }
            break;
        }

        if (bytesReceived > 0) {
            // Créer un RawPacket et l'ajouter à la queue
            ClientAddress sender(clientAddr);
            RawPacket packet(buffer, bytesReceived, sender);
            packet.timestamp = getCurrentTimestamp();

            {
                std::lock_guard<std::mutex> lock(m_receiveMutex);
                m_receiveQueue.push(std::move(packet));
            }

            m_packetsReceived++;
            m_bytesReceived += bytesReceived;
        }
    }

    std::cout << "[Network] Receive thread stopped" << std::endl;
}

bool UDPSocketManager::initializeSocket() {
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[Network] Failed to create socket" << std::endl;
        return false;
    }

    // Set socket to non-blocking mode with timeout
#ifdef _WIN32
    DWORD timeout = 100;  // 100ms timeout
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;  // 100ms timeout
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif

    // Allow address reuse
    int reuse = 1;
#ifdef _WIN32
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
#else
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

    return true;
}

void UDPSocketManager::cleanupSocket() {
    if (m_socket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_socket);
        WSACleanup();
#else
        close(m_socket);
#endif
        m_socket = INVALID_SOCKET;
    }
    m_initialized = false;
}

uint32_t UDPSocketManager::getCurrentTimestamp() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

} // namespace network
