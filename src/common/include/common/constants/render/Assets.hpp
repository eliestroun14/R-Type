/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Assets
*/

#ifndef ASSETS_HPP_
#define ASSETS_HPP_

#define NUMBER_ASSETS 38
#define NUMBER_AUDIO_ASSETS 25


#include <array>
#include <iostream>

// ############################################################################
// ############### IF YOU ADD A NEW ASSET, PLEASE UPDATE THE ##################
// ############ MACRO, THE ENUM AND THE ARRAY IN THE SAME ORDER ###############
// ############################################################################

enum Assets {
    // WINDOW
    RTYPE_ICON,

    // BACKGROUND
    STARS_BG,
    GAME_BG,

    // PLAYERS
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    PLAYER_5,

    // ENEMIES
    BASE_ENEMY,

    // BULLET
    CHARCHING_BULLET,
    DEFAULT_BULLET,
    FULL_CHARGED_BULLET,

    // POWERUPS
    PET_POWERUPS,

    // EXPLOSIONS
    EXPLOSIONS,
    SMALL_EXPLOSION,
    MEDIUM_EXPLOSION,
    BIG_EXPLOSION,

    // MENU
    // LOGO
    LOGO_RTYPE,

    // BUTTONS
    DEFAULT_NONE_BUTTON,
    DEFAULT_HOVER_BUTTON,
    DEFAULT_CLICKED_BUTTON,

    BACK_NONE_BUTTON,
    BACK_HOVER_BUTTON,
    BACK_CLICKED_BUTTON,

    // MENU/OPTIONS -> MUSIC
    MUSIC_ON_NONE_BUTTON,
    MUSIC_ON_HOVER_BUTTON,
    MUSIC_ON_CLICKED_BUTTON,

    MUSIC_OFF_NONE_BUTTON,
    MUSIC_OFF_HOVER_BUTTON,
    MUSIC_OFF_CLICKED_BUTTON,

    // MENU/OPTIONS -> SOUND
    SOUND_ON_NONE_BUTTON,
    SOUND_ON_HOVER_BUTTON,
    SOUND_ON_CLICKED_BUTTON,

    SOUND_OFF_NONE_BUTTON,
    SOUND_OFF_HOVER_BUTTON,
    SOUND_OFF_CLICKED_BUTTON,


    // FONT
    DEFAULT_FONT,
    DYSLEXIC_FONT,

};

static const std::array<std::string, NUMBER_ASSETS> pathAssets = {
    // WINDOW
    "../assets/window/RTypeIcon.png",

    // BACKGROUND
    "../assets/background/starsBg.png",
    "../assets/background/gameBg.png",

    // PLAYERS
    "../assets/players/player1.png",
    "../assets/players/player2.png",
    "../assets/players/player3.png",
    "../assets/players/player4.png",
    "../assets/players/player5.png",

    // ENEMIES
    "../assets/enemies/baseEnemy.png",

    // BULLET
    "../assets/bullet/chargingBullet.png",
    "../assets/bullet/defaultBullet.png",
    "../assets/bullet/fullChargedBullet.png",

    // POWERUPS
    "../assets/powerups/petPowerup.png",

    // EXPLOSIONS
    "../assets/explosions/explosions.png",
    "../assets/explosions/smallExplosion.png",
    "../assets/explosions/mediumExplosion.png",
    "../assets/explosions/bigExplosion.png",

    // MENU
    // LOGO
    "../assets/menu/mainMenu/logo/logoRtype.png",

    // BUTTONS
    "../assets/button/defaultNoneButton",
    "../assets/button/defaultHoverButton",
    "../assets/button/defaultClickedButton",

    "../assets/button/backNoneButton",
    "../assets/button/backHoverButton",
    "../assets/button/backClickedButton",

    // MENU/OPTIONS -> MUSIC
    "../assets/music/button/musicOnNoneButton",
    "../assets/music/button/musicOnHoverButton",
    "../assets/music/button/musicOnClickedButton",

    "../assets/music/button/musicOffNoneButton",
    "../assets/music/button/musicOffHoverButton",
    "../assets/music/button/musicOffClickedButton",

    // MENU/OPTIONS -> SOUND
    "../assets/sound/button/soundOnNoneButton",
    "../assets/sound/button/soundOnHoverButton",
    "../assets/sound/button/soundOnClickedButton",

    "../assets/sound/button/soundOffNoneButton",
    "../assets/sound/button/soundOffHoverButton",
    "../assets/sound/button/soundOffClickedButton",


    // FONT
    "../assets/font/defaultFont.otf",
    "../assets/font/dyslexicFont.otf",
};


enum AudioAssets {
    // WEAPONS
    SFX_SHOOT_BASIC,
    SFX_SHOOT_CHARGED,
    SFX_SHOOT_LASER,

    // EXPLOSIONS
    SFX_EXPLOSION_SMALL_1,
    SFX_EXPLOSION_SMALL_2,
    SFX_EXPLOSION_LARGE_1,
    SFX_EXPLOSION_LARGE_2,

    // POWERUPS
    SFX_POWERUP_COLLECT_1,
    SFX_POWERUP_COLLECT_2,

    // PLAYER
    SFX_PLAYER_HIT,
    SFX_PLAYER_DEATH_1,
    SFX_PLAYER_DEATH_2,
    SFX_PLAYER_DEATH_3,

    // FORCE
    SFX_FORCE_ATTACH,
    SFX_FORCE_DETACH,

    // BOSS
    SFX_BOSS_ROAR,

    // UI
    SFX_MENU_SELECT,
    SFX_MENU_ALERT,

    // MUSIC
    MAIN_MENU_MUSIC,
    FIRST_LEVEL_MUSIC,
    SECOND_LEVEL_MUSIC,
    THIRD_LEVEL_MUSIC,
    FOURTH_LEVEL_MUSIC,
    VICTORY_MUSIC,
    DEFEAT_MUSIC,
};

static const std::array<std::string, NUMBER_AUDIO_ASSETS> pathAudioAssets = {
    // WEAPONS
    "../assets/audio/weapons/basic.wav",
    "../assets/audio/weapons/charged.wav",
    "../assets/audio/weapons/laser.wav",

    // EXPLOSIONS
    "../assets/audio/explosions/smallExplosion1.wav",
    "../assets/audio/explosions/smallExplosion2.wav",
    "../assets/audio/explosions/largeExplosion1.wav",
    "../assets/audio/explosions/largeExplosion2.wav",

    // POWERUPS
    "../assets/audio/powerups/collectPowerup1.wav",
    "../assets/audio/powerups/collectPowerup2.wav",

    // PLAYER
    "../assets/audio/player/playerHit.wav",
    "../assets/audio/player/playerDeath1.wav",
    "../assets/audio/player/playerDeath2.wav",
    "../assets/audio/player/playerDeath3.wav",

    // FORCE
    "../assets/audio/force/forceAttach.wav",
    "../assets/audio/force/forceDetach.wav",

    // BOSS
    "../assets/audio/boss/bossRoar.wav",

    // UI
    "../assets/audio/ui/select.wav",
    "../assets/audio/ui/alert.wav",

    // MUSIC
    "../assets/audio/music/mainMenuMusic.ogg",
    "../assets/audio/music/firstLevelMusic.ogg",
    "../assets/audio/music/secondLevelMusic.ogg",
    "../assets/audio/music/thirdLevelMusic.ogg",
    "../assets/audio/music/fourthLevelMusic.ogg",
    "../assets/audio/music/victoryMusic.ogg",
    "../assets/audio/music/defeatMusic.ogg",

};



#endif /* !ASSETS_HPP_ */
