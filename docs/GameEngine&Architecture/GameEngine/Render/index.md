# RenderManager

The `RenderManager` encapsulates the graphical interface (SFML), resource management, and the input system. It acts as an abstraction layer between the raw hardware events (Keyboard, Mouse, Window) and the high-level game logic.

It is responsible for window lifecycle, maintaining framerate, polling events, managing textures, and translating physical inputs into logical game actions.

## Architecture & Design

### Input Abstraction (`GameAction`)
To decouple game logic from specific hardware keys, the engine uses an intermediary enum called `GameAction`.

* **Continuous State:** `isActionActive` checks if a key is currently held down (e.g., movement).
* **Edge Detection:** `isActionJustPressed` checks if a key was pressed *this specific frame* (e.g., shooting semi-auto, toggling UI).

### Resource Management
The RenderManager owns a `TextureStorage` subsystem. It pre-loads all game assets at initialization to prevent lag spikes during gameplay. Systems access these assets via `getTexture()`, which returns thread-safe shared pointers.

## API Reference

### 1. Lifecycle & Initialization

| Method | Description |
| :--- | :--- |
| `RenderManager()` | **Constructor**. Initializes internal key bindings and action states. Does **not** open the window. |
| `void init()` | **Opens the Window**. Creates the SFML window, sets framerate limit, loads textures, and centers the window on the desktop. |
| `~RenderManager()` | **Destructor**. Handles clean-up. |
| `bool isOpen()` | Returns `true` if the window is currently open. |

### 2. Input Handling

| Method | Description |
| :--- | :--- |
| `void setLocalPlayer(Coordinator&, Entity)` | **ECS Link.** Associates the Input System with a specific local entity (for updating ECS `InputComponent`). |
| `void processInput()` | **Main Input Loop**. Polls SFML events, updates mouse position, and updates `GameAction` states (Current & Previous). |
| `bool isActionActive(GameAction)` | **Continuous.** Returns `true` if the key is currently held down. |
| `bool isActionJustPressed(GameAction)` | **Trigger.** Returns `true` only on the exact frame the key was pressed. |
| `getMousePosition()` | Returns the current (x, y) coordinates of the mouse relative to the window. |
| `getActiveActions()` | Returns a reference to the complete map of active action states. |

### 3. Rendering

| Method | Description |
| :--- | :--- |
| `void beginFrame()` | **Step 1.** Clears the window buffer. Must be called at the start of the render loop. |
| `void render()` | **Step 2.** Displays the buffer (Swaps buffers). Must be called at the very end of the frame. |

### 4. Resources & Window Access

| Method | Description |
| :--- | :--- |
| `getTexture(Assets id)` | Retrieves a `std::shared_ptr<sf::Texture>` for a specific asset ID. Returns `nullptr` if invalid. |
| `getWindow()` | Returns a reference to the raw `sf::RenderWindow`. Used by `RenderSystem` to draw sprites. |
| `getScaleFactor()` | Returns a float representing the window scale factor (useful for resolution-independent velocity). |

## Default Key Bindings

Source of truth: see constructor in [src/engine/src/engine/render/RenderManager.cpp](src/engine/src/engine/render/RenderManager.cpp#L15-L44).

| Physical Key | Logical Action | Description |
| :--- | :--- | :--- |
| **Arrow Up** | `MOVE_UP` | Move up |
| **Arrow Down** | `MOVE_DOWN` | Move down |
| **Arrow Left** | `MOVE_LEFT` | Move left |
| **Arrow Right** | `MOVE_RIGHT` | Move right |
| **Space** | `SHOOT` | Fire primary weapon |
| **Escape** | `EXIT` | Exit/close |

Unbound (but tracked): `SWITCH_WEAPON`, `USE_POWERUP`, `SPECIAL` are initialized to `false` with no default key. Alternative ZQSD bindings are commented in code for convenience.

### Binding snippet (from code)

```cpp
// Default bindings
this->_keyBindings[sf::Keyboard::Up] = GameAction::MOVE_UP;
this->_keyBindings[sf::Keyboard::Down] = GameAction::MOVE_DOWN;
this->_keyBindings[sf::Keyboard::Left] = GameAction::MOVE_LEFT;
this->_keyBindings[sf::Keyboard::Right] = GameAction::MOVE_RIGHT;
this->_keyBindings[sf::Keyboard::Space] = GameAction::SHOOT;
this->_keyBindings[sf::Keyboard::Escape] = GameAction::EXIT;
```

## Usage Example

In the `Coordinator` or `GameEngine` loop:

In the `Coordinator` or `GameEngine` loop:

```cpp
#include "RenderManager.hpp"

void ClientGameLoop() {
    RenderManager renderer;

    // 1. Init Window & Textures
    renderer.init();

    while (renderer.isOpen()) {

        // 2. Process Inputs
        renderer.processInput();

        // 3. Logic: Check Actions
        if (renderer.isActionJustPressed(GameAction::EXIT)) {
            // Close window logic
        }

        // 4. Rendering Cycle
        renderer.beginFrame(); // Clear

        // ... (RenderSystem draws sprites here using renderer.getWindow()) ...

        renderer.render(); // Display
    }
}