/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** IMenu
*/

#ifndef IMENU_HPP_
#define IMENU_HPP_

class IMenu {
    public:
        virtual ~IMenu() = default;

        virtual void update() = 0;

        virtual void createMainMenu() = 0;
        virtual void createOptionMenu() = 0;
};

#endif /* !IMENU_HPP_ */
