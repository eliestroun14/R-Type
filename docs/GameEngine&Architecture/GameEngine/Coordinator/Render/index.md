# RenderManager

The `RenderManager` encapsulates the graphical interface (SFML) and the input system. It acts as an abstraction layer between the raw hardware events (Keyboard, Mouse, Window) and the game logic.

It is responsible for creating the window, maintaining the framerate, polling events, and translating physical inputs into logical game actions.

## Architecture & Design

### Input Abstraction (`GameAction`)
To decouple the game logic from specific hardware keys, the engine uses an intermediary enum called `GameAction`. Systems do not check if "Space" is pressed; they check if `GameAction::USE_POWERUP` is active.

**Benefits:**
- **Rebindable Keys:** Changing a key binding only requires updating the internal map, not the game logic.
- **Multi-input support:** Both "Arrow Up" and "Z" could trigger the same `MOVE_UP` action.

### Non-Blocking Input Loop
The manager uses `pollEvent` instead of `waitEvent`. This ensures the game loop continues running (updating physics, network) even if the user is not pressing any keys.

## API Reference

### 1. Lifecycle & Initialization

| Method | Description |
| :--- | :--- |
| `RenderManager()` | **Constructor**. Initializes internal key bindings and action states. Does **not** open the window. |
| `void init()` | **Opens the Window**. Creates an 800x600 SFML window, sets the limit to 60 FPS, and centers the window on the desktop. |
| `~RenderManager()` | **Destructor**. Handles clean-up. |
| `bool isOpen()` | Returns `true` if the window is currently open. |

### 2. Input Handling

| Method | Description |
| :--- | :--- |
| `void processInput()` | **Main Input Loop**. Updates mouse position and polls all pending events from SFML. Calls `handleEvent` internally. |
| `bool isActionActive(GameAction)` | **Query State**. Returns `true` if the specific action is currently being held down. Used by Systems. |
| `sf::Vector2i getMousePosition()` | Returns the current (x, y) coordinates of the mouse relative to the window. |
| `handleEvent(const sf::Event&)` | **Internal Logic**. Processes a single event to update the state map. Public primarily for Unit Testing (mocking inputs). |
| `getActiveActions()` | Returns a reference to the complete map of action states. |

### 3. Rendering

| Method | Description |
| :--- | :--- |
| `void render()` | **Draws the Frame**. Clears the window, (draws entities - *WIP*), and displays the buffer. Must be called once per frame. |

## Default Key Bindings

The `RenderManager` comes pre-configured with the following default mappings:

| Physical Key | Logical Action | Description |
| :--- | :--- | :--- |
| **Arrow Up** | `MOVE_UP` | Move ship up |
| **Arrow Down** | `MOVE_DOWN` | Move ship down |
| **Arrow Left** | `MOVE_LEFT` | Move ship left |
| **Arrow Right** | `MOVE_RIGHT` | Move ship right |
| **S** | `SHOOT` | Fire main weapon |
| **D** | `SWITCH_WEAPON` | Cycle through weapons |
| **Space** | `USE_POWERUP` | Activate current powerup |
| **F** | `SPECIAL` | Trigger special ability |
| **Escape** | `EXIT` | Close the game |

*(Note: ZQSD bindings are present in the source code but commented out by default)*

## Usage Example

In the `Coordinator` or `GameEngine` loop:

```cpp
#include "RenderManager.hpp"

void GameLoop() {
    RenderManager renderer;
    
    // 1. Open Window
    renderer.init();

    while (renderer.isOpen()) {
        
        // 2. Poll Inputs (Keyboard/Mouse)
        renderer.processInput();

        // 3. Logic: Check Actions
        if (renderer.isActionActive(GameAction::EXIT)) {
            // Break loop or close window
        }
        
        if (renderer.isActionActive(GameAction::SHOOT)) {
            // Create projectile entity...
        }

        // 4. Draw Frame
        renderer.render();
    }
}
```