# ECS Components Reference

Source of truth: [src/engine/include/engine/ecs/component/Components.hpp](src/engine/include/engine/ecs/component/Components.hpp). Components are plain data (no logic); systems consume them.

## Core
- `Transform { float x, y, rotation, scale; }`
- `Velocity { float vx, vy; }`
- `NetworkId { uint32_t id; bool isLocal; }`
- `HitBox {}` (tag; presence only)

## Render
- `Sprite { Assets assetId; ZIndex zIndex; sf::Rect<int> rect; sf::FloatRect globalBounds; }`
- `Animation { int frameWidth, frameHeight, currentFrame; float elapsedTime, frameDuration; int startFrame, endFrame; bool loop; }`
- `Text { std::string data; }`
- `ScrollingBackground { float scrollSpeed, currentOffset; bool horizontal, repeat; }`
- `VisualEffect { protocol::VisualEffectType type; float scale, duration; float color_r, color_g, color_b; }`

## Lifecycle
- `Lifetime { float remainingTime; }`

## Gameplay
- `Health { int currentHealth, maxHp; }`
- `Powerup { PowerupType powerupType; float duration; }` with `PowerupType { SPEED_BOOST, WEAPON_UPGRADE, FORCE, SHIELD, EXTRA_LIFE, INVINCIBILITY, HEAL, UNKOWN }`
- `Projectile { Entity shooterId; bool isFromPlayable; int damage; }`
- `ProjectileType { MISSILE, LASER, UNKNOWN }`
- `Weapon { uint32_t fireRateMs, lastShotTime; int damage; ProjectileType projectileType; }`

## Input & Tags
- `Clickable { bool isClicked; }`
- `Drawable {}` (tag)
- `Playable {}` (tag)
- `InputComponent { uint32_t playerId; std::map<GameAction, bool> activeActions; }`
- `Enemy {}` (tag)

## Behaviour / AI
- `MovementPattern { MovementPatternType patternType; float amplitude; float frequency; std::vector<std::pair<float,float>> positions; float timeElapsed; }`
- `MovementPatternType { LINEAR, SINE_WAVE, CIRCULAR, BEZIER, CHASE_PLAYER }`
- `AI { AiBehaviour aiBehaviour; float detectionRange; float aggroRange; float internalTime; }` with `AiBehaviour { KAMIKAZE, SHOOTER_TACTIC, ZIGZAG, FORMATION }`

## Level / Waves
- `EnemySpawn { EnemyType type; float spawnX, spawnY; float delayAfterPrevious; }` with `EnemyType { BASIC, FAST, TANK, BOSS }`
- `Wave { std::vector<EnemySpawn> enemies; float startTime; }`
- `Level { std::vector<Wave> waves; int currentWaveIndex; float elapsedTime; bool completed; }`

## Player state
- `DeadPlayer { float timer; bool initialized; uint32_t killerId; }`
- `Score { uint32_t score; }`

## Audio
- `AudioEffect { protocol::AudioEffectType type; float volume; float pitch; bool isPlaying; }`
- `AudioSource { AudioAssets assetId; bool loop; float minDistance; float attenuation; bool isUI; }`

## R-Type specific
- `Force { uint32_t parentShipId; ForceAttachmentPoint attachmentPoint; uint8_t powerLevel; uint8_t chargePercentage; bool isFiring; }` with `ForceAttachmentPoint { FORCE_DETACHED, FORCE_FRONT, FORCE_BACK, FORCE_ORBITING }`