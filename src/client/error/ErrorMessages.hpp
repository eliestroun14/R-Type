#ifndef ERRORMESSAGES_HPP
#define ERRORMESSAGES_HPP

namespace ErrorMessages {
    // Server Errors
    constexpr const char *INVALID_MAGIC_NUMBER = "Invalid magic number: The packet header is corrupted or unrecognized.";
    constexpr const char *INVALID_PACKET_TYPE = "Invalid packet type: The packet type does not match any known types.";
    constexpr const char *INVALID_SIZE = "Invalid size: The packet size is inconsistent with the expected format.";
    constexpr const char *CORRUPTED_DATA = "Corrupted data: The packet payload is unreadable or malformed.";

    // Client Errors
    constexpr const char *CONNECTION_FAILED = "Connection failed: Unable to establish a connection to the server.";
    constexpr const char *DISCONNECTED = "Disconnected: The connection to the server was lost unexpectedly.";
    constexpr const char *INVALID_RESPONSE = "Invalid response: The server sent an unrecognized or malformed response.";
    constexpr const char *TIMEOUT = "Timeout: The server did not respond within the expected time frame.";

    // Client Graphical Errors
    constexpr const char *RENDER_FAILURE = "Render failure: Unable to render the graphical interface.";
    constexpr const char *RESOURCE_LOAD_FAILURE = "Resource load failure: Failed to load graphical assets such as textures or models.";
    constexpr const char *UNSUPPORTED_RESOLUTION = "Unsupported resolution: The selected resolution is not supported by the system.";
    constexpr const char *GRAPHICS_INITIALIZATION_FAILED = "Graphics initialization failed: Unable to initialize the graphical subsystem.";

    // General Errors
    constexpr const char *NETWORK_ERROR = "Failed to connect to server: Timeout.";
    constexpr const char *GAMEPLAY_ERROR = "Invalid player input: Unsupported action detected.";
    constexpr const char *RESOURCE_ERROR = "Failed to load resource: File not found.";
    constexpr const char *CONFIGURATION_ERROR = "Invalid configuration: Missing required parameter.";
    constexpr const char *SYSTEM_ERROR = "System error: Insufficient permissions to access the network.";
}

#endif // ERRORMESSAGES_HPP
