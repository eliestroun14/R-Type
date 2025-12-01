#ifndef ERRORMESSAGES_HPP
#define ERRORMESSAGES_HPP

namespace ErrorMessages {
    constexpr const char *NETWORK_ERROR = "Failed to connect to server: Timeout.";
    constexpr const char *GAMEPLAY_ERROR = "Invalid player input: Unsupported action detected.";
    constexpr const char *RESOURCE_ERROR = "Failed to load resource: File not found.";
    constexpr const char *CONFIGURATION_ERROR = "Invalid configuration: Missing required parameter.";
    constexpr const char *SYSTEM_ERROR = "System error: Insufficient permissions to access the network.";
}

#endif // ERRORMESSAGES_HPP
