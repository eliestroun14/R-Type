/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Stub Components for Game.cpp coverage tests
*/

#ifndef TESTS_STUB_COMPONENTS_HPP_
#define TESTS_STUB_COMPONENTS_HPP_

#include <cstdint>
#include <vector>
#include <optional>
#include <string>
#include <functional>
#include <cstring>

#include <common/constants/render/Assets.hpp>
#include <common/constants/defines.hpp>
#include <SFML/Graphics.hpp>
#include <engine/ecs/entity/Entity.hpp>

struct GameConfig {
    FontAssets activeFont;
    bool musicEnabled;
    bool soundEnabled;

    GameConfig(FontAssets font, bool music, bool sound)
        : activeFont(font), musicEnabled(music), soundEnabled(sound) {}
};

struct Transform {
    float x;
    float y;
    float rotation;
    float scale;

    Transform(float xx, float yy, float rot, float sca)
        : x(xx), y(yy), rotation(rot), scale(sca) {}
};

struct Level {
    bool started = false;
    bool completed = false;
};

enum ZIndex {
    IS_BACKGROUND,
    IS_GAME,
    IS_UI_HUD
};

struct Sprite {
    Assets assetId;
    ZIndex zIndex;
    sf::IntRect rect;
    sf::FloatRect globalBounds;

    Sprite(Assets id, ZIndex z, sf::IntRect r)
        : assetId(id), zIndex(z), rect(r),
          globalBounds(static_cast<float>(r.left), static_cast<float>(r.top),
                       static_cast<float>(r.width), static_cast<float>(r.height)) {}
};

struct Animation {
    int frameWidth = 0;
    int frameHeight = 0;
    int currentFrame = 0;
    float elapsedTime = 0.0f;
    float frameDuration = 0.0f;
    int startFrame = 0;
    int endFrame = 0;
    bool loop = false;

    Animation() = default;
    Animation(int w, int h, int current, float elapsed, float duration,
        int start, int end, bool looping)
        : frameWidth(w), frameHeight(h), currentFrame(current), elapsedTime(elapsed),
          frameDuration(duration), startFrame(start), endFrame(end), loop(looping) {}
};

struct Text {
    char str[128];
    sf::Color color;
    unsigned int size;
    ZIndex zIndex;

    Text(const char* s, sf::Color c, unsigned int sz, ZIndex z)
        : color(c), size(sz), zIndex(z) {
        std::memset(str, 0, sizeof(str));
        if (s) {
            std::strncpy(str, s, sizeof(str) - 1);
        }
    }
};

struct ButtonTextures {
    Assets none;
    Assets hover;
    Assets clicked;
};

enum ButtonState {
    NONE,
    HOVER,
    CLICKED,
};

struct ButtonComponent {
    ButtonTextures textures;
    ButtonState state = ButtonState::NONE;
    std::function<void()> onClick;

    ButtonComponent(ButtonTextures t, std::function<void()> click)
        : textures(t), onClick(click) {}
};

struct ScrollingBackground {
    float scrollSpeed;
    float currentOffset;
    bool horizontal;
    bool repeat;

    ScrollingBackground(float speed, bool isHorizontal, bool shouldRepeat)
        : scrollSpeed(speed), currentOffset(0.0f), horizontal(isHorizontal), repeat(shouldRepeat) {}
};

struct AudioSource {
    AudioAssets assetId;
    bool loop;
    float minDistance;
    float attenuation;
    bool isUI;
    bool hasBeenPlayed;
    float elapsedTimeSincePlay;
    float soundDuration;

    AudioSource(AudioAssets asset, bool looping = false,
                float minDist = 100.0f, float atten = 0.5f, bool ui = false,
                float duration = 1.0f)
        : assetId(asset), loop(looping), minDistance(minDist), attenuation(atten),
          isUI(ui), hasBeenPlayed(false), elapsedTimeSincePlay(0.0f),
          soundDuration(duration) {}
};

enum class GameAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    SHOOT,
    SWITCH_WEAPON,
    USE_POWERUP,
    SPECIAL,
    OPTIONS,
    EXIT,
    RIGHT_CLICK,
    LEFT_CLICK,
    UNKNOW
};

struct Rebind {
    GameAction action;
    bool isWaiting = false;
    Entity associatedText;

    Rebind(GameAction a, Entity textEnt)
        : action(a), isWaiting(false), associatedText(textEnt) {}
};

#endif /* TESTS_STUB_COMPONENTS_HPP_ */
