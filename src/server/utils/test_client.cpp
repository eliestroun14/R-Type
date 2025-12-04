/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Simple test client to verify server connection handling
*/

#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include "../protocol/Protocol.hpp"

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
    #define closesocket close
    typedef int SOCKET;
#endif

class SimpleTestClient {
public:
    SimpleTestClient() : m_socket(INVALID_SOCKET), m_connected(false) {}
    
    ~SimpleTestClient() {
        disconnect();
    }
    
    bool connect(const std::string& serverIp, uint16_t serverPort, const std::string& playerName) {
        // Initialize socket
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "[TestClient] WSAStartup failed" << std::endl;
            return false;
        }
#endif
        
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET) {
            std::cerr << "[TestClient] Failed to create socket" << std::endl;
            return false;
        }
        
        // Set timeout
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        
        // Setup server address
        std::memset(&m_serverAddr, 0, sizeof(m_serverAddr));
        m_serverAddr.sin_family = AF_INET;
        m_serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverIp.c_str(), &m_serverAddr.sin_addr);
        
        // Send CLIENT_CONNECT packet
        protocol::ClientConnect connectPacket{};
        connectPacket.header.magic = 0x5254;
        connectPacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_CONNECT);
        connectPacket.header.flags = 0;
        connectPacket.header.sequence_number = 0;
        connectPacket.header.timestamp = 0;
        connectPacket.protocol_version = 1;
        connectPacket.client_id = 12345;  // Random client ID
        std::strncpy(connectPacket.player_name, playerName.c_str(), 31);
        connectPacket.player_name[31] = '\0';
        
        std::cout << "[TestClient] Sending connection request to " << serverIp << ":" << serverPort << std::endl;
        
        int bytesSent = sendto(m_socket, 
                               reinterpret_cast<const char*>(&connectPacket),
                               sizeof(connectPacket),
                               0,
                               (struct sockaddr*)&m_serverAddr,
                               sizeof(m_serverAddr));
        
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "[TestClient] Failed to send connection request" << std::endl;
            return false;
        }
        
        std::cout << "[TestClient] Connection request sent (" << bytesSent << " bytes)" << std::endl;
        
        // Wait for response
        uint8_t buffer[1024];
        sockaddr_in fromAddr;
        socklen_t fromLen = sizeof(fromAddr);
        
        std::cout << "[TestClient] Waiting for server response..." << std::endl;
        
        int bytesReceived = recvfrom(m_socket,
                                     reinterpret_cast<char*>(buffer),
                                     sizeof(buffer),
                                     0,
                                     (struct sockaddr*)&fromAddr,
                                     &fromLen);
        
        if (bytesReceived <= 0) {
            std::cerr << "[TestClient] No response from server (timeout)" << std::endl;
            return false;
        }
        
        std::cout << "[TestClient] Received response (" << bytesReceived << " bytes)" << std::endl;
        
        // Parse response
        const protocol::PacketHeader* header = reinterpret_cast<const protocol::PacketHeader*>(buffer);
        
        if (header->magic != 0x5254) {
            std::cerr << "[TestClient] Invalid magic number in response" << std::endl;
            return false;
        }
        
        if (header->packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_ACCEPT)) {
            const protocol::ServerAccept* acceptPacket = reinterpret_cast<const protocol::ServerAccept*>(buffer);
            m_playerId = acceptPacket->assigned_player_id;
            m_connected = true;
            
            std::cout << "[TestClient] ✓ Connection ACCEPTED!" << std::endl;
            std::cout << "[TestClient]   Assigned Player ID: " << m_playerId << std::endl;
            std::cout << "[TestClient]   Max Players: " << static_cast<int>(acceptPacket->max_players) << std::endl;
            std::cout << "[TestClient]   Server Tickrate: " << acceptPacket->server_tickrate << " Hz" << std::endl;
            return true;
        }
        else if (header->packet_type == static_cast<uint8_t>(protocol::PacketTypes::TYPE_SERVER_REJECT)) {
            const protocol::ServerReject* rejectPacket = reinterpret_cast<const protocol::ServerReject*>(buffer);
            
            std::cout << "[TestClient] ✗ Connection REJECTED!" << std::endl;
            std::cout << "[TestClient]   Reason code: " << static_cast<int>(rejectPacket->reject_code) << std::endl;
            std::cout << "[TestClient]   Message: " << rejectPacket->reason_message << std::endl;
            return false;
        }
        
        std::cerr << "[TestClient] Unknown response type: 0x" << std::hex 
                  << static_cast<int>(header->packet_type) << std::dec << std::endl;
        return false;
    }
    
    void sendHeartbeats(int count = 5) {
        if (!m_connected) {
            std::cerr << "[TestClient] Not connected, cannot send heartbeats" << std::endl;
            return;
        }
        
        std::cout << "[TestClient] Sending " << count << " heartbeats..." << std::endl;
        
        for (int i = 0; i < count; i++) {
            protocol::HeartBeat heartbeat{};
            heartbeat.header.magic = 0x5254;
            heartbeat.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_HEARTBEAT);
            heartbeat.header.flags = 0;
            heartbeat.header.sequence_number = i;
            heartbeat.header.timestamp = 0;
            heartbeat.player_id = m_playerId;
            
            sendto(m_socket,
                   reinterpret_cast<const char*>(&heartbeat),
                   sizeof(heartbeat),
                   0,
                   (struct sockaddr*)&m_serverAddr,
                   sizeof(m_serverAddr));
            
            std::cout << "[TestClient] Heartbeat " << (i + 1) << "/" << count << " sent" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void disconnect() {
        if (!m_connected) {
            return;
        }
        
        std::cout << "[TestClient] Disconnecting..." << std::endl;
        
        // Send disconnect packet
        protocol::ClientDisconnect disconnectPacket{};
        disconnectPacket.header.magic = 0x5254;
        disconnectPacket.header.packet_type = static_cast<uint8_t>(protocol::PacketTypes::TYPE_CLIENT_DISCONNECT);
        disconnectPacket.header.flags = static_cast<uint8_t>(protocol::PacketFlags::FLAG_RELIABLE);
        disconnectPacket.header.sequence_number = 0;
        disconnectPacket.header.timestamp = 0;
        disconnectPacket.player_id = m_playerId;
        disconnectPacket.reason = static_cast<uint8_t>(protocol::DisconnectReasons::REASON_NORMAL_DISCONNECT);
        
        sendto(m_socket,
               reinterpret_cast<const char*>(&disconnectPacket),
               sizeof(disconnectPacket),
               0,
               (struct sockaddr*)&m_serverAddr,
               sizeof(m_serverAddr));
        
        if (m_socket != INVALID_SOCKET) {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        
#ifdef _WIN32
        WSACleanup();
#endif
        
        m_connected = false;
        std::cout << "[TestClient] Disconnected" << std::endl;
    }
    
private:
    SOCKET m_socket;
    sockaddr_in m_serverAddr;
    bool m_connected;
    uint32_t m_playerId;
};

int main(int argc, char** argv) {
    std::string serverIp = "127.0.0.1";
    uint16_t serverPort = 4242;
    std::string playerName = "TestPlayer";
    
    // Parse arguments
    if (argc > 1) {
        serverIp = argv[1];
    }
    if (argc > 2) {
        serverPort = static_cast<uint16_t>(std::stoi(argv[2]));
    }
    if (argc > 3) {
        playerName = argv[3];
    }
    
    std::cout << "=== R-Type Test Client ===" << std::endl;
    std::cout << "Server: " << serverIp << ":" << serverPort << std::endl;
    std::cout << "Player: " << playerName << std::endl;
    std::cout << std::endl;
    
    SimpleTestClient client;
    
    if (client.connect(serverIp, serverPort, playerName)) {
        // Send some heartbeats
        client.sendHeartbeats(5);
        
        // Disconnect cleanly
        client.disconnect();
        
        std::cout << "\n✓ Test completed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Test failed!" << std::endl;
        return 1;
    }
}
