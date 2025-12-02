/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** getCurrentTimeMs
*/

#include <cstdint>
#include <chrono>

uint32_t getCurrentTimeMs()
{
    auto now = std::chrono::system_clock::now();

    auto duration = now.time_since_epoch();

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return milliseconds;
}

