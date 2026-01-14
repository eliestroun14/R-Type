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

    // IMAGES
    addMenuEntity(createImage(*this->_engine, Assets::LOGO_RTYPE, {150, 50},
        LOGO_RTYPE_ROTATION, LOGO_RTYPE_SCALE, sf::IntRect(0, 0, LOGO_RTYPE_SPRITE_WIDTH, LOGO_RTYPE_SPRITE_HEIGHT), ZIndex::IS_UI_HUD));

    // ANIMATED IMAGES
    Animation planetAnimation(MAIN_MENU_PLANET_SPRITE_WIDTH, MAIN_MENU_PLANET_SPRITE_HEIGHT, MAIN_MENU_PLANET_ANIMATION_CURRENT,
        MAIN_MENU_PLANET_ANIMATION_ELAPSED_TIME, MAIN_MENU_PLANET_ANIMATION_DURATION, MAIN_MENU_PLANET_ANIMATION_START,
        MAIN_MENU_PLANET_ANIMATION_END, MAIN_MENU_PLANET_ANIMATION_LOOPING);

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::MAIN_MENU_PLANET, planetAnimation, {-200, -1300},
        0, MAIN_MENU_PLANET_SPRITE_SCALE, sf::IntRect(0, 0, MAIN_MENU_PLANET_SPRITE_WIDTH, MAIN_MENU_PLANET_SPRITE_HEIGHT), ZIndex::IS_BACKGROUND));


    // BUTTONSs
    addMenuEntity(createButton(*_engine, "PLAY", 40, sf::Color::White, {275, 300}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                // this->createOptionMenu();
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "OPTIONS", 40, sf::Color::White, {275, 500}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createOptionMenu();
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "EXIT", 40, sf::Color::White, {275, 700}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                if (this->_engine)
                    this->_engine->closeWindow();
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
