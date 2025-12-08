//#include "client/core/Client.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    /* try {
        client::core::Client client;

        if (!client.initialize()) {
            std::cerr << "Failed to initialize client" << std::endl;
            return 1;
        }

        // Parse command line arguments for server address
        std::string serverAddress = "127.0.0.1";
        uint16_t serverPort = 8080;

        if (argc >= 2) {
            serverAddress = argv[1];
        }
        if (argc >= 3) {
            serverPort = static_cast<uint16_t>(std::stoi(argv[2]));
        }

        std::cout << "R-Type Client starting..." << std::endl;
        std::cout << "Server: " << serverAddress << ":" << serverPort << std::endl;

        client.run();
        client.shutdown();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    } */

    printf("hello world");
}
