#include <gtest/gtest.h>

#include <common/network/sockets/AsioSocket.hpp>
#include <common/protocol/Packet.hpp>

TEST(AsioSocketCoverage, ConnectSendReceiveClose)
{
    common::network::AsioSocket socket;
    common::protocol::Packet packet(static_cast<uint8_t>(1));

    bool connected = socket.connect("127.0.0.1", 4242);
    if (!connected) {
        EXPECT_FALSE(socket.isConnected());
        EXPECT_FALSE(socket.getLastError().empty());
        return;
    }

    EXPECT_TRUE(socket.isConnected());

    bool sent = socket.send(packet);
    if (!sent) {
        EXPECT_FALSE(socket.getLastError().empty());
    }

    common::protocol::Packet received;
    EXPECT_FALSE(socket.receive(received));

    socket.close();
    EXPECT_FALSE(socket.isConnected());
}

TEST(AsioSocketCoverage, BindAndSocketOptions)
{
    common::network::AsioSocket socket;

    socket.setNonBlocking(false);
    bool bound = socket.bind(0);
    if (!bound) {
        EXPECT_FALSE(socket.getLastError().empty());
    }

    socket.setBufferSizes(8192, 16384);
    socket.setBroadcast(true);
    socket.setMaxPacketSize(1024);

    socket.close();
    EXPECT_FALSE(socket.isConnected());
}

TEST(AsioSocketCoverage, ReceiveFromAndSendToFailures)
{
    common::network::AsioSocket socket;
    common::protocol::Packet packet(static_cast<uint8_t>(1));

    socket.bind(0);

    std::string remote;
    EXPECT_FALSE(socket.receiveFrom(packet, remote));

    bool sent = socket.sendTo(packet, "invalid_address");
    EXPECT_FALSE(sent);
    EXPECT_FALSE(socket.getLastError().empty());

    socket.close();
}
