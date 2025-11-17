/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main
*/

#include <iostream>
#include <string>

int main(int argc, char const **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <name>" << std::endl;
        return 1;
    }
    std::printf("Hello, %s!\n", argv[1]);
    return 0;
}