#pragma once

#include <vector>
#include <cstdint>
#include <cstring>

namespace common {
namespace protocol {

class Serialization {
public:
    // Serialize primitives
    template<typename T>
    static void write(std::vector<uint8_t>& buffer, const T& value) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
    }

    template<typename T>
    static bool read(const std::vector<uint8_t>& buffer, size_t& offset, T& value) {
        if (offset + sizeof(T) > buffer.size()) {
            return false;
        }
        std::memcpy(&value, buffer.data() + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }

    // Serialize strings
    static void writeString(std::vector<uint8_t>& buffer, const std::string& str);
    static bool readString(const std::vector<uint8_t>& buffer, size_t& offset, std::string& str);
};

} // namespace protocol
} // namespace common
