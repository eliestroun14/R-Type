/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ClientMenu
*/

#include <client/menu/ClientMenu.hpp>

ClientMenu::ClientMenu(std::shared_ptr<gameEngine::GameEngine> engine)
    : _engine(engine)
{}

ClientMenu::~ClientMenu()
{
}

void ClientMenu::update() {
    while (!_pendingActions.empty()) {
        _pendingActions.front()();
        _pendingActions.pop();
    }
}

void ClientMenu::createMainMenu()
{
    // BACKGROUND
    addMenuEntity(createImage(*this->_engine, Assets::GAME_BG,
        sf::Vector2f(0, 0), MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
        sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
        ZIndex::IS_BACKGROUND));


    

    // BUTTONSs
    addMenuEntity(createButton(*_engine, "OPTIONS", 30, sf::Color::White, {400, 500}, 1.0f, sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createOptionMenu();
            });
        }
    ));
}

void ClientMenu::createOptionMenu()
{
    std::cout << "ClientMenu Option Menu created" << std::endl;
}

void ClientMenu::clearMenuEntities()
{
    for (auto& entity : _menuEntities)
        _engine->destroyEntity(entity);
    _menuEntities.clear();
}
