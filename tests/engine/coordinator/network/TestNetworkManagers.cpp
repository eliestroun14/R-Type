/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Network manager smoke tests
*/

#include <gtest/gtest.h>
#include <client/network/ClientNetworkManager.hpp>
#include <server/network/ServerNetworkManager.hpp>
#include <common/protocol/Protocol.hpp>
#include <common/protocol/Packet.hpp>

TEST(NetworkManagers, ClientQueueLifecycle)
{
    client::network::ClientNetworkManager manager("127.0.0.1", 4242);
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));

    manager.queueOutgoing(packet);
    auto incoming = manager.fetchIncoming();

    EXPECT_TRUE(incoming.empty());
    manager.stop();
}

TEST(NetworkManagers, ServerQueueLifecycle)
{
    server::network::ServerNetworkManager manager(4242, 2);
    common::protocol::Packet packet(static_cast<uint8_t>(protocol::PacketTypes::TYPE_ENTITY_SPAWN));

    manager.queueOutgoing(packet);
    auto incoming = manager.fetchIncoming();

    EXPECT_TRUE(incoming.empty());
    manager.stop();
}
