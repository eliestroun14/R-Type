/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#include <engine/gameEngine/coordinator/render/RenderManager.hpp>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>
#include <common/constants/render/Assets.hpp>

RenderManager::RenderManager()
{
    this->_keyBindings[sf::Keyboard::Up] = GameAction::MOVE_UP;
    // this->_keyBindings[sf::Keyboard::Z] = GameAction::MOVE_UP; // to support ZQSD

    this->_keyBindings[sf::Keyboard::Down] = GameAction::MOVE_DOWN;
    // this->_keyBindings[sf::Keyboard::S] = GameAction::MOVE_DOWN; // to support ZQSD

    this->_keyBindings[sf::Keyboard::Left] = GameAction::MOVE_LEFT;
    // this->_keyBindings[sf::Keyboard::Q] = GameAction::MOVE_LEFT; // to support ZQSD

    this->_keyBindings[sf::Keyboard::Right] = GameAction::MOVE_RIGHT;
    // this->_keyBindings[sf::Keyboard::D] = GameAction::MOVE_RIGHT; // to support ZQSD

    this->_keyBindings[sf::Keyboard::S] = GameAction::SHOOT;
    this->_keyBindings[sf::Keyboard::D] = GameAction::SWITCH_WEAPON;
    this->_keyBindings[sf::Keyboard::Space] = GameAction::USE_POWERUP;
    this->_keyBindings[sf::Keyboard::F] = GameAction::SPECIAL;
    // this->_keyBindings[sf::Keyboard::P] = GameAction::OPTIONS;
    this->_keyBindings[sf::Keyboard::Escape] = GameAction::EXIT;


    this->_activeActions[GameAction::MOVE_UP] = false;
    this->_activeActions[GameAction::MOVE_DOWN] = false;
    this->_activeActions[GameAction::MOVE_LEFT] = false;
    this->_activeActions[GameAction::MOVE_RIGHT] = false;
    this->_activeActions[GameAction::SHOOT] = false;
    this->_activeActions[GameAction::SWITCH_WEAPON] = false;
    this->_activeActions[GameAction::USE_POWERUP] = false;
    this->_activeActions[GameAction::SPECIAL] = false;
    // this->_activeActions[GameAction::OPTIONS] = false;
    this->_activeActions[GameAction::EXIT] = false;
}

RenderManager::~RenderManager()
{
}

void RenderManager::init()
{
    // TODO: when project is ended, set fullscreen mod for the window

    this->_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "R-Type", sf::Style::Close);
    this->_window.setFramerateLimit(FRAMERATE_LIMIT);

    auto desktop = sf::VideoMode::getDesktopMode();

    // to center position. If window is in fullscreen, comment this part below
    this->_window.setPosition(sf::Vector2i(
        desktop.width / 2 - this->_window.getSize().x / 2,
        desktop.height / 2 - this->_window.getSize().y / 2));

    auto image = sf::Image{};
    if (!image.loadFromFile(pathAssets[RTYPE_ICON])) {
        throw Error(ErrorType::ResourceLoadFailure, ErrorMessages::RESOURCE_LOAD_FAILURE);
    }

    
    this->_window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
    
    // load every textures for the game.
    this->_textures.init();
}

void RenderManager::beginFrame()
{
    if (!this->_window.isOpen())
        return;

    this->_window.clear();
}

void RenderManager::render()
{
    if (!this->_window.isOpen())
        return;

    this->_window.display();
}

void RenderManager::processInput()
{
    sf::Event event;

    this->_mousePos = sf::Mouse::getPosition(this->_window);

    while(this->_window.pollEvent(event)) {
        handleEvent(event);
    }
}

bool RenderManager::isActionActive(GameAction action) const
{
    if (this->_activeActions.count(action))
        return this->_activeActions.at(action);
    return false;
}

sf::Vector2i RenderManager::getMousePosition() const
{
    return _mousePos;
}

bool RenderManager::isOpen() const
{
    return this->_window.isOpen();
}

void RenderManager::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::Closed)
        this->_window.close();

    if (event.type == sf::Event::KeyPressed
        || event.type == sf::Event::KeyReleased) {
        bool isPressed = (event.type == sf::Event::KeyPressed);

        if (this->_keyBindings.count(event.key.code)) {
            GameAction action = this->_keyBindings[event.key.code];
            this->_activeActions[action] = isPressed;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed)
        if (event.mouseButton.button == sf::Mouse::Left)
            return; //TODO: add here action

    if (event.type == sf::Event::MouseButtonPressed)
        if (event.mouseButton.button == sf::Mouse::Right)
            return; //TODO: add here action
}

std::map<GameAction, bool>& RenderManager::getActiveActions()
{
    return this->_activeActions;
}

std::shared_ptr<sf::Texture> RenderManager::getTexture(Assets id) const
{
    return this->_textures.getTexture(id);
}

sf::RenderWindow &RenderManager::getWindow()
{
    return this->_window;
}
