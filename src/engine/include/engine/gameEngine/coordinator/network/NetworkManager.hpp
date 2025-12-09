/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** NetworkManager
*/

#ifndef NETWORKMANAGER_HPP_
#define NETWORKMANAGER_HPP_

#include <cstdint>
#include <vector>
#include <map>
#include <optional>
#include <array>
#include "../../../../../common/include/common/protocol/Protocol.hpp"
#include "../../../../../common/include/common/protocol/Packet.hpp"

/**
 * @class NetworkManager
 * @brief Manages network packet processing and creation for the ECS
 *
 * The NetworkManager is responsible for:
 * - Processing (deserializing and validating) incoming network packets
 * - Creating (building and serializing) outgoing network packets
 *
 * It uses a compile-time optimized handler table (constexpr std::array)
 * to dispatch packet types to their respective handlers with minimal overhead.
 */
class NetworkManager {
    public:
        // ==============================================================
        //                          Initialization
        // ==============================================================

        NetworkManager() = default;
        ~NetworkManager() = default;

        // ==============================================================
        //                        Packet Processing
        // ==============================================================

        /**
         * @brief Process a received network packet (deserialize and validate)
         * @param packet The packet to process
         * @return The deserialized and validated packet, or std::nullopt if validation failed
         */
        std::optional<common::protocol::Packet> processPacket(const common::protocol::Packet &packet);

        /**
         * @brief Create a network packet to send
         * @param type The packet type to create
         * @param args Additional arguments for packet creation
         * @return The created packet
         */
        common::protocol::Packet createPacket(protocol::PacketTypes type, const std::vector<uint8_t> &args = {});

    private:

        // ==============================================================
        //                  Assertion Functions
        // ==============================================================

        /**
         * @brief Validate an ENTITY_SPAWN packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertEntitySpawn(const common::protocol::Packet &packet);

        /**
         * @brief Validate a WORLD_SNAPSHOT packet
         * @param packet The packet to validate
         * @return true if packet data is valid, false otherwise
         */
        static bool assertWorldSnapshot(const common::protocol::Packet &packet);

        // ==============================================================
        //                  Creation Functions
        // ==============================================================

        /**
         * @brief Create an ENTITY_SPAWN packet
         * @param args Arguments for packet creation
         * @return The created packet
         */
        static common::protocol::Packet createEntitySpawn(const std::vector<uint8_t> &args);

        /**
         * @brief Create a WORLD_SNAPSHOT packet
         * @param args Arguments for packet creation
         * @return The created packet
         */
        static common::protocol::Packet createWorldSnapshot(const std::vector<uint8_t> &args);

        // ==============================================================
        //                  Packet Handler Structure
        // ==============================================================

        /**
         * @struct PacketHandler
         * @brief Associates a packet type with its validation and creation functions
         *
         * Used by the handlers table to map packet types to their respective
         * assertion (validation) and creation (serialization) functions.
         */
        struct PacketHandler {
            protocol::PacketTypes type;
            bool (*assertFunc)(const common::protocol::Packet &);
            common::protocol::Packet (*createFunc)(const std::vector<uint8_t> &);
        };

        /**
         * @brief Static handler table for all supported packet types
         *
         * This table maps each supported packet type to its assertion and creation functions.
         * It is initialized at compile-time (constexpr) for optimal performance and memory usage.
         * Each entry contains:
         * - The packet type identifier
         * - A pointer to the assertion function (validates packet data)
         * - A pointer to the creation function (builds/serializes packets)
         *
         * @details Exemple handlers:
         * - TYPE_ENTITY_SPAWN: Handles entity spawn events
         * - TYPE_WORLD_SNAPSHOT: Handles world state synchronization
         *
         * To add new packet types, simply add a new entry to this array with
         * the corresponding assertion and creation functions.
         */
        static constexpr std::array<PacketHandler, 2> handlers = {{
            { protocol::PacketTypes::TYPE_ENTITY_SPAWN, &NetworkManager::assertEntitySpawn, &NetworkManager::createEntitySpawn },
            { protocol::PacketTypes::TYPE_WORLD_SNAPSHOT, &NetworkManager::assertWorldSnapshot, &NetworkManager::createWorldSnapshot }
        }};

        /**
         * @brief Find a packet handler by type
         * @param type The packet type to find
         * @return Pointer to the handler, or nullptr if not found
         */
        static PacketHandler *findHandler(protocol::PacketTypes type);
};

#endif /* !NETWORKMANAGER_HPP_ */
