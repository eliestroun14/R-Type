# Audio System Documentation - R-Type

<!-- ## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [AudioStorage](#audiostorage)
4. [AudioManager](#audiomanager)
5. [Engine Integration](#engine-integration)
6. [Usage Guide](#usage-guide)
7. [Optimization and Performance](#optimization-and-performance)
8. [Troubleshooting](#troubleshooting)

--- -->

## Overview

The R-Type audio system is designed to efficiently manage sound effects and music in a real-time multiplayer game environment. It uses **SFML Audio** and implements a pooling system for performance optimization.

### Key Features

- **3D Positioned Sounds**: Spatial attenuation based on distance
- **Non-positioned UI Sounds**: For menus and interfaces
- **Music Management**: Audio streaming with looping
- **Object Pooling**: sf::Sound object reuse to avoid allocations
- **Volume Control**: Independent Master, SFX, and Music volumes
- **Asset Preloading**: All sounds loaded at startup

---

## Architecture
```
┌────────────────────────────────────────────────────────────┐
│                         GameEngine                         │
│  ┌────────────────────────────────────────────────────┐    │
│  │              Audio Interface Layer                 │    │
│  │  - playSound(type, x, y)                           │    │
│  │  - playSoundUI(type)                               │    │
│  │  - playMusic(filepath)                             │    │
│  └────────────────┬───────────────────────────────────┘    │
└───────────────────┼────────────────────────────────────────┘
                    │
                    ▼
┌────────────────────────────────────────────────────────────┐
│                      AudioManager                          │
│  ┌────────────────────────────────────────────────────┐    │
│  │           Audio Processing & Control               │    │
│  │  - Sound positioning (3D)                          │    │
│  │  - Volume management                               │    │
│  │  - Sound pool management                           │    │
│  │  - Listener position                               │    │
│  └────────────────┬───────────────────────────────────┘    │
│                   │                                        │
│  ┌────────────────▼───────────────────────────────────┐    │
│  │              AudioStorage                          │    │
│  │  - SoundBuffer storage                             │    │
│  │  - Asset preloading                                │    │
│  │  - Buffer retrieval                                │    │
│  └────────────────────────────────────────────────────┘    │
└────────────────────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────────────┐
│                      SFML Audio                             │
│  ┌─────────────────┐  ┌─────────────────┐                   │
│  │  sf::Sound      │  │   sf::Music     │                   │
│  │  (pooled x32)   │  │   (streaming)   │                   │
│  └─────────────────┘  └─────────────────┘                   │
└─────────────────────────────────────────────────────────────┘
```

<!-- ### Data Flow
```
User Request (playSound)
    │
    ▼
Engine::playSound()
    │
    ▼
AudioManager::playSound3D()
    │
    ├─ mapProtocolToAsset() ─── Get AudioAssets enum
    │
    ├─ AudioStorage::getSoundBuffer() ─── Get sf::SoundBuffer
    │
    ├─ getAvailableSound() ─── Get free sf::Sound from pool
    │
    └─ Configure & play sound (position, volume, pitch)
``` -->

---

## AudioStorage

### Description

`AudioStorage` is responsible for loading and centrally managing all audio buffers in the game. It acts as a repository for **audio assets**.

### `AudioStorage.hpp`
```cpp
#ifndef AUDIOSTORAGE_HPP_
#define AUDIOSTORAGE_HPP_

#include <SFML/Audio.hpp>
#include <common/constants/render/Assets.hpp>
#include <vector>
#include <memory>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>

/**
 * @class AudioStorage
 * @brief Centralized storage for all game audio buffers
 *
 * AudioStorage manages the loading and storage of all sound buffers used
 * in the game. It preloads all audio assets at initialization to avoid
 * loading delays during gameplay.
 *
 * @note This class uses the Repository pattern for asset management
 * @see AudioAssets enum for available audio assets
 */
class AudioStorage {
public:
    /**
     * @brief Default constructor
     */
    AudioStorage() = default;

    /**
     * @brief Initialize the audio storage and load all sound buffers
     *
     * This method loads all audio files defined in the pathAudioAssets array.
     * It should be called once during game initialization.
     *
     * @throws Error if any audio file fails to load
     *
     * @example
     * @code
     * AudioStorage storage;
     * storage.init(); // Loads all audio files
     * @endcode
     */
    void init()
    {
        // Reserve space to avoid useless reallocations
        this->_soundBuffers.reserve(NUMBER_AUDIO_ASSETS);

        for (size_t i = 0; i < NUMBER_AUDIO_ASSETS; i++) {
            auto buffer = std::make_shared<sf::SoundBuffer>();
            if (!buffer->loadFromFile(pathAudioAssets[i])) {
                std::cerr << "Failed to load audio: " << pathAudioAssets[i] << std::endl;
                throw Error(ErrorType::ResourceLoadFailure, 
                           ErrorMessages::RESOURCE_LOAD_FAILURE);
            }
            this->_soundBuffers.push_back(std::move(buffer));
            std::cout << "[AudioStorage] Loaded: " << pathAudioAssets[i] << std::endl;
        }
    }

    /**
     * @brief Retrieve a sound buffer by its asset ID
     *
     * @param id The AudioAssets enum value identifying the sound
     * @return std::shared_ptr<sf::SoundBuffer> Shared pointer to the sound buffer,
     *         or nullptr if the ID is invalid
     *
     * @note The returned buffer is shared and should not be modified
     *
     * @example
     * @code
     * auto buffer = storage.getSoundBuffer(AudioAssets::SFX_SHOOT_BASIC);
     * if (buffer) {
     *     sound.setBuffer(*buffer);
     * }
     * @endcode
     */
    std::shared_ptr<sf::SoundBuffer> getSoundBuffer(AudioAssets id) const
    {
        if (id < 0 || id >= this->_soundBuffers.size())
            return nullptr;
        return this->_soundBuffers[id];
    }

private:
    /**
     * @brief Storage container for all sound buffers
     *
     * Vector of shared pointers to sf::SoundBuffer objects.
     * Indexed by AudioAssets enum values.
     */
    std::vector<std::shared_ptr<sf::SoundBuffer>> _soundBuffers;
};

#endif /* !AUDIOSTORAGE_HPP_ */
```

### Usage

#### Initialization
```cpp
AudioStorage storage;
storage.init(); // Load all sounds from disk
```

#### Retrieving a buffer
```cpp
auto buffer = storage.getSoundBuffer(AudioAssets::SFX_EXPLOSION_LARGE_1);
if (buffer) {
    sound.setBuffer(*buffer);
} else {
    // Error handling: invalid buffer
}
```

<!--
### Technical Specifications

| Property | Value |
|----------|-------|
| **Storage type** | `std::vector<std::shared_ptr<sf::SoundBuffer>>` |
| **Loading** | Full preloading at `init()` |
| **Access** | O(1) by index |
| **Memory** | ~1-5 MB per sound (depending on quality) |
| **Thread-safety** | Read thread-safe, write non-thread-safe | -->

### Important Points

**Warnings**:
- All audio files must exist before calling `init()`
- Paths are defined in `pathAudioAssets` (**`Assets.hpp`**)
- Loading errors throw an `Error` exception

**Best Practices**:
- Call `init()` only once at startup
- Use `.wav` files for SFX (uncompressed)
- Use `.ogg` files for music (compressed)
- Limit SFX file size to < 1 MB

---

## AudioManager

### Description

`AudioManager` is the core of the audio system. It manages sound playback, music, 3D positioning, volumes, and sf::Sound object pooling.

### File: `AudioManager.hpp`
```cpp
/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** AudioManager
*/

#ifndef AUDIOMANAGER_HPP_
#define AUDIOMANAGER_HPP_

#include <SFML/Audio.hpp>
#include <common/protocol/AudioEffect.hpp>
#include <common/constants/render/Assets.hpp>
#include <memory>
#include <vector>
#include "AudioStorage.hpp"

namespace audio {

/**
 * @class AudioManager
 * @brief Central audio management system for the game
 *
 * AudioManager handles all audio playback, including:
 * - 3D positioned sound effects with distance attenuation
 * - Non-positioned UI sounds
 * - Background music with looping
 * - Volume control (master, sound, music)
 * - Audio listener positioning for 3D audio
 * - Sound pooling for performance optimization
 *
 * The manager uses an object pool of 32 sf::Sound objects to avoid
 * runtime memory allocations and improve performance.
 *
 * @note This class is designed for single-threaded use
 */
class AudioManager {
public:
    /**
     * @brief Constructor - Initializes SFML audio and sound pool
     *
     * Sets up the SFML audio listener with default 3D audio parameters
     * and pre-allocates 32 sf::Sound objects in the sound pool.
     */
    AudioManager();

    /**
     * @brief Destructor
     */
    ~AudioManager() = default;

    /**
     * @brief Initialize the audio manager and load all audio assets
     *
     * This method initializes the AudioStorage and loads all sound buffers.
     * Must be called before any audio playback.
     *
     * @throws Error if audio loading fails
     *
     * @example
     * @code
     * AudioManager audioMgr;
     * audioMgr.init(); // Load all sounds
     * @endcode
     */
    void init();

    /**
     * @brief Play a 3D positioned sound effect
     *
     * Plays a sound at a specific position in the game world with distance-based
     * attenuation. The sound will be louder when close to the listener and
     * quieter when far away.
     *
     * @param type The audio effect type (from protocol)
     * @param x X coordinate of the sound source
     * @param y Y coordinate of the sound source
     * @param volume Volume multiplier (0.0 = silent, 1.0 = full volume)
     * @param pitch Pitch multiplier (1.0 = normal, 2.0 = double speed/octave up)
     *
     * @note The sound uses the sound pool and may override the oldest playing sound
     *       if all 32 slots are occupied
     *
     * @example
     * @code
     * // Play explosion at position (100, 200) with full volume
     * audioMgr.playSound3D(AudioEffectType::SFX_EXPLOSION_LARGE_1, 100, 200, 1.0f, 1.0f);
     *
     * // Play laser with slight pitch variation
     * audioMgr.playSound3D(AudioEffectType::SFX_SHOOT_LASER, x, y, 0.8f, 1.05f);
     * @endcode
     */
    void playSound3D(protocol::AudioEffectType type, float x, float y, 
                     float volume = 1.0f, float pitch = 1.0f);

    /**
     * @brief Play a non-positioned UI sound effect
     *
     * Plays a sound without 3D positioning, suitable for UI elements, menus,
     * and global game events. The sound is always at full stereo with no
     * distance attenuation.
     *
     * @param type The audio effect type (from protocol)
     * @param volume Volume multiplier (0.0 = silent, 1.0 = full volume)
     * @param pitch Pitch multiplier (1.0 = normal)
     *
     * @example
     * @code
     * // Play menu selection sound
     * audioMgr.playSoundUI(AudioEffectType::SFX_MENU_SELECT, 1.0f, 1.0f);
     *
     * // Play alert with higher pitch
     * audioMgr.playSoundUI(AudioEffectType::SFX_MENU_ALERT, 0.9f, 1.2f);
     * @endcode
     */
    void playSoundUI(protocol::AudioEffectType type, 
                     float volume = 1.0f, float pitch = 1.0f);

    /**
     * @brief Play background music with looping
     *
     * Starts playing a music file. The music will loop indefinitely until
     * stopped. Only one music track can play at a time; calling this method
     * will stop any currently playing music.
     *
     * @param filepath Path to the music file (relative or absolute)
     * @param volume Volume level (0.0 = silent, 1.0 = full volume)
     *
     * @note Music is streamed from disk rather than loaded into memory
     * @note Recommended format: .ogg (Vorbis) for good quality/size ratio
     *
     * @example
     * @code
     * // Play level 1 music at 40% volume
     * audioMgr.playMusic("../assets/music/level_1.ogg", 0.4f);
     * @endcode
     */
    void playMusic(const std::string& filepath, float volume = 0.5f);

    /**
     * @brief Stop the currently playing music
     *
     * Immediately stops the background music. Safe to call even if no music
     * is playing.
     *
     * @example
     * @code
     * audioMgr.stopMusic();
     * @endcode
     */
    void stopMusic();

    /**
     * @brief Update the 3D audio listener position
     * 
     * Sets the position of the audio listener (typically the player's position).
     * This affects how 3D positioned sounds are heard - sounds closer to the
     * listener will be louder.
     * 
     * @param x X coordinate of the listener
     * @param y Y coordinate of the listener
     * 
     * @note Should be called every frame with the player's current position
     * 
     * @example
     * @code
     * // In game loop
     * auto playerPos = player.getPosition();
     * audioMgr.setListenerPosition(playerPos.x, playerPos.y);
     * @endcode
     */
    void setListenerPosition(float x, float y);

    /**
     * @brief Set the master volume (affects all audio)
     * 
     * @param volume Volume level (0.0 = silent, 1.0 = full volume)
     * 
     * @note Values are automatically clamped to [0.0, 1.0]
     * 
     * @example
     * @code
     * audioMgr.setMasterVolume(0.7f); // 70% global volume
     * @endcode
     */
    void setMasterVolume(float volume);

    /**
     * @brief Set the sound effects volume
     * 
     * @param volume Volume level (0.0 = silent, 1.0 = full volume)
     * 
     * @note Does not affect music volume
     * @note Values are automatically clamped to [0.0, 1.0]
     * 
     * @example
     * @code
     * audioMgr.setSoundVolume(0.5f); // 50% SFX volume
     * @endcode
     */
    void setSoundVolume(float volume);

    /**
     * @brief Set the music volume
     * 
     * @param volume Volume level (0.0 = silent, 1.0 = full volume)
     * 
     * @note Does not affect sound effects volume
     * @note Values are automatically clamped to [0.0, 1.0]
     * @note Immediately affects currently playing music
     * 
     * @example
     * @code
     * audioMgr.setMusicVolume(0.3f); // 30% music volume
     * @endcode
     */
    void setMusicVolume(float volume);

    /**
     * @brief Update audio systems
     * 
     * Performs housekeeping tasks such as cleaning up finished sounds.
     * 
     * @note Currently optional as SFML handles cleanup automatically,
     *       but provided for future enhancements
     */
    void update();

    /**
     * @brief Get a sound buffer by asset ID
     * 
     * Direct access to the underlying AudioStorage for advanced use cases.
     * 
     * @param id The AudioAssets enum value
     * @return std::shared_ptr<sf::SoundBuffer> The sound buffer, or nullptr if invalid
     */
    std::shared_ptr<sf::SoundBuffer> getSoundBuffer(AudioAssets id) const;

private:
    /**
     * @brief Storage for all audio buffers
     */
    AudioStorage _audioStorage;

    /**
     * @brief Pool of reusable sf::Sound objects
     * 
     * Preallocated pool of 32 sound objects to avoid runtime allocations.
     * Sounds are reused when they finish playing.
     */
    std::vector<std::unique_ptr<sf::Sound>> _activeSounds;

    /**
     * @brief Background music player
     * 
     * Only one music instance exists as only one track plays at a time.
     */
    std::unique_ptr<sf::Music> _music;

    /**
     * @brief Master volume multiplier (0.0 - 1.0)
     * 
     * Affects all audio (sounds and music).
     */
    float _masterVolume = 1.0f;

    /**
     * @brief Sound effects volume multiplier (0.0 - 1.0)
     * 
     * Only affects sound effects, not music.
     */
    float _soundVolume = 1.0f;

    /**
     * @brief Music volume multiplier (0.0 - 1.0)
     * 
     * Only affects background music, not sound effects.
     */
    float _musicVolume = 0.5f;

    /**
     * @brief Current listener position in 2D space
     */
    sf::Vector2f _listenerPosition;

    /**
     * @brief Get an available sound from the pool
     * 
     * Searches for a sound that is not currently playing. If all sounds
     * are in use, stops the oldest sound and returns it.
     * 
     * @return sf::Sound* Pointer to an available sound object
     */
    sf::Sound* getAvailableSound();

    /**
     * @brief Map protocol audio type to asset ID
     * 
     * Converts a protocol::AudioEffectType to an AudioAssets enum value.
     * 
     * @param type Protocol audio effect type
     * @return AudioAssets Corresponding asset ID
     */
    AudioAssets mapProtocolToAsset(protocol::AudioEffectType type) const;
};

} // namespace audio

#endif /* !AUDIOMANAGER_HPP_ */
```

### Technical Specifications

| Property | Value |
|----------|-------|
| **Pool size** | 32 sf::Sound max simultaneous |
| **Attenuation** | 0.01 (distance) |
| **Min distance** | 50.0 units |
| **Volume range** | 0.0 - 1.0 (clamped) |
| **Pitch range** | 0.0 - ∞ (recommended: 0.5 - 2.0) |
| **Music format** | Streaming (sf::Music) |

---

## Engine Integration

### GameEngine.hpp
```cpp
class Engine {
private:
    std::unique_ptr<audio::AudioManager> _audioManager;

public:
    Engine() {
        _audioManager = std::make_unique<audio::AudioManager>();
    }

    void init() {
        _audioManager->init();
        // Optionally start menu music
        _audioManager->playMusic("../assets/music/main_menu.ogg", 0.3f);
    }

    // ################################################################
    // ########################### AUDIO ##############################
    // ################################################################

    /**
     * @brief Play a positioned 3D sound effect
     * @param type The audio effect type
     * @param x,y The position of the sound
     * @param volume Volume multiplier (0.0 - 1.0)
     * @param pitch Pitch multiplier (1.0 = normal)
     */
    void playSound(protocol::AudioEffectType type, float x, float y, 
                   float volume = 1.0f, float pitch = 1.0f)
    {
        this->_audioManager->playSound3D(type, x, y, volume, pitch);
    }

    /**
     * @brief Play a non-positioned UI sound effect
     * @param type The audio effect type
     * @param volume Volume multiplier (0.0 - 1.0)
     * @param pitch Pitch multiplier (1.0 = normal)
     */
    void playSoundUI(protocol::AudioEffectType type, 
                     float volume = 1.0f, float pitch = 1.0f)
    {
        this->_audioManager->playSoundUI(type, volume, pitch);
    }

    /**
     * @brief Play background music (stops previous music)
     * @param filepath Path to the music file
     * @param volume Volume level (0.0 - 1.0)
     */
    void playMusic(const std::string& filepath, float volume = 0.5f)
    {
        this->_audioManager->playMusic(filepath, volume);
    }

    /**
     * @brief Stop the currently playing music
     */
    void stopMusic()
    {
        this->_audioManager->stopMusic();
    }

    /**
     * @brief Update the audio listener position (usually the player's position)
     * @param x,y The listener position
     */
    void updateAudioListener(float x, float y)
    {
        this->_audioManager->setListenerPosition(x, y);
    }

    /**
     * @brief Set the master volume (affects all audio)
     * @param volume Volume level (0.0 - 1.0)
     */
    void setMasterVolume(float volume)
    {
        this->_audioManager->setMasterVolume(volume);
    }

    /**
     * @brief Set the sound effects volume
     * @param volume Volume level (0.0 - 1.0)
     */
    void setSoundVolume(float volume)
    {
        this->_audioManager->setSoundVolume(volume);
    }

    /**
     * @brief Set the music volume
     * @param volume Volume level (0.0 - 1.0)
     */
    void setMusicVolume(float volume)
    {
        this->_audioManager->setMusicVolume(volume);
    }

    /**
     * @brief Get the audio manager instance
     * @return Pointer to the audio manager
     */
    audio::AudioManager* getAudioManager()
    {
        return this->_audioManager.get();
    }

    /**
     * @brief Update audio systems (cleanup finished sounds, etc.)
     */
    void updateAudio()
    {
        this->_audioManager->update();
    }
};
```

---

## Usage Guide

### 1. System Initialization
```cpp
// In main.cpp or game initialization
Engine engine;
engine.init(); // Load all sounds and start music
```

### 2. Play a Positioned Sound (3D)
```cpp
// Laser shot at ship position
auto& pos = getComponent<Position>(ship);
engine.playSound(
    protocol::AudioEffectType::SFX_SHOOT_LASER,
    pos.x, pos.y,     // Position
    1.0f,             // Volume (100%)
    1.0f              // Normal pitch
);
```

### 3. Play a UI Sound
```cpp
// Menu button click
engine.playSoundUI(
    protocol::AudioEffectType::SFX_MENU_SELECT,
    0.8f,   // Volume (80%)
    1.0f    // Normal pitch
);
```

### 4. Music Management
```cpp
// Start level 1 music
engine.playMusic("../assets/music/level_1.ogg", 0.4f);

// Change music (automatically stops the old one)
engine.playMusic("../assets/music/level_2.ogg", 0.4f);

// Stop music
engine.stopMusic();
```

### 5. Update Listener (in game loop)
```cpp
void GameLoop::update(float deltaTime)
{
    // ... other updates ...
    
    // Update player's audio position
    if (localPlayer) {
        auto& pos = engine.getComponent<Position>(localPlayer);
        engine.updateAudioListener(pos.x, pos.y);
    }
    
    // Update audio systems
    engine.updateAudio();
}
```

### 6. Options Menu (volumes)
```cpp
void OptionsMenu::onMasterVolumeSliderChanged(float value)
{
    // value: 0.0 - 1.0 from slider
    engine.setMasterVolume(value);
}

void OptionsMenu::onSoundVolumeSliderChanged(float value)
{
    engine.setSoundVolume(value);
}

void OptionsMenu::onMusicVolumeSliderChanged(float value)
{
    engine.setMusicVolume(value);
}
```

### 7. Advanced Examples

#### Pitch Variation to Avoid Repetition
```cpp
void WeaponSystem::fire()
{
    // Random pitch between 0.95 and 1.05
    float randomPitch = 0.95f + (rand() % 10) / 100.0f;
    
    engine.playSound(
        protocol::AudioEffectType::SFX_SHOOT_BASIC,
        x, y,
        1.0f,
        randomPitch
    );
}
```

#### Random Sounds Among Multiple Variations
```cpp
void ExplosionSystem::explode(float x, float y, bool large)
{
    if (large) {
        // Randomly choose between 2 large explosions
        auto type = (rand() % 2 == 0) 
            ? protocol::AudioEffectType::SFX_EXPLOSION_LARGE_1
            : protocol::AudioEffectType::SFX_EXPLOSION_LARGE_2;
        engine.playSound(type, x, y, 1.0f, 1.0f);
    } else {
        // Randomly choose between 2 small explosions
        auto type = (rand() % 2 == 0)
            ? protocol::AudioEffectType::SFX_EXPLOSION_SMALL_1
            : protocol::AudioEffectType::SFX_EXPLOSION_SMALL_2;
        engine.playSound(type, x, y, 0.8f, 1.0f);
    }
}
```

#### Manual Distance-Based Attenuation
```cpp
void playDistantSound(float soundX, float soundY, 
                      float listenerX, float listenerY)
{
    float dx = soundX - listenerX;
    float dy = soundY - listenerY;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    float maxDistance = 500.0f;
    if (distance > maxDistance) return; // Too far
    
    float volume = 1.0f - (distance / maxDistance);
    
    engine.playSound(
        protocol::AudioEffectType::SFX_BOSS_ROAR,
        soundX, soundY,
        volume,
        1.0f
    );
}
```

---

## Optimization and Performance

### Object Pooling

The system uses a pool of 32 pre-allocated `sf::Sound` objects:

**Advantages**:
- No allocation/deallocation during gameplay
- Constant and predictable performance
- Avoids memory fragmentation
- Reduces latency spikes

**Limitations**:
- **Maximum 32 simultaneous sounds**
- If exceeded, the oldest sound is stopped

<!-- ### Memory Management

| Component | Estimated Size | Notes |
|-----------|----------------|-------|
| sf::SoundBuffer (1 sound) | ~500 KB - 2 MB | Depends on duration and quality |
| sf::Sound (pool 32) | ~10 KB | Very lightweight |
| sf::Music | ~100 KB | Streaming, not loaded in RAM |
| **Total (24 sounds)** | **~20-50 MB** | Reasonable for a modern game | -->

### Recommendations

1. **Audio Format**:
   - Short SFX: `.wav` 16-bit, 44.1 kHz, mono
   - Music: `.ogg` Vorbis, quality 6-7, stereo

2. **Sound Duration**:
   - SFX: < 3 seconds
   - Music: 2-5 minutes (loop)

3. **Number of Sounds**:
   - 20-30 different SFX
   - 5-10 music tracks

4. **Pool Size**:
   - 32 simultaneous sounds = optimal for R-Type
   - If needed, modify in constructor

---

## Troubleshooting

### Problem: Sound Not Playing

**Symptoms**: No audible sound

**Solutions**:
1. Check that `init()` was called
2. Verify file paths in `pathAudioAssets`
3. Ensure audio files exist and are valid
4. Check volume levels (not set to 0)
5. Verify sound format (SFML supports: WAV, OGG, FLAC)
```cpp
// Debug code
std::cout << "Master Volume: " << masterVolume << std::endl;
std::cout << "Sound Volume: " << soundVolume << std::endl;
auto buffer = audioMgr.getSoundBuffer(AudioAssets::SFX_SHOOT_BASIC);
std::cout << "Buffer loaded: " << (buffer != nullptr) << std::endl;
```

### Problem: Sound Cuts Off

**Symptoms**: Sound stops playing prematurely

**Cause**: All 32 sound slots are occupied

**Solutions**:
1. Increase pool size in AudioManager constructor
2. Reduce number of simultaneous sounds
3. Use shorter sound effects