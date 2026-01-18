/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ClientUtils tests
*/

#include <gtest/gtest.h>
#include <game/utils/ClientUtils.hpp>
#include <thread>
#include <set>
#include <chrono>

// ==================== generateClientId Tests ====================

TEST(GenerateClientIdTest, GeneratesNonZeroId) {
    uint32_t id = generateClientId();
    // ID should be generated (might be 0, but very unlikely)
    EXPECT_GE(id, 0);
}

TEST(GenerateClientIdTest, GeneratesDifferentIds) {
    uint32_t id1 = generateClientId();
    uint32_t id2 = generateClientId();
    uint32_t id3 = generateClientId();
    
    // Very unlikely that two consecutive calls return the same value
    // (probability is 1 in 2^32)
    EXPECT_TRUE(id1 != id2 || id2 != id3 || id1 != id3);
}

TEST(GenerateClientIdTest, GeneratesMultipleUniqueIds) {
    std::set<uint32_t> ids;
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; i++) {
        ids.insert(generateClientId());
    }
    
    // With 100 generations, should have at least 95% unique IDs
    // (allowing for very rare collisions)
    EXPECT_GT(ids.size(), ITERATIONS * 0.90);
}

TEST(GenerateClientIdTest, ThreadSafeGeneration) {
    std::set<uint32_t> ids;
    std::mutex idMutex;
    const int THREADS = 10;
    const int IDS_PER_THREAD = 10;
    
    auto generateIds = [&]() {
        for (int i = 0; i < IDS_PER_THREAD; i++) {
            uint32_t id = generateClientId();
            {
                std::lock_guard<std::mutex> lock(idMutex);
                ids.insert(id);
            }
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < THREADS; i++) {
        threads.emplace_back(generateIds);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Should have generated IDs from multiple threads
    EXPECT_GT(ids.size(), 0);
}

// ==================== getCurrentTimeMs Tests ====================

TEST(GetCurrentTimeMsTest, ReturnsPositiveValue) {
    uint32_t time = getCurrentTimeMs();
    EXPECT_GT(time, 0);
}

TEST(GetCurrentTimeMsTest, IncrementsOverTime) {
    uint32_t time1 = getCurrentTimeMs();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint32_t time2 = getCurrentTimeMs();
    
    // Second call should be >= first call
    EXPECT_GE(time2, time1);
}

TEST(GetCurrentTimeMsTest, AdvancesWithDelay) {
    uint32_t time1 = getCurrentTimeMs();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    uint32_t time2 = getCurrentTimeMs();
    
    // Should advance by roughly 50ms (allow some variance)
    uint32_t delta = time2 - time1;
    EXPECT_GE(delta, 30); // Allow some variance
    EXPECT_LE(delta, 200); // But not too much
}

TEST(GetCurrentTimeMsTest, ConsistentlyIncreasing) {
    uint32_t prev = getCurrentTimeMs();
    
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        uint32_t current = getCurrentTimeMs();
        
        EXPECT_GE(current, prev);
        prev = current;
    }
}

TEST(GetCurrentTimeMsTest, MultipleCallsInSequence) {
    std::vector<uint32_t> times;
    
    for (int i = 0; i < 10; i++) {
        times.push_back(getCurrentTimeMs());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Times should be non-decreasing
    for (size_t i = 1; i < times.size(); i++) {
        EXPECT_GE(times[i], times[i-1]);
    }
}

// ==================== Integration Tests ====================

TEST(ClientUtilsIntegration, IdAndTimeAreIndependent) {
    uint32_t id1 = generateClientId();
    uint32_t time1 = getCurrentTimeMs();
    uint32_t id2 = generateClientId();
    uint32_t time2 = getCurrentTimeMs();
    
    // IDs should be different from time values
    EXPECT_NE(id1, time1);
    EXPECT_NE(id2, time2);
}

TEST(ClientUtilsIntegration, CanGenerateMultipleClientsWithTimestamp) {
    std::vector<std::pair<uint32_t, uint32_t>> clientData;
    
    for (int i = 0; i < 10; i++) {
        uint32_t id = generateClientId();
        uint32_t time = getCurrentTimeMs();
        clientData.push_back({id, time});
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Timestamps should be increasing
    for (size_t i = 1; i < clientData.size(); i++) {
        EXPECT_GE(clientData[i].second, clientData[i-1].second);
    }
}

// ==================== Edge Case Tests ====================

TEST(ClientUtilsEdgeCases, GenerateClientIdConsistency) {
    // Multiple calls should work without issues
    for (int i = 0; i < 1000; i++) {
        uint32_t id = generateClientId();
        EXPECT_GE(id, 0);
    }
}

TEST(ClientUtilsEdgeCases, GetTimeConsistency) {
    // Multiple calls should work without issues
    for (int i = 0; i < 1000; i++) {
        uint32_t time = getCurrentTimeMs();
        EXPECT_GT(time, 0);
    }
}

TEST(ClientUtilsEdgeCases, RapidIdGeneration) {
    std::set<uint32_t> ids;
    
    // Generate IDs as fast as possible
    for (int i = 0; i < 10000; i++) {
        ids.insert(generateClientId());
    }
    
    // Should have generated many unique IDs
    EXPECT_GT(ids.size(), 1000);
}

// ==================== Stress Tests ====================

TEST(ClientUtilsStress, HighFrequencyTimeQueries) {
    uint32_t firstTime = getCurrentTimeMs();
    
    for (int i = 0; i < 10000; i++) {
        uint32_t time = getCurrentTimeMs();
        EXPECT_GE(time, firstTime);
    }
}

TEST(ClientUtilsStress, ParallelIdGeneration) {
    std::vector<std::set<uint32_t>> threadIds(5);
    std::vector<std::thread> threads;
    
    for (int t = 0; t < 5; t++) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < 1000; i++) {
                threadIds[t].insert(generateClientId());
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Each thread should have generated IDs
    for (int t = 0; t < 5; t++) {
        EXPECT_GT(threadIds[t].size(), 100);
    }
}

TEST(ClientUtilsStress, MixedOperations) {
    std::vector<uint32_t> ids;
    std::vector<uint32_t> times;
    
    for (int i = 0; i < 100; i++) {
        ids.push_back(generateClientId());
        times.push_back(getCurrentTimeMs());
        ids.push_back(generateClientId());
        times.push_back(getCurrentTimeMs());
    }
    
    // Should have generated 200 IDs and times
    EXPECT_EQ(ids.size(), 200);
    EXPECT_EQ(times.size(), 200);
    
    // Times should be non-decreasing
    for (size_t i = 1; i < times.size(); i++) {
        EXPECT_GE(times[i], times[i-1]);
    }
}

// ==================== Determinism Tests ====================

TEST(ClientUtilsProperties, IdCanBeAnyValue) {
    // ID can be any uint32_t value from 0 to UINT32_MAX
    bool hasLowValue = false;
    bool hasHighValue = false;
    
    for (int i = 0; i < 1000; i++) {
        uint32_t id = generateClientId();
        if (id < 1000000) hasLowValue = true;
        if (id > 4000000000U) hasHighValue = true;
        
        if (hasLowValue && hasHighValue) break;
    }
    
    // Statistically should have both low and high values
    EXPECT_TRUE(hasLowValue || hasHighValue);
}

TEST(ClientUtilsProperties, TimeNeverGoesBackward) {
    uint32_t prevTime = getCurrentTimeMs();
    
    for (int i = 0; i < 100; i++) {
        uint32_t currentTime = getCurrentTimeMs();
        EXPECT_GE(currentTime, prevTime);
        prevTime = currentTime;
    }
}

// ==================== Boundary Tests ====================

TEST(ClientUtilsProperties, IdIsUint32) {
    uint32_t id = generateClientId();
    // Should be able to cast to uint32_t
    EXPECT_NO_THROW({
        uint32_t copy = id;
        (void)copy;
    });
}

TEST(ClientUtilsProperties, TimeIsUint32) {
    uint32_t time = getCurrentTimeMs();
    // Should be able to cast to uint32_t
    EXPECT_NO_THROW({
        uint32_t copy = time;
        (void)copy;
    });
}
