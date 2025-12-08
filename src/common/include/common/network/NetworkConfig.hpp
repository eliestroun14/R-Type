#pragma once

#include <string>
#include <cstdint>

namespace common {
namespace network {

struct NetworkConfig {
    std::string serverAddress = "127.0.0.1";
    uint16_t serverPort = 8080;
    uint32_t maxPacketSize = 1400;
    uint32_t sendBufferSize = 65536;
    uint32_t receiveBufferSize = 65536;
    bool useCompression = false;
    bool useEncryption = false;
};

} // namespace network
} // namespace common
