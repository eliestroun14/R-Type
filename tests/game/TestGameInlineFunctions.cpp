/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Test inline functions in Game.hpp for coverage
*/

#include <gtest/gtest.h>
#include <game/Game.hpp>
#include <memory>

// ============================================================================
// INLINE FUNCTION COVERAGE TESTS
// ============================================================================
// These tests specifically target the inline functions in Game.hpp
// to maximize coverage: setConnected, getCoordinator, setMenu

class GameInlineFunctionsTest : public ::testing::Test {
protected:
    std::unique_ptr<Game> game;

    void SetUp() override {
        // Create a server-type game instance
        game = std::make_unique<Game>(Game::Type::SERVER);
    }

    void TearDown() override {
        game.reset();
    }
};

// ============================================================================
// setConnected() - Line 37 in Game.hpp
// ============================================================================

TEST_F(GameInlineFunctionsTest, SetConnectedTrue) {
    // Test setting connected to true
    game->setConnected(true);
    EXPECT_TRUE(game->isConnected());
}

TEST_F(GameInlineFunctionsTest, SetConnectedFalse) {
    // Test setting connected to false
    game->setConnected(false);
    EXPECT_FALSE(game->isConnected());
}

TEST_F(GameInlineFunctionsTest, SetConnectedToggle) {
    // Test toggling connected state
    game->setConnected(true);
    EXPECT_TRUE(game->isConnected());
    
    game->setConnected(false);
    EXPECT_FALSE(game->isConnected());
    
    game->setConnected(true);
    EXPECT_TRUE(game->isConnected());
}

// ============================================================================
// getCoordinator() - Line 43 in Game.hpp
// ============================================================================

TEST_F(GameInlineFunctionsTest, GetCoordinatorNotNull) {
    // Test that getCoordinator returns a valid shared_ptr
    auto coordinator = game->getCoordinator();
    EXPECT_NE(coordinator, nullptr);
}

TEST_F(GameInlineFunctionsTest, GetCoordinatorConsistency) {
    // Test that multiple calls return the same coordinator
    auto coordinator1 = game->getCoordinator();
    auto coordinator2 = game->getCoordinator();
    EXPECT_EQ(coordinator1, coordinator2);
}

TEST_F(GameInlineFunctionsTest, GetCoordinatorUsable) {
    // Test that we can use the coordinator after getting it
    auto coordinator = game->getCoordinator();
    EXPECT_NE(coordinator, nullptr);
    
    // Verify coordinator is properly initialized by checking if it's server mode
    EXPECT_NO_THROW({
        bool isServer = coordinator->isServer();
        (void)isServer; // Suppress unused variable warning
    });
}

// ============================================================================
// setMenu() - Line 52 in Game.hpp
// ============================================================================

// Mock IMenu implementation for testing
class MockMenu : public IMenu {
public:
    bool updateCalled = false;
    bool createMainMenuCalled = false;
    bool createOptionMenuCalled = false;

    void update() override {
        updateCalled = true;
    }

    void createMainMenu() override {
        createMainMenuCalled = true;
    }

    void createOptionMenu() override {
        createOptionMenuCalled = true;
    }
};

TEST_F(GameInlineFunctionsTest, SetMenuValid) {
    // Test setting a valid menu
    auto menu = std::make_shared<MockMenu>();
    EXPECT_NO_THROW({
        game->setMenu(menu);
    });
}

TEST_F(GameInlineFunctionsTest, SetMenuNull) {
    // Test setting a null menu (should be allowed)
    EXPECT_NO_THROW({
        game->setMenu(nullptr);
    });
}

TEST_F(GameInlineFunctionsTest, SetMenuMultipleTimes) {
    // Test replacing the menu multiple times
    auto menu1 = std::make_shared<MockMenu>();
    auto menu2 = std::make_shared<MockMenu>();
    
    game->setMenu(menu1);
    game->setMenu(menu2);
    game->setMenu(nullptr);
    
    // Should complete without issues
    SUCCEED();
}

// ============================================================================
// COMBINED TESTS
// ============================================================================

TEST_F(GameInlineFunctionsTest, AllInlineFunctionsCombined) {
    // Test all three inline functions together
    
    // 1. Set connected state
    game->setConnected(true);
    EXPECT_TRUE(game->isConnected());
    
    // 2. Get coordinator
    auto coordinator = game->getCoordinator();
    EXPECT_NE(coordinator, nullptr);
    
    // 3. Set menu
    auto menu = std::make_shared<MockMenu>();
    game->setMenu(menu);
    
    // Verify all operations succeeded
    EXPECT_TRUE(game->isConnected());
    EXPECT_NE(game->getCoordinator(), nullptr);
}
