#include "../include/server/network/Network.hpp"
#include <iostream>
#include <chrono>

namespace network {

UDPSocketManager::UDPSocketManager()
    : m_running(false)
    , m_initialized(false)
    , m_packetsReceived(0)
    , m_packetsSent(0)
    , m_bytesReceived(0)
    , m_bytesSent(0)
{
}

UDPSocketManager::~UDPSocketManager() {
    stop();
}

bool UDPSocketManager::initialize(uint16_t port) {
    if (m_initialized.load()) {
        std::cerr << "[Network] Already initialized" << std::endl;
        return false;
    }

    try {
        // Créer le contexte ASIO
        m_ioContext = std::make_unique<asio::io_context>();

        // Créer le socket UDP
        m_socket = std::make_unique<asio::ip::udp::socket>(
            *m_ioContext,
            asio::ip::udp::endpoint(asio::ip::udp::v4(), port)
        );

        std::cout << "[Network] Socket initialized and bound to port " << port << std::endl;
        m_initialized = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[Network] Failed to initialize: " << e.what() << std::endl;
        return false;
    }
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

    // Démarrer la réception asynchrone
    startReceive();

    // Démarrer le thread qui fait tourner io_context
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

    // Arrêter le io_context
    if (m_ioContext) {
        m_ioContext->stop();
    }

    // Fermer le socket
    if (m_socket && m_socket->is_open()) {
        asio::error_code ec;
        m_socket->close(ec);
    }

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

    try {
        size_t bytesSent = m_socket->send_to(
            asio::buffer(data, size),
            destination.getEndpoint()
        );

        m_packetsSent++;
        m_bytesSent += bytesSent;

        return static_cast<int>(bytesSent);

    } catch (const std::exception& e) {
        std::cerr << "[Network] send_to failed: " << e.what() << std::endl;
        return -1;
    }
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
    std::lock_guard<std::mutex> lock(m_receiveMutex);
    return m_receiveQueue.size();
}

void UDPSocketManager::receiveThreadFunction() {
    std::cout << "[Network] Receive thread started (ASIO io_context)" << std::endl;

    try {
        // Faire tourner le io_context (bloquant jusqu'à stop())
        m_ioContext->run();
    } catch (const std::exception& e) {
        std::cerr << "[Network] Exception in receive thread: " << e.what() << std::endl;
    }

    std::cout << "[Network] Receive thread stopped" << std::endl;
}

void UDPSocketManager::startReceive() {
    m_socket->async_receive_from(
        asio::buffer(m_receiveBuffer),
        m_remoteEndpoint,
        [this](const asio::error_code& error, size_t bytesReceived) {
            handleReceive(error, bytesReceived);
        }
    );
}

void UDPSocketManager::handleReceive(const asio::error_code& error, size_t bytesReceived) {
    if (!error && bytesReceived > 0 && m_running.load()) {
        // Créer un RawPacket et l'ajouter à la queue
        ClientAddress sender(m_remoteEndpoint);
        RawPacket packet(m_receiveBuffer.data(), bytesReceived, sender);
        packet.timestamp = getCurrentTimestamp();

        {
            std::lock_guard<std::mutex> lock(m_receiveMutex);
            m_receiveQueue.push(std::move(packet));
        }

        m_packetsReceived++;
        m_bytesReceived += bytesReceived;
    } else if (error && error != asio::error::operation_aborted) {
        std::cerr << "[Network] Receive error: " << error.message() << std::endl;
    }

    // Continuer à recevoir (tant que le serveur tourne)
    if (m_running.load()) {
        startReceive();
    }
}

uint32_t UDPSocketManager::getCurrentTimestamp() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

} // namespace network
