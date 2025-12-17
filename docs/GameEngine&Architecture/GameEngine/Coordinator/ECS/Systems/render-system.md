# RenderSystem

The `RenderSystem` is responsible for drawing all visual entities to the SFML window. It implements a **Painter's Algorithm** (Z-Ordering) to ensure correct layering of game elements (Background < Game < UI).

## Required Components

Entities processed by this system must have:
1.  **`Transform`**: World position, rotation, and scale.
2.  **`Sprite`**: Texture ID and Z-Index.

## Logic & Algorithm

The system performs a full render pass every frame:

1.  **Sorting (Z-Ordering):**
    * Copies all valid entities into a local buffer `_sortedEntities`.
    * Sorts them based on `Sprite.zIndex` to handle depth.

2.  **Global Scaling:**
    * Calculates a dynamic `scale` factor based on the window size relative to a reference resolution (1920x1080).
    * Ensures the game looks correct on any screen size.

3.  **Drawing:**
    * Retrieves the texture from the Coordinator (Asset Manager).
    * **Standard Sprite:** Applies Position, Rotation, and Scale, then draws.
    * **Scrolling Background:**
        * If `ScrollingBackground.repeat` is true, it sets the texture to `Repeated` mode.
        * Expands the texture rectangle width by 3x (`width * 3`) to ensure the screen is always covered during scrolling.

## Visual Layering



```text
Layer 0 (Background)  --> Drawn First
Layer 1 (Game Entities) --> Drawn Second (Over BG)
Layer 2 (UI/HUD)      --> Drawn Last (On Top)
```

## Usage

**Automatic**. Ensure entities have **Sprite** and **Transform**.