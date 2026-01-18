/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Stub GameEngine for Game.cpp coverage tests
*/

#ifndef TESTS_STUB_GAME_ENGINE_HPP_
#define TESTS_STUB_GAME_ENGINE_HPP_

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <engine/ecs/entity/Entity.hpp>
#include <engine/ecs/component/Components.hpp>
#include <common/protocol/Protocol.hpp>

namespace gameEngine {

class GameEngine {
public:
    Entity createEntity(const std::string&) {
        std::size_t id = _nextEntityId++;
        ensureSize(_configs, id + 1);
        ensureSize(_transforms, id + 1);
        ensureSize(_levels, id + 1);
        ensureSize(_sprites, id + 1);
        ensureSize(_texts, id + 1);
        ensureSize(_buttons, id + 1);
        ensureSize(_animations, id + 1);
        ensureSize(_scrollingBackgrounds, id + 1);
        ensureSize(_rebinds, id + 1);
        ensureSize(_audioSources, id + 1);
        return Entity::fromId(id);
    }

    template <typename T, typename... Args>
    void addComponent(Entity entity, Args&&... args) {
        std::size_t index = static_cast<std::size_t>(entity);
        if constexpr (std::is_same_v<T, GameConfig>) {
            ensureSize(_configs, index + 1);
            _configs[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, Transform>) {
            ensureSize(_transforms, index + 1);
            _transforms[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, Sprite>) {
            ensureSize(_sprites, index + 1);
            _sprites[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, Text>) {
            ensureSize(_texts, index + 1);
            _texts[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, ButtonComponent>) {
            ensureSize(_buttons, index + 1);
            _buttons[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, Animation>) {
            ensureSize(_animations, index + 1);
            _animations[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, ScrollingBackground>) {
            ensureSize(_scrollingBackgrounds, index + 1);
            _scrollingBackgrounds[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, Rebind>) {
            ensureSize(_rebinds, index + 1);
            _rebinds[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, AudioSource>) {
            ensureSize(_audioSources, index + 1);
            _audioSources[index] = T(std::forward<Args>(args)...);
        } else if constexpr (std::is_same_v<T, Level>) {
            ensureSize(_levels, index + 1);
            _levels[index] = T(std::forward<Args>(args)...);
        }
    }

    template <typename T>
    std::vector<std::optional<T>>& getComponents() {
        if constexpr (std::is_same_v<T, GameConfig>) {
            return _configs;
        } else if constexpr (std::is_same_v<T, Transform>) {
            return _transforms;
        } else if constexpr (std::is_same_v<T, Sprite>) {
            return _sprites;
        } else if constexpr (std::is_same_v<T, Text>) {
            return _texts;
        } else if constexpr (std::is_same_v<T, ButtonComponent>) {
            return _buttons;
        } else if constexpr (std::is_same_v<T, Animation>) {
            return _animations;
        } else if constexpr (std::is_same_v<T, ScrollingBackground>) {
            return _scrollingBackgrounds;
        } else if constexpr (std::is_same_v<T, Rebind>) {
            return _rebinds;
        } else if constexpr (std::is_same_v<T, AudioSource>) {
            return _audioSources;
        } else {
            return _levels;
        }
    }

    template <typename T>
    std::optional<T>& getComponentEntity(Entity entity) {
        std::size_t index = static_cast<std::size_t>(entity);
        auto& components = getComponents<T>();
        ensureSize(components, index + 1);
        return components[index];
    }

    Entity getEntityFromId(uint32_t id) {
        return Entity::fromId(id);
    }

    void updateSystems(float dt) {
        lastUpdateDt = dt;
        ++updateCalls;
    }

    void processInput() {
        ++processInputCalls;
    }

    bool isWindowOpen() const {
        return windowOpen;
    }

    void beginFrame() {
        ++beginFrameCalls;
    }

    void render() {
        ++renderCalls;
    }

    void playSound(protocol::AudioEffectType effectType, float x, float y, float volume, float pitch) {
        lastSoundEffect = effectType;
        lastSoundX = x;
        lastSoundY = y;
        lastSoundVolume = volume;
        lastSoundPitch = pitch;
        ++playSoundCalls;
    }

    void playSoundUI(protocol::AudioEffectType effectType, float volume, float pitch) {
        lastUISoundEffect = effectType;
        lastUISoundVolume = volume;
        lastUISoundPitch = pitch;
        ++playSoundUICalls;
    }

    void setWindowOpen(bool open) {
        windowOpen = open;
    }

    float getScaleFactor() const {
        return scaleFactor;
    }

    void setScaleFactor(float factor) {
        scaleFactor = factor;
    }

    sf::Vector2i getMousePosition() const {
        return mousePosition;
    }

    void setMousePosition(const sf::Vector2i& pos) {
        mousePosition = pos;
    }

    bool isActionActive(GameAction action) const {
        if (action == GameAction::LEFT_CLICK) {
            return leftClickHeld;
        }
        return false;
    }

    bool isActionJustReleased(GameAction action) const {
        if (action == GameAction::LEFT_CLICK) {
            return leftClickReleased;
        }
        return false;
    }

    void setLeftClickHeld(bool held) {
        leftClickHeld = held;
    }

    void setLeftClickReleased(bool released) {
        leftClickReleased = released;
    }

    int updateCalls = 0;
    int processInputCalls = 0;
    int beginFrameCalls = 0;
    int renderCalls = 0;
    float lastUpdateDt = 0.0f;
    int playSoundCalls = 0;
    int playSoundUICalls = 0;
    protocol::AudioEffectType lastSoundEffect = protocol::AudioEffectType::SFX_SHOOT_BASIC;
    protocol::AudioEffectType lastUISoundEffect = protocol::AudioEffectType::SFX_SHOOT_BASIC;
    float lastSoundX = 0.0f;
    float lastSoundY = 0.0f;
    float lastSoundVolume = 0.0f;
    float lastSoundPitch = 0.0f;
    float lastUISoundVolume = 0.0f;
    float lastUISoundPitch = 0.0f;

private:
    template <typename T>
    void ensureSize(std::vector<std::optional<T>>& vec, std::size_t size) {
        if (vec.size() < size) {
            vec.resize(size);
        }
    }

    std::size_t _nextEntityId = 1;
    bool windowOpen = true;
    float scaleFactor = 1.0f;
    sf::Vector2i mousePosition{0, 0};
    bool leftClickHeld = false;
    bool leftClickReleased = false;

    std::vector<std::optional<GameConfig>> _configs;
    std::vector<std::optional<Transform>> _transforms;
    std::vector<std::optional<Sprite>> _sprites;
    std::vector<std::optional<Text>> _texts;
    std::vector<std::optional<ButtonComponent>> _buttons;
    std::vector<std::optional<Animation>> _animations;
    std::vector<std::optional<ScrollingBackground>> _scrollingBackgrounds;
    std::vector<std::optional<Rebind>> _rebinds;
    std::vector<std::optional<AudioSource>> _audioSources;
    std::vector<std::optional<Level>> _levels;
};

} // namespace gameEngine

#endif /* TESTS_STUB_GAME_ENGINE_HPP_ */
