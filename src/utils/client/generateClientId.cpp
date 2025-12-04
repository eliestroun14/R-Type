/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** generateClientId
*/

#include <random>
#include <iostream>

uint32_t generateClientId()
{
    std::random_device rd;          // génère une graine aléatoire
    std::mt19937 gen(rd());         // algorithme de nombres
    std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);

    return dis(gen);
}
