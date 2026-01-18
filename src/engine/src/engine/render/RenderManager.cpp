/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#include <engine/render/RenderManager.hpp>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>
#include <common/constants/render/Assets.hpp>
#include <common/constants/defines.hpp>


RenderManager::RenderManager() : _coordinator(nullptr), _localPlayerEntity(Entity::fromId(0))
{
    // this->_keyBindings[sf::Keyboard::Up] = GameAction::MOVE_UP;
    // // this->_keyBindings[sf::Keyboard::Z] = GameAction::MOVE_UP; // to support ZQSD

    // this->_keyBindings[sf::Keyboard::Down] = GameAction::MOVE_DOWN;
    // // this->_keyBindings[sf::Keyboard::S] = GameAction::MOVE_DOWN; // to support ZQSD

    // this->_keyBindings[sf::Keyboard::Left] = GameAction::MOVE_LEFT;
    // // this->_keyBindings[sf::Keyboard::Q] = GameAction::MOVE_LEFT; // to support ZQSD

    // this->_keyBindings[sf::Keyboard::Right] = GameAction::MOVE_RIGHT;
    // // this->_keyBindings[sf::Keyboard::D] = GameAction::MOVE_RIGHT; // to support ZQSD

    // // Primary shoot bindings
    // this->_keyBindings[sf::Keyboard::Space] = GameAction::SHOOT;
    // // this->_keyBindings[sf::Keyboard::P] = GameAction::OPTIONS;
    // this->_keyBindings[sf::Keyboard::Escape] = GameAction::EXIT;


    this->_activeActions[GameAction::MOVE_UP] = false;
    this->_activeActions[GameAction::MOVE_DOWN] = false;
    this->_activeActions[GameAction::MOVE_LEFT] = false;
    this->_activeActions[GameAction::MOVE_RIGHT] = false;
    this->_activeActions[GameAction::SHOOT] = false;
    this->_activeActions[GameAction::SWITCH_WEAPON] = false;
    this->_activeActions[GameAction::USE_POWERUP] = false;
    this->_activeActions[GameAction::SPECIAL] = false;
    this->_activeActions[GameAction::OPTIONS] = false;
    this->_activeActions[GameAction::EXIT] = false;

    this->_activeActions[GameAction::RIGHT_CLICK] = false;
    this->_activeActions[GameAction::LEFT_CLICK] = false;
}

RenderManager::~RenderManager()
{
}

void RenderManager::init()
{
    // TODO: when project is ended, set fullscreen mod for the window

    // Load icon BEFORE creating window
    auto image = sf::Image{};
    if (!image.loadFromFile(pathAssets[RTYPE_ICON])) {
        throw Error(ErrorType::ResourceLoadFailure, ErrorMessages::RESOURCE_LOAD_FAILURE);
    }

    // Create window and set icon BEFORE positioning
    this->_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "R-Type", sf::Style::Fullscreen);
    this->_window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
    std::cout << "[RenderManager] Icon set to window" << std::endl;
    
    this->_window.setFramerateLimit(FRAMERATE_LIMIT);
    this->_window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

    auto desktop = sf::VideoMode::getDesktopMode();

    // to center position. If window is in fullscreen, comment this part below
    this->_window.setPosition(sf::Vector2i(
        desktop.width / 2 - this->_window.getSize().x / 2,
        desktop.height / 2 - this->_window.getSize().y / 2));
    
    // load every textures for the game.
    this->_textures.init();

    // load every fonts for the game.
    this->_fonts.init();
}

void RenderManager::setLocalPlayer(Coordinator& coordinator, Entity localPlayerEntity)
{
    this->_coordinator = &coordinator;
    this->_localPlayerEntity = localPlayerEntity;
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

    // Preserve previous state before processing new events (edge detection)
    this->_previousActions = this->_activeActions;

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

bool RenderManager::isActionJustPressed(GameAction action) const
{
    bool currentState = isActionActive(action);
    bool previousState = false;
    
    if (this->_previousActions.count(action))
        previousState = this->_previousActions.at(action);
    
    // True only if currently pressed AND was not pressed before
    return currentState && !previousState;
}

bool RenderManager::isActionJustReleased(GameAction action) const {
    auto it = _previousActions.find(action);
    bool previousState = (it != _previousActions.end()) ? it->second : false;

    return !isActionActive(action) && previousState;
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

    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        bool isPressed = (event.type == sf::Event::KeyPressed);

        // find in the updated map
        auto it = _keyBindings.find(event.key.code);
        if (it != _keyBindings.end())
            this->_activeActions[it->second] = isPressed;
    }

    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        bool isPressed = (event.type == sf::Event::MouseButtonPressed);

        if (event.mouseButton.button == sf::Mouse::Left)
            this->_activeActions[GameAction::LEFT_CLICK] = isPressed;

        if (event.mouseButton.button == sf::Mouse::Right)
            this->_activeActions[GameAction::RIGHT_CLICK] = isPressed;
    }
}

std::map<GameAction, bool>& RenderManager::getActiveActions()
{
    return this->_activeActions;
}

std::shared_ptr<sf::Texture> RenderManager::getTexture(Assets id) const
{
    return this->_textures.getTexture(id);
}

std::shared_ptr<sf::Font> RenderManager::getFont(FontAssets id) const
{
    return this->_fonts.getFont(id);
}

sf::RenderWindow &RenderManager::getWindow()
{
    return this->_window;
}

float RenderManager::getScaleFactor() const
{
    sf::Vector2u windowSize = this->_window.getSize();
    float scaleX = windowSize.x / WINDOW_WIDTH;
    float scaleY = windowSize.y / WINDOW_HEIGHT;
    return std::min(scaleX, scaleY);
}

void RenderManager::closeWindow()
{
    this->_window.close();
}
