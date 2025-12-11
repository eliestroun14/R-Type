/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientUtils
*/

#include <client/utils/ClientUtils.hpp>
#include <random>
#include <iostream>
#include <chrono>

uint32_t generateClientId()
{
    std::random_device rd;  // Random seed generator
    std::mt19937 gen(rd());  // Mersenne Twister random engine
    std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);  // Uniform distribution

    return dis(gen);
}

uint32_t getCurrentTimeMs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return milliseconds;
}
