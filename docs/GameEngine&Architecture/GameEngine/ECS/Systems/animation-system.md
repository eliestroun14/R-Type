# AnimationSystem

The `AnimationSystem` handles the visual progression of sprite animations. It updates the texture coordinates (`sf::IntRect`) of entities based on elapsed time, creating the illusion of movement.

## Required Components

Entities processed by this system must have the following components:
1.  **`Animation`**: Contains timing, frame counts, and loop settings.
2.  **`Sprite`**: Contains the visual rectangle (`rect`) that determines which part of the texture is displayed.

## Logic & Algorithm

The system performs the following logic per entity per frame:

1.  **Time Accumulation:**
    * Adds `dt` (Delta Time) to the `elapsedTime` accumulator of the Animation component.

2.  **Frame Advance:**
    * If `elapsedTime` > `frameDuration`, the frame index (`currentFrame`) is incremented.
    * The timer is reset to 0.

3.  **Boundary Check (Looping):**
    * Checks if `currentFrame` exceeds `endFrame`.
    * **If `loop` is true:** Resets `currentFrame` to `startFrame`.
    * **If `loop` is false:** Clamps `currentFrame` to `endFrame` (stops animating).

4.  **Rect Update (Sprite Sheet):**
    * Calculates the new `left` coordinate for the texture rectangle:
        `newLeft = currentFrame * sprite.width`
    * Updates the `Sprite` component so the `RenderSystem` draws the correct image.

### Code reference
[src/game/src/systems/AnimationSystem.cpp](src/game/src/systems/AnimationSystem.cpp#L1-L55)

```cpp
void AnimationSystem::onUpdate(float dt) {
    auto& animations = _engine.getComponents<Animation>();
    auto& sprites = _engine.getComponents<Sprite>();
    for (size_t e : _entities) {
        if (!animations[e] || !sprites[e]) continue;
        auto& anim = animations[e].value();
        auto& sprite = sprites[e].value();
        anim.elapsedTime += dt;
        if (anim.elapsedTime > anim.frameDuration) {
            anim.currentFrame++; anim.elapsedTime = 0;
        }
        if (anim.currentFrame > anim.endFrame)
            anim.currentFrame = anim.loop ? anim.startFrame : anim.endFrame;
        sprite.rect.left = anim.currentFrame * sprite.rect.width;
    }
}
```

## Visual Representation

Assuming a sprite sheet where frames are aligned horizontally:

```text
[Frame 0] [Frame 1] [Frame 2] ...
    ^
    |
  rect.left (shifts every X milliseconds)
```

## Usage

```c++
Entity entity = gameEngine.createEntity("Entity");

gameEngine.addComponent<Sprite(entity, Sprite(ASSET_ID, Z_INDEX, sf::IntRect(0, 0, RECT_WIDTH, RECT_HEIGHT)));
gameEngine.addComponent<Animation>(entity, Animation(
    RECT_WIDTH,
    RECT_HEIGHT,
    CURRENT,
    ELAPSED_TIME,
    DURATION,
    START_FRAME,
    END_FRAME,
    IS_LOOP,
));
```