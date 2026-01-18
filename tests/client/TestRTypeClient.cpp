/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TestRTypeClient
*/

#include <gtest/gtest.h>
#include <client/RTypeClient.hpp>
#include <memory>

class RTypeClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        client = std::make_unique<RTypeClient>();
    }

    void TearDown() override {
        client.reset();
    }

    std::unique_ptr<RTypeClient> client;
};

// Test getPlayerName() - line 42
TEST_F(RTypeClientTest, GetPlayerName) {
    client->setPlayerName("TestPlayer");
    EXPECT_EQ(client->getPlayerName(), "TestPlayer");
}

// Test getPlayerName() with default value - line 42
TEST_F(RTypeClientTest, GetPlayerNameDefault) {
    EXPECT_EQ(client->getPlayerName(), "");
}

// Test getPlayerName() with different names - line 42
TEST_F(RTypeClientTest, GetPlayerNameMultiple) {
    client->setPlayerName("Player1");
    EXPECT_EQ(client->getPlayerName(), "Player1");
    
    client->setPlayerName("Player2");
    EXPECT_EQ(client->getPlayerName(), "Player2");
    
    client->setPlayerName("Player3");
    EXPECT_EQ(client->getPlayerName(), "Player3");
}

// Test setConnected() true - line 44
TEST_F(RTypeClientTest, SetConnectedTrue) {
    client->setConnected(true);
    EXPECT_TRUE(client->isConnected());
}

// Test setConnected() false - line 44
TEST_F(RTypeClientTest, SetConnectedFalse) {
    client->setConnected(true);
    client->setConnected(false);
    EXPECT_FALSE(client->isConnected());
}

// Test setConnected() multiple times - line 44
TEST_F(RTypeClientTest, SetConnectedMultiple) {
    client->setConnected(true);
    EXPECT_TRUE(client->isConnected());
    
    client->setConnected(false);
    EXPECT_FALSE(client->isConnected());
    
    client->setConnected(true);
    EXPECT_TRUE(client->isConnected());
}

// Test setConnected() when _game is nullptr - line 44
TEST_F(RTypeClientTest, SetConnectedWithoutGame) {
    // This should not crash even if _game is nullptr
    EXPECT_NO_THROW({
        client->setConnected(true);
        EXPECT_TRUE(client->isConnected());
        client->setConnected(false);
        EXPECT_FALSE(client->isConnected());
    });
}

// Test getPlayerName after setConnected - line 42 and 44
TEST_F(RTypeClientTest, GetPlayerNameAfterSetConnected) {
    client->setPlayerName("NetworkPlayer");
    client->setConnected(true);
    
    EXPECT_EQ(client->getPlayerName(), "NetworkPlayer");
    EXPECT_TRUE(client->isConnected());
}
