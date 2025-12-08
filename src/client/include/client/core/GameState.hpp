#pragma once

#include <cstdint>

namespace client {
namespace core {

enum class GameStateType : uint8_t {
    MENU = 0,
    CONNECTING = 1,
    LOBBY = 2,
    PLAYING = 3,
    PAUSED = 4,
    GAME_OVER = 5
};

class GameState {
public:
    GameState();
    virtual ~GameState() = default;

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    GameStateType getType() const { return m_type; }

protected:
    GameStateType m_type;
};

} // namespace core
} // namespace client
