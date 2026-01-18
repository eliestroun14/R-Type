#include <gtest/gtest.h>

#include <common/network/sockets/ASocket.hpp>

namespace {

class TestSocket : public common::network::ASocket {
public:
    bool connect(const std::string&, uint16_t) override { return true; }
    bool bind(uint16_t port) override {
        _localPort = port;
        return true;
    }
    bool send(const common::protocol::Packet&) override { return true; }
    bool receive(common::protocol::Packet&) override { return false; }
    void close() override { setConnectionState(false); }
    bool hasData() const override { return false; }

    void setErrorPublic(const std::string& error) { setError(error); }
    bool validatePacketPublic(const common::protocol::Packet& packet) const {
        return validatePacket(packet);
    }
    void setConnectionStatePublic(bool connected) { setConnectionState(connected); }
    void setTimeoutPublic(uint32_t timeoutMs) { setTimeout(timeoutMs); }
    uint32_t getTimeoutPublic() const { return _timeout; }
    bool getNonBlockingPublic() const { return _nonBlocking.load(); }
};

} // namespace

TEST(ASocketCoverage, DefaultStateAndSetters)
{
    TestSocket socket;

    EXPECT_FALSE(socket.isConnected());
    EXPECT_EQ(socket.getLocalPort(), 0u);
    EXPECT_EQ(socket.getRemotePort(), 0u);
    EXPECT_TRUE(socket.getRemoteAddress().empty());

    socket.setNonBlocking(false);
    EXPECT_FALSE(socket.getNonBlockingPublic());

    socket.setTimeoutPublic(1234u);
    EXPECT_EQ(socket.getTimeoutPublic(), 1234u);
}

TEST(ASocketCoverage, ErrorStateAndConnection)
{
    TestSocket socket;

    socket.setErrorPublic("oops");
    EXPECT_EQ(socket.getLastError(), "oops");

    socket.setConnectionStatePublic(true);
    EXPECT_TRUE(socket.isConnected());

    socket.close();
    EXPECT_FALSE(socket.isConnected());
}

TEST(ASocketCoverage, ValidatePacketSize)
{
    TestSocket socket;
    common::protocol::Packet packet;

    packet.data.resize(65507);
    EXPECT_TRUE(socket.validatePacketPublic(packet));

    packet.data.resize(65508);
    EXPECT_FALSE(socket.validatePacketPublic(packet));
}
