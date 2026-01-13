
# GameEngine

Central facade that wires the ECS (entities, components, systems) with rendering and audio. The concrete API lives in [src/engine/include/engine/GameEngine.hpp](src/engine/include/engine/GameEngine.hpp) and delegates to [RenderManager](src/engine/include/engine/render/RenderManager.hpp) for window/input and [AudioManager](src/engine/include/engine/audio/AudioManager.hpp) for sound.

## Responsibilities
- ECS orchestration: owns [EntityManager](src/engine/include/engine/ecs/entity/EntityManager.hpp) and [SystemManager](src/engine/include/engine/ecs/system/SystemManager.hpp), registers component types, wires system signatures, and runs `updateSystems(dt)`.
- Rendering and input: proxies calls to RenderManager (`processInput()`, `isActionActive()`, `beginFrame()`, `render()`, textures, scale factor, window state).
- Audio: proxies to AudioManager for SFX/UI/music playback and listener/volume controls.
- Network alignment: exposes `createEntityWithId()` and `removeComponentByType()` to mirror server-authored IDs and component state when receiving packets.

## Lifecycle
1. `init()` sets up EntityManager and SystemManager and links them.
2. Client-side boot: `initRender()` opens the window and preloads assets; `initAudio()` loads audio resources.
3. Systems bootstrap: register systems, set signatures, then call `onCreateSystems()`.
4. Frame loop (typical client):
   - `processInput()`
   - `beginFrame()`
   - `updateSystems(dt)`
   - optional audio upkeep (`updateAudio()`)
   - `render()`
5. Shutdown: call `onDestroySystems()` then let RAII clean up managers.

## Key APIs
- Entities: `createEntity(name)`, `createEntityWithId(id, name)`, `destroyEntity(id)`, `isAlive(entity)`, `getEntityName(entity)`.
- Components: `registerComponent<T>()`, `addComponent<T>(entity, data)`, `emplaceComponent<T>(entity, args...)`, `updateComponent<T>(entity, data)`, `removeComponent<T>(entity)`, `removeComponentByType(type, entity)`, `getComponents<T>()`, `getComponentEntity<T>(entity)`, `hasComponent<T>(entity)`.
- Systems: `registerSystem<S>()`, `getSystem<S>()`, `removeSystem<S>()`, `setSystemSignature<S, Components...>()`, `updateSystems(dt)`, `onCreateSystems()`, `onDestroySystems()`.
- Input/Render: `processInput()`, `isActionActive(action)`, `isActionJustPressed(action)`, `getMapAction()`, `getMousePosition()`, `getWindow()`, `getTexture(assetId)`, `getScaleFactor()`, `beginFrame()`, `render()`, `isWindowOpen()`.
- Audio: `playSound(effect, x, y, volume, pitch)`, `playSoundUI(effect, volume, pitch)`, `playMusic(path, volume)`, `stopMusic()`, `updateAudio()`, `setMasterVolume()`, `setSoundVolume()`, `setMusicVolume()`, `getAudioManager()`.

## Usage Skeleton (client side)

```cpp
gameEngine::GameEngine engine;
engine.init();
engine.initRender();
engine.initAudio();

// register components and systems, set signatures, then:
engine.onCreateSystems();

while (engine.isWindowOpen()) {
	engine.processInput();
	engine.beginFrame();
	engine.updateSystems(dt);
	engine.updateAudio();
	engine.render();
}

engine.onDestroySystems();
```
