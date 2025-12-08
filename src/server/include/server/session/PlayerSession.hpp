#pragma once

#include <string>
#include <cstdint>

namespace server {
namespace session {

class PlayerSession {
public:
    PlayerSession(uint32_t sessionId, uint32_t clientId);

    uint32_t getSessionId() const { return m_sessionId; }
    uint32_t getClientId() const { return m_clientId; }
    uint32_t getPlayerId() const { return m_playerId; }

    const std::string& getPlayerName() const { return m_playerName; }
    void setPlayerName(const std::string& name) { m_playerName = name; }

    bool isAuthenticated() const { return m_authenticated; }
    void setAuthenticated(bool auth) { m_authenticated = auth; }

    void updateLastActivity();
    uint32_t getLastActivity() const { return m_lastActivity; }

private:
    uint32_t m_sessionId;
    uint32_t m_clientId;
    uint32_t m_playerId;
    std::string m_playerName;
    bool m_authenticated = false;
    uint32_t m_lastActivity = 0;
};

} // namespace session
} // namespace server
