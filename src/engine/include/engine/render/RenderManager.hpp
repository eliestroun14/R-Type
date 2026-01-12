/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#ifndef RENDERMANAGER_HPP_
#define RENDERMANAGER_HPP_

#include <SFML/Graphics.hpp>
#include <engine/render/TextureStorage.hpp>
#include <engine/ecs/entity/Entity.hpp>
#include <common/constants/defines.hpp>
#include <engine/render/FontStorage.hpp>

// Forward declaration
class Coordinator;

// can change on updates
/**
 * @enum GameAction
 * @brief Represents high-level logical actions in the game.
 * * This enum abstracts the physical keyboard/mouse inputs into game logic.
 * Using this allows changing key bindings without breaking game logic code.
 */
enum class GameAction {
    MOVE_LEFT,      ///< Move the entity to the left (Default: Left Arrow)
    MOVE_RIGHT,     ///< Move the entity to the right (Default: Right Arrow)
    MOVE_UP,        ///< Move the entity up (Default: Up Arrow)
    MOVE_DOWN,      ///< Move the entity down (Default: Down Arrow)
    SHOOT,          ///< Fire main weapon (Default: S)
    SWITCH_WEAPON,  ///< Change current weapon (Default: D)
    USE_POWERUP,    ///< Activate hold powerup (Default: Space)
    SPECIAL,        ///< Trigger special ability (Default: F)
    OPTIONS,        ///< Open option menu (Default: P - commented out)
    EXIT,           ///< Close the game/window (Default: Escape)
    RIGHT_CLICK,    ///< Interact with the UI (Default: Right click)
    LEFT_CLICK,     ///< Interact with the UI (Default: left click)
    UNKNOW          ///< Fallback for unrecognized actions
};

class RenderManager {
    public:
        /**
         * @brief Construct a new Render Manager object.
         * * Initializes the key bindings map and sets initial action states to false.
         * @note Does NOT open the window. Call init() to open the window.
         */
        RenderManager();

        /**
         * @brief Destroy the Render Manager object.
         */
        ~RenderManager();

        /**
         * @brief Initializes the window and graphical settings.
         * * Creates the SFML window with defined dimensions (WINDOW_WIDTH, WINDOW_HEIGHT),
         * sets the framerate limit, and centers the window on the desktop.
         */
        void init();

        /**
         * @brief Sets the local player entity for input handling.
         * * Should be called after the local player entity is created.
         * 
         * @param coordinator Reference to the Coordinator
         * @param localPlayerEntity The entity ID of the local player
         */
        void setLocalPlayer(Coordinator& coordinator, Entity localPlayerEntity);

        /**
         * @brief Clears the window buffer to prepare for new frame drawing.
         * * Should be called at the start of each frame before any drawing.
         */
        void beginFrame();

        /**
         * @brief Displays the rendered frame.
         * * Should be called at the end of the frame after all drawing is done.
         */
        void render();

        /**
         * @brief Polls all pending input events from the window.
         * * Retrieves events (keyboard, mouse, window close) from SFML
         * and updates the internal state of GameActions via handleEvent().
         * Updates the mouse position.
         */
        void processInput();

        /**
         * @brief Processes a single SFML event to update action states.
         * * Maps a raw SFML event (KeyPressed/Released) to a GameAction
         * and updates the _activeActions map.
         * * @param event The raw SFML event to process.
         * @warning Public mainly for Unit Testing purposes (Mocking inputs).
         */
        void handleEvent(const sf::Event& event);

        /**
         * @brief Checks if a specific game action is currently active (pressed).
         * * @param action The logical action to check (e.g., GameAction::SHOOT).
         * @return true If the key corresponding to the action is currently held down.
         * @return false Otherwise.
         */
        bool isActionActive(GameAction action) const;

        /**
         * @brief Checks if a specific game action was just pressed (edge detection).
         * * @param action The logical action to check.
         * @return true If the action is currently pressed AND was not pressed last frame.
         * @return false Otherwise.
         */
        bool isActionJustPressed(GameAction action) const;


        /**
         * @brief Checks if a specific game action was just released (edge detection).
         * * @param action The logical action to check.
         * @return true If the action is currently released AND was not released last frame.
         * @return false Otherwise.
         */
        bool isActionJustReleased(GameAction action) const;

        /**
         * @brief Retrieves the current mouse position relative to the window.
         * * @return sf::Vector2i Coordinates (x, y) of the mouse.
         */
        sf::Vector2i getMousePosition() const;

        /**
         * @brief Checks if the game window is currently open.
         * * @return true If the window is open and running.
         * @return false If the window has been closed.
         */
        bool isOpen() const;

        /**
         * @brief Get the full map of active actions.
         * * @return const std::map<GameAction, bool>& Reference to the internal action map.
         */
        std::map<GameAction, bool>& getActiveActions();

        /**
         * @brief Get the sf::Texture of sprite in function of his id.
         * * @return const std::shared_ptr<sf::Texture> to the sf::Texture.
         * * @param id The id reference to the enum Assets
         */
        std::shared_ptr<sf::Texture> getTexture(Assets id) const;


        /**
         * @brief Get the sf::Font of sprite in function of his id.
         * * @return const std::shared_ptr<sf::Font> to the sf::Font.
         * * @param id The id reference to the enum fontAssets
         */
        std::shared_ptr<sf::Font> getFont(FontAssets id) const;


        /**
         * @brief Get the sf::RenderWindow of the game.
         * * @return const sf::RenderWindow& Reference to the window.
         */
        sf::RenderWindow& getWindow();

        /**
         * @brief Get the Factor scale for adjust velocity in function of the window size.
         * * @return const float of the factor.
         */
        float getScaleFactor() const;

    private:
        Coordinator* _coordinator;
        Entity _localPlayerEntity;

        sf::RenderWindow _window;

        TextureStorage _textures;
        FontStorage _fonts;

        /// @brief Maps physical keys (sf::Keyboard::Key) to logical actions (GameAction).
        std::map<sf::Keyboard::Key, GameAction> _keyBindings;

        /// @brief Stores the current state (true=pressed, false=released) of each action.
        std::map<GameAction, bool> _activeActions;

        /// @brief Stores the previous frame state of each action (for edge detection).
        std::map<GameAction, bool> _previousActions;

        sf::Vector2i _mousePos;
};

#endif /* !RENDERMANAGER_HPP_ */
