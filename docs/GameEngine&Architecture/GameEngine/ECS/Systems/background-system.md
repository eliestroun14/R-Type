# BackgroundSystem

The `BackgroundSystem` manages infinite scrolling backgrounds (Parallax effect). It moves background elements and seamlessly resets their position when they move off-screen to create an illusion of an endless world.

## Required Components

Entities processed by this system must have the following components:
1.  **`ScrollingBackground`**: Contains speed, direction, and repetition settings.
2.  **`Transform`**: Defines the current world position.
3.  **`Sprite`**: Used to determine the dimensions (width/height) of the texture for looping calculations.

## Logic & Algorithm

1.  **Movement:**
    * Calculates `scrollAmount` based on `scrollSpeed`, `scaleFactor`, and `dt`.
    * Subtracts this amount from the `currentOffset`.

2.  **Direction Handling:**
    * **Horizontal:** Updates `transform.x`.
    * **Vertical:** Updates `transform.y`.

3.  **Infinite Looping (Reset):**
    * If `repeat` is enabled, the system checks if the texture has moved completely off-screen.
    * **Condition:** `currentOffset <= - (textureSize * scale)`
    * **Action:** Adds the scaled texture size to `currentOffset`, effectively "teleporting" the background back to the start position without visual interruption.

## Diagram: Infinite Scrolling

```text
       [ Viewport ]
            |
[ Bg 1 ] [ Bg 2 ]  <-- Moving Left
   |
(When Bg 1 exits Viewport completely, it snaps behind Bg 2)
```

## USAGE

```c++
// Example: Background moving horizontally
Entity bg = gameEngine.createEntity("SpaceBackground");
gameEngine.addComponent<Transform>(bg, Transform(x, y, SPACE_ROT, SPACE_SCALE));
gameEngine.addComponent<Sprite>(bg, Sprite(ASSET_ID, Z_INDEX, sf::IntRect(0, 0, RECT_WIDTH, RECT_HEIGHT))); // Z-Index 0 (Back)
gameEngine.addComponent<ScrollingBackground>(bg, ScrollingBackground(
    BACKGROUND_SPEED, // Speed
    true,  // Horizontal
    true   // Repeat
));
```