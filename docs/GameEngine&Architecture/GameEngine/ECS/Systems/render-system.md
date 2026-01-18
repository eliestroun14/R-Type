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
    * Retrieves the texture from the `GameEngine` (Render Manager).
    * **Standard Sprite:** Applies Position, Rotation, and Scale, then draws.
    * **Scrolling Background:**
        * If `ScrollingBackground.repeat` is true, it sets the texture to `Repeated` mode.
        * Expands the texture rectangle width by 3x (`width * 3`) to ensure the screen is always covered during scrolling.

### Code reference
[src/game/src/systems/RenderSystem.cpp](src/game/src/systems/RenderSystem.cpp#L1-L93)

```cpp
void RenderSystem::onUpdate(float dt) {
    _sortedEntities.assign(_entities.begin(), _entities.end());
    auto& sprites = _engine.getComponents<Sprite>();
    std::sort(_sortedEntities.begin(), _sortedEntities.end(),
        [&sprites](size_t a, size_t b) {
            if (!sprites[a] || !sprites[b]) return a < b;
            return sprites[a]->zIndex < sprites[b]->zIndex;
        });

    sf::RenderWindow& window = _engine.getWindow();
    float scale = std::min(window.getSize().x / WINDOW_WIDTH, window.getSize().y / WINDOW_HEIGHT);
    auto& backgrounds = _engine.getComponents<ScrollingBackground>();
    auto& transforms = _engine.getComponents<Transform>();

    for (size_t entity : _sortedEntities) {
        auto& trans = transforms[entity].value();
        auto& sprite = sprites[entity].value();
        auto tex = _engine.getTexture(sprite.assetId);
        if (!tex) continue;
        bool scrolling = backgrounds[entity].has_value() && backgrounds[entity]->repeat;
        sf::Sprite s(*tex);
        s.setPosition(trans.x, trans.y);
        s.setScale(trans.scale * scale, trans.scale * scale);
        s.setRotation(trans.rotation);
        if (scrolling) {
            tex->setRepeated(true);
            s.setTextureRect(sf::IntRect(0, 0, sprite.rect.width * 3, sprite.rect.height));
        } else if (sprite.rect.width > 0) {
            s.setTextureRect(sprite.rect);
        }
        sprite.globalBounds = s.getGlobalBounds();
        window.draw(s);
    }
}
```

## Visual Layering



```text
Layer 0 (Background)  --> Drawn First
Layer 1 (Game Entities) --> Drawn Second (Over BG)
Layer 2 (UI/HUD)      --> Drawn Last (On Top)
```

## Usage

**Automatic**. Ensure entities have **Sprite** and **Transform**.