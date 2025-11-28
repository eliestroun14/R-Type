# Google Test Setup Tutorial

## Step 1: Install Google Test
On Fedora:
```bash
sudo dnf install gtest gtest-devel gmock gmock-devel
```

On Ubuntu/Debian:
```bash
sudo apt-get install libgtest-dev
```

## Step 2: Create Test Directory Structure
If a `tests/` folder already exists at the root of the project, you can **go to next step**.

If not, then do this:
```bash
mkdir -p tests
touch tests/CMakeLists.txt
```

## Step 3: Configure Root `CMakeLists.txt`
If the `CMakeLists.txt` looks like this, **you may update it** to set new `target_include_directories`.
If not, modify it to look like this:
```c++
cmake_minimum_required(VERSION 3.10.0)
project(R-Type VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for main executable
set(SOURCES
    "src/main.cpp"
)

# Create main executable
add_executable(R-Type ${SOURCES})

# Include directories for the main target
target_include_directories(R-Type PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/gameEngine
    ${CMAKE_SOURCE_DIR}/src/gameEngine/ECS
    // Add new target include directory HERE
)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)
```

## Step 4: Create `tests/CMakeLists.txt`
If a `CMakeLists.txt` file already exists in `tests/` folder, **you may update it** to set new source files and new target include directories.

If not, create it like this:
```c++
# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include your source directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/gameEngine)
include_directories(${CMAKE_SOURCE_DIR}/src/gameEngine/ECS)
// Add new target include directory HERE

# Create test executable for ECS
add_executable(ecs_tests
    // your tests files
    // example: test_game_engine.cpp
)

# Link Google Test and your libraries
target_link_libraries(ecs_tests
    GTest::GTest
    GTest::Main
    pthread
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)
```

## Step 5: Create Your First Test File
At the root of the project do this:
```bash
touch tests/test_file_name.cpp
```

### Example
This example is based on `test_game_engine.cpp`

```c++
#include <gtest/gtest.h>
#include "GameEngine.hpp"

struct Transform {
    float x, y, rotation;
    Transform(float xx, float yy, float rot)
        : x(xx), y(yy), rotation(rot) {}
};

class GameEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine.init();
        engine.registerComponent<Transform>();
    }

    GameEngine engine;
};

TEST_F(GameEngineTest, createEntity) {
    Entity e = engine.createEntity("TestEntity");
    EXPECT_TRUE(engine.isAlive(e));
    EXPECT_EQ(engine.getEntityName(e), "TestEntity");
}

TEST_F(GameEngineTest, destroyEntity) {
    Entity e = engine.createEntity("TestEntity");
    engine.destroyEntity(e);
    EXPECT_FALSE(engine.isAlive(e));
}

TEST_F(GameEngineTest, AddAndAccessComponent) {
    Entity e = engine.createEntity("TestEntity");
    engine.emplaceComponent<Transform>(e, 10.f, 20.f, 45.f);

    auto& transform = engine.getComponentEntity<Transform>(e);
    ASSERT_TRUE(transform.has_value());
    EXPECT_FLOAT_EQ(transform->x, 10.f);
    EXPECT_FLOAT_EQ(transform->y, 20.f);
    EXPECT_FLOAT_EQ(transform->rotation, 45.f);
}

TEST_F(GameEngineTest, removeComponent) {
    Entity e = engine.createEntity("TestEntity");
    engine.emplaceComponent<Transform>(e, 10.f, 20.f, 0.f);
    engine.removeComponent<Transform>(e);

    auto& transform = engine.getComponentEntity<Transform>(e);
    EXPECT_FALSE(transform.has_value());
}

TEST_F(GameEngineTest, IterateComponents) {
    Entity e1 = engine.createEntity("A");
    Entity e2 = engine.createEntity("B");
    Entity e3 = engine.createEntity("C");

    engine.emplaceComponent<Transform>(e1, 1.f, 2.f, 0.f);
    engine.emplaceComponent<Transform>(e2, 3.f, 4.f, 0.f);
    engine.emplaceComponent<Transform>(e3, 5.f, 6.f, 0.f);

    auto& transforms = engine.getComponents<Transform>();

    int count = 0;
    for (size_t id = 0; id < transforms.size(); ++id) {
        if (transforms[id].has_value()) {
            count++;
        }
    }

    EXPECT_EQ(count, 3);
}
```

## Step 7: Build and Run Tests
Compile and launch your tests:
```bash
# Create build folder
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Compile
make

# Launch all your tests
ctest --output-on-failure

# Or launch directly with the executable test
./tests/ecs_tests

# Launch specifics tests
./tests/ecs_tests --gtest_filter="GameManagerTest.*"

# Launch with more details
./tests/ecs_tests --gtest_color=yes
```

**Congratulations, you made tests** !!
--------------------------------------

