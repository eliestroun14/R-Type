/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** RebindSystem
*/

#include <game/systems/RebindSystem.hpp>
#include <map>
#include <game/utils/ClientUtils.hpp>

void RebindSystem::applyNewKeybind(GameConfig& config, sf::Keyboard::Key key, GameAction action)
{
    // remove old binds to avoid that a key do 2 actions
    for (auto it = config._keybinds.begin(); it != config._keybinds.end();) {
        if (it->second == action)
            it = config._keybinds.erase(it);
        else
            ++it;
    }
    config._keybinds[key] = action;
}

void RebindSystem::onUpdate(float dt) {
    auto& rebinds = this->_engine.getComponents<Rebind>();
    auto& configs = this->_engine.getComponents<GameConfig>();

    for (size_t e = 0; e < rebinds.size(); e++) {
        if (!rebinds[e].has_value() || !rebinds[e]->isWaiting)
            continue;

        for (auto& config : configs) {
            if (!config.has_value()) continue;

            for (int k = 0; k < sf::Keyboard::KeyCount; ++k) {
                sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);

                if (sf::Keyboard::isKeyPressed(key)) {
                    // update the map in GameConfig entity
                    this->applyNewKeybind(config.value(), key, rebinds[e]->action);

                    // we refresh all buttons in case user set a key already used by another action
                    // get through all rebinds component to update them
                    for (size_t j = 0; j < rebinds.size(); j++) {
                        if (rebinds[j].has_value()) {
                            // find which key is link to the action
                            sf::Keyboard::Key currentKey = sf::Keyboard::Unknown;
                            for (auto const& [kBind, actionBind] : config.value()._keybinds)
                                if (actionBind == rebinds[j]->action) {
                                    currentKey = kBind;
                                    break;
                                }

                            // update the text of the entity linked to
                            auto& targetTxt = _engine.getComponentEntity<Text>(rebinds[j]->associatedText);
                            if (currentKey != sf::Keyboard::Unknown)
                                std::strncpy(targetTxt->str, keyToString(currentKey).c_str(), sizeof(targetTxt->str) - 1);
                            else
                                // if the action does not have key now (because of duplication), display NONE
                                std::strncpy(targetTxt->str, "NONE", sizeof(targetTxt->str) - 1);
                        }
                    }

                    rebinds[e]->isWaiting = false;
                    break;
                }
            }
            break;
        }
    }
}
