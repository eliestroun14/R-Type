/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ClientMenu
*/

#ifndef CLIENTMENU_HPP_
#define CLIENTMENU_HPP_

#include <engine/GameEngine.hpp>
#include <game/utils/ClientUtils.hpp>
#include <game/menu/IMenu.hpp>
#include <queue>
#include <memory>

// Forward declaration
class Coordinator;

class ClientMenu : public IMenu {

    public:
        ClientMenu(std::shared_ptr<gameEngine::GameEngine> engine, std::shared_ptr<Coordinator> coordinator);
        ~ClientMenu();

        template<typename T>
        void addMenuEntity(T entityOrList) {
            if constexpr (std::is_same_v<T, Entity>)
                _menuEntities.push_back(entityOrList);
            else
                _menuEntities.insert(_menuEntities.end(), entityOrList.begin(), entityOrList.end());
        }

        void update() override;

        /**
         * @brief Method that create every entity for the main menu.
         */
        void createMainMenu() override;

        /**
         * @brief Method that create every entity for the option menu.
         */
        void createOptionMenu() override;

        /**
         * @brief Method that create every entity for the keybinds menu.
         */
        void createKeybindsMenu();

        /**
         * @brief Method that create every entity for the accessibility menu.
         */
        void createAccessibilityMenu();

        /**
         * @brief Method that create every entity for the accessibility menu.
         */
        void createPlayMenu();

        /**
         * @brief Method that remove every entities'components to diseable and hide them.
         */
        void clearMenuEntities(bool keepBackground = false);

    private:
        bool _isConnected;
        std::shared_ptr<gameEngine::GameEngine> _engine;
        std::shared_ptr<Coordinator> _coordinator;
        std::vector<Entity> _menuEntities;
        std::queue<std::function<void()>> _pendingActions;

        Assets _currentBackgroundAsset = Assets::MAIN_MENU_BG;

        bool _musicOn;
        bool _soundOn;
};

#endif /* !CLIENTMENU_HPP_ */
