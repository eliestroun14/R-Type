# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include your source directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Create test executable for ECS
add_executable(ecs_tests

   engine/TestEntity.cpp
   engine/TestComponentManager.cpp
   engine/TestEntityManager.cpp
   engine/TestCoordinator.cpp
   engine/TestSystemManager.cpp
   engine/LoggerTest.cpp
   engine/coordinator/network/TestNetworkManagerAssert.cpp
   engine/coordinator/network/TestProcessPacket.cpp
   common/TestPacket.cpp

   TestDebug.cpp
    Error_messages_test.cpp
)

# Link Google Test and your libraries
target_link_libraries(ecs_tests
    GTest::GTest
    GTest::Main
    pthread
    engine
    common
    spdlog::spdlog
)

# Apply coverage flags if enabled
if(ENABLE_COVERAGE)
    target_compile_options(ecs_tests PRIVATE --coverage -O0)
    target_link_options(ecs_tests PRIVATE --coverage)
endif()

# Discover tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)

# Coverage targets (only if lcov is available)
if(ENABLE_COVERAGE)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    
    if(LCOV AND GENHTML)
        # List of patterns to exclude from coverage (external libraries)
        set(COVERAGE_EXCLUDES
            '/usr/*'
            '${CMAKE_SOURCE_DIR}/tests/*'
            '${CMAKE_BINARY_DIR}/_deps/*'
        )
        
        # Build exclude options for lcov
        set(EXCLUDE_OPTIONS "")
        foreach(PATTERN ${COVERAGE_EXCLUDES})
            list(APPEND EXCLUDE_OPTIONS "--exclude" "${PATTERN}")
        endforeach()
        
        add_custom_target(coverage
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info ${EXCLUDE_OPTIONS} --ignore-errors unused
            COMMAND ${GENHTML} coverage.info --output-directory coverage_report --ignore-errors unused
            COMMAND echo "Coverage report generated in: coverage_report/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report..."
        )
        
        add_custom_target(coverage_clean
            COMMAND ${LCOV} --directory . --zerocounters
            COMMAND find . -name '*.gcda' -delete
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Cleaning coverage data..."
        )
    endif()
endif()
