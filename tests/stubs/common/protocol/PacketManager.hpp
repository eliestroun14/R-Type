/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Stub PacketManager for Game.cpp coverage tests
*/

#ifndef TESTS_STUB_PACKETMANAGER_HPP_
#define TESTS_STUB_PACKETMANAGER_HPP_

#include <optional>
#include <vector>

#include <common/constants/defines.hpp>
#include <common/protocol/Packet.hpp>
#include <common/protocol/Protocol.hpp>

class PacketManager {
public:
    static std::optional<common::protocol::Packet> createEntitySpawn(const std::vector<uint8_t>&) {
        common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));
        packet.data.resize(ENTITY_SPAWN_PAYLOAD_SIZE, 0);
        return packet;
    }
};

#endif /* TESTS_STUB_PACKETMANAGER_HPP_ */
