# Audio Components

The audio subsystem uses two complementary components for sound management: `AudioSource` for configuring playback behavior and `AudioEffect` for triggering specific audio events.

## AudioSource

Configuration component that defines how a sound should be played.

### Fields

```cpp
struct AudioSource {
    AudioAssets assetId;          // ID of the audio asset to play
    bool loop;                    // Whether the sound loops continuously
    float minDistance;            // Distance before attenuation applies (spatial audio)
    float attenuation;            // Attenuation factor (higher = less attenuation)
    bool isUI;                    // If true, ignores 3D positioning (menu sounds)
};
```

### Field Descriptions

- **assetId**: Pre-loaded audio asset identifier from the audio resource manager
- **loop**: 
  - `true` for music and ambient sounds
  - `false` for one-shot effects (explosions, laser shots)
- **minDistance**: Defines the radius in which sound is at maximum volume. Beyond this distance, volume decreases.
  - Example: `100.0f` means full volume within 100 units
  - Recommended: 100-200 for game world sounds
- **attenuation**: Logarithmic factor controlling how quickly volume decreases with distance
  - `0.0` = no attenuation (constant volume regardless of distance)
  - `0.5` = gentle falloff (recommended for ambient)
  - `1.0` = moderate falloff (recommended for effects)
  - Values > 1.0 = rapid falloff
- **isUI**: Disables 3D spatial positioning
  - `true` for UI sounds (buttons, menus, notifications)
  - `false` for in-world positional audio

### Used By

- **AudioSystem**: Reads component to initialize playback
- **GameEngine**: `playSound()` and `playSoundUI()` methods

### Example Usage

```cpp
// Positional laser sound
AudioSource laserSound(
    AudioAssets::LASER_SHOT,
    false,                  // Don't loop
    100.0f,                 // Full volume within 100 units
    0.5f,                   // Gentle falloff
    false                   // Positional 3D audio
);

// UI button click (non-positional)
AudioSource buttonClick(
    AudioAssets::UI_CLICK,
    false,                  // Don't loop
    0.0f,                   // N/A (isUI = true)
    0.0f,                   // N/A (isUI = true)
    true                    // UI sound (no 3D)
);
```

---

## AudioEffect

Event component that triggers temporary audio effects (separate from `AudioSource` playback).

### Fields

```cpp
struct AudioEffect {
    protocol::AudioEffectType type;  // Type of effect to apply
    float volume;                     // Volume multiplier (0.0 - 1.0)
    float pitch;                      // Pitch modifier (1.0 = normal, 2.0 = double speed)
    bool isPlaying;                   // Current playback state
};
```

### Field Descriptions

- **type**: Specifies which audio effect to apply
  - Maps to `protocol::AudioEffectType` enum (defined in network protocol)
  - Examples: `EXPLOSION`, `POWERUP_COLLECTED`, `ENEMY_SPAWN`
  
- **volume**: Multiplicative volume control
  - `0.0` = silent
  - `1.0` = normal volume
  - `> 1.0` = amplified (use carefully to avoid distortion)
  
- **pitch**: Playback speed modifier
  - `1.0` = normal playback
  - `2.0` = double speed (2x pitch, higher tone)
  - `0.5` = half speed (lower tone)
  - Useful for dynamic SFX variation
  
- **isPlaying**: Tracks playback state
  - Set by AudioSystem during playback
  - Can be polled to check if effect is still active

### Used By

- **AudioSystem**: Listens for component additions and triggers playback
- **Game Logic**: Damage system, collision detection, powerup collection

### Example Usage

```cpp
// Boss explosion with deep pitch
AudioEffect bossExplosion(
    protocol::AudioEffectType::EXPLOSION,
    1.0f,    // Normal volume
    0.7f,    // Lower pitch (more bass-heavy)
);
gameEngine.addComponent(bossEntity, bossExplosion);

// Quick powerup collection sound
AudioEffect powerupSfx(
    protocol::AudioEffectType::POWERUP_COLLECTED,
    0.8f,    // Slightly quieter
    1.3f,    // Higher pitch (success/victory feel)
);
gameEngine.addComponent(playerEntity, powerupSfx);
```

---

## Architecture

### Separation of Concerns

- **AudioSource**: Defines *how* to play (config, looping, distance)
- **AudioEffect**: Triggers *when* to play (event, temporary)

### Lifecycle

1. **AudioSource** is added once and managed by `AudioSystem`
2. **AudioEffect** is added as needed (on collision, powerup, death) and removed when complete
3. Both can coexist on the same entity (e.g., player with background music + hit sound effect)

### Performance

- AudioSource components are cheap (no per-frame updates once configured)
- AudioEffect components trigger pooled audio objects for efficiency
- Avoid creating excessive `AudioEffect` components per frame
