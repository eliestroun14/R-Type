#pragma once

#include <unordered_map>
#include <memory>
#include <cstdint>

namespace server {
namespace session {

class PlayerSession;

class SessionManager {
public:
    std::shared_ptr<PlayerSession> createSession(uint32_t clientId);
    std::shared_ptr<PlayerSession> getSession(uint32_t sessionId);
    void removeSession(uint32_t sessionId);

    void updateSessions(float deltaTime);

private:
    std::unordered_map<uint32_t, std::shared_ptr<PlayerSession>> m_sessions;
    uint32_t m_nextSessionId = 1;
};

} // namespace session
} // namespace server
