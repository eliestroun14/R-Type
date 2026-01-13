/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ClientMenu
*/

#ifndef CLIENTMENU_HPP_
#define CLIENTMENU_HPP_

#include <game/Game.hpp>
#include <game/utils/ClientUtils.hpp>

class ClientMenu {

    public:
        ClientMenu();
        ~ClientMenu();


        /**
         * @brief Method that create every entity for the main menu.
         */
        void createMainMenu();

        /**
         * @brief Method that create every entity for the option menu.
         */
        void createOptionMenu();

    private:
        bool _isConnected;
        std::unique_ptr<Game> _game;


};

#endif /* !CLIENTMENU_HPP_ */
