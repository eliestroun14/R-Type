/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Standalone test for Game.hpp inline functions
*/

#include <iostream>
#include <memory>
#include <game/Game.hpp>

// Simple IMenu mock
class TestMenu : public IMenu {
public:
    void update() override {}
    void createMainMenu() override {}
    void createOptionMenu() override {}
};

int main() {
    int testsPassed = 0;
    int totalTests = 0;
    
    std::cout << "=== Testing Game.hpp Inline Functions ===" << std::endl;
    
    try {
        // Create a Game instance
        auto game = std::make_unique<Game>(Game::Type::SERVER);
        std::cout << "✓ Game instance created" << std::endl;
        
        // Test 1: setConnected()
        totalTests++;
        game->setConnected(true);
        if (game->isConnected()) {
            std::cout << "✓ Test 1: setConnected(true) works" << std::endl;
            testsPassed++;
        } else {
            std::cout << "✗ Test 1: setConnected(true) FAILED" << std::endl;
        }
        
        // Test 2: setConnected(false)
        totalTests++;
        game->setConnected(false);
        if (!game->isConnected()) {
            std::cout << "✓ Test 2: setConnected(false) works" << std::endl;
            testsPassed++;
        } else {
            std::cout << "✗ Test 2: setConnected(false) FAILED" << std::endl;
        }
        
        // Test 3: getCoordinator()
        totalTests++;
        auto coordinator = game->getCoordinator();
        if (coordinator != nullptr) {
            std::cout << "✓ Test 3: getCoordinator() returns non-null" << std::endl;
            testsPassed++;
        } else {
            std::cout << "✗ Test 3: getCoordinator() FAILED" << std::endl;
        }
        
        // Test 4: getCoordinator() consistency
        totalTests++;
        auto coordinator2 = game->getCoordinator();
        if (coordinator == coordinator2) {
            std::cout << "✓ Test 4: getCoordinator() returns same instance" << std::endl;
            testsPassed++;
        } else {
            std::cout << "✗ Test 4: getCoordinator() consistency FAILED" << std::endl;
        }
        
        // Test 5: setMenu()
        totalTests++;
        auto menu = std::make_shared<TestMenu>();
        game->setMenu(menu);
        std::cout << "✓ Test 5: setMenu() executes without error" << std::endl;
        testsPassed++;
        
        // Test 6: setMenu(nullptr)
        totalTests++;
        game->setMenu(nullptr);
        std::cout << "✓ Test 6: setMenu(nullptr) executes without error" << std::endl;
        testsPassed++;
        
        // Test 7: Multiple game types
        totalTests++;
        auto clientGame = std::make_unique<Game>(Game::Type::CLIENT);
        clientGame->setConnected(true);
        auto coord = clientGame->getCoordinator();
        if (coord != nullptr && clientGame->isConnected()) {
            std::cout << "✓ Test 7: CLIENT game type works with inline functions" << std::endl;
            testsPassed++;
        } else {
            std::cout << "✗ Test 7: CLIENT game type FAILED" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "✗ Exception caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Tests passed: " << testsPassed << "/" << totalTests << std::endl;
    
    if (testsPassed == totalTests) {
        std::cout << "✓ All inline function tests passed!" << std::endl;
        std::cout << "\nThis means Game.hpp lines 37, 43, and 52 are being executed and will show coverage." << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some tests failed" << std::endl;
        return 1;
    }
}
