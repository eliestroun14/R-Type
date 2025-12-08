#pragma once

#include <string>
#include <functional>
#include <unordered_map>

namespace server {
namespace utils {

class CommandHandler {
public:
    using CommandFunction = std::function<void(const std::vector<std::string>&)>;

    void registerCommand(const std::string& command, CommandFunction handler);
    void executeCommand(const std::string& commandLine);

    void listCommands();

private:
    std::unordered_map<std::string, CommandFunction> m_commands;
};

} // namespace utils
} // namespace server
