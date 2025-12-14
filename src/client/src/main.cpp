/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** main
*/

#include <client/RTypeClient.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_ip>" << "<port>" << "<player_name>" << std::endl;
        return 1;
    }

    RTypeClient client;

    // Initialize the client with server IP, port, and player name
    client.init(argv[1], std::stoi(argv[2]), argv[3]);

    // Start the client
    client.run();

    return 0;
}
