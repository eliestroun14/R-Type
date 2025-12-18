/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ClientUtils
*/

#ifndef CLIENT_UTILS_HPP
#define CLIENT_UTILS_HPP

#include <cstdint>

/**
 * @brief Get the current time in milliseconds since epoch
 * @return uint32_t The current time in milliseconds
 */
uint32_t getCurrentTimeMs();

/**
 * @brief Generate a unique client ID using random number generation
 * @return uint32_t A randomly generated client ID
 */
uint32_t generateClientId();

#endif /* !CLIENT_UTILS_HPP */
