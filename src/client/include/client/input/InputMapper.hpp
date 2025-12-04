#pragma once

#include "InputManager.hpp"
#include <functional>
#include <unordered_map>
#include <string>

namespace client {
namespace input {

enum class Action : uint8_t {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    SHOOT,
    USE_ABILITY,
    PAUSE
};

class InputMapper {
public:
    void bindKey(Action action, Key key);
    void unbindKey(Action action);

    bool isActionActive(Action action) const;
    void update(const InputManager& inputManager);

    void saveBindings(const std::string& filepath);
    void loadBindings(const std::string& filepath);

private:
    std::unordered_map<Action, Key> m_bindings;
    std::unordered_map<Action, bool> m_actionStates;
};

} // namespace input
} // namespace client
