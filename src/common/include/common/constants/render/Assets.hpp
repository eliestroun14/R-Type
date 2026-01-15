/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Assets
*/

#ifndef ASSETS_HPP_
#define ASSETS_HPP_

#define NUMBER_ASSETS 51
#define NUMBER_FONT_ASSETS 2
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
    BASE_WALKER_ENEMY,
    GREEN_BRAIN_ENEMY,
    TURBINE_ENEMY,

    // BOSS
    GREEN_CELLS_BOSS,
    SPACESHIP_BOSS,

    // BULLET
    CHARCHING_BULLET,
    DEFAULT_BULLET,
    FULL_CHARGED_BULLET,
    SPIRAL_BULLET,

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

    // MAIN MENU ELEMENTS
    MAIN_MENU_PLANET,
    MAIN_MENU_BG,
    MAIN_MENU_BG_LIGHT,


    // OPTIONS ELEMENTS
    OPTION_MENU_PLANET,
    OPTION_MENU_BG,


    // KEYBINDS ELEMENTS
    KEYBINDS_MENU_PLANET,
    KEYBINDS_MENU_BG,


    // ACCESIBILITY ELEMENTS
    ACCESSIBILITY_MENU_PLANET,
    ACCESSIBILITY_MENU_BG,

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
    "../assets/enemies/baseWalkerEnemy.gif",
    "../assets/enemies/greenBrainEnemy.gif",
    "../assets/enemies/turbineEnemy.gif",

    // BOSS
    "../assets/enemies/boss/greenCellsBoss.gif",
    "../assets/enemies/boss/spaceShipBoss.gif",

    // BULLET
    "../assets/bullet/chargingBullet.png",
    "../assets/bullet/defaultBullet.png",
    "../assets/bullet/fullChargedBullet.png",
    "../assets/bullet/spiralBullet.png",

    // POWERUPS
    "../assets/powerups/petPowerup.png",

    // EXPLOSIONS
    "../assets/explosions/explosions.png",
    "../assets/explosions/smallExplosion.png",
    "../assets/explosions/mediumExplosion.png",
    "../assets/explosions/bigExplosion.png",

    // MENU
    // LOGO
    "../assets/menu/mainMenu/logo/logoRType.png",

    // MAIN MENU ELEMENTS
    "../assets/menu/mainMenu/planet/mainMenuPlanet.png",
    "../assets/menu/mainMenu/background/mainMenuBg.png",
    "../assets/menu/mainMenu/background/mainMenuBgLight.png",


    // OPTION MENU ELEMENTS
    "../assets/menu/options/planet/optionMenuPlanet.png",
    "../assets/menu/options/background/optionMenuBg.png",


    // KEYBINDS ELEMENTS
    "../assets/menu/keybinds/planet/keybindsMenuPlanet.png",
    "../assets/menu/keybinds/background/keybindsMenuBg.png",


    // ACCESSIBILITY ELEMENTS
    "../assets/menu/accessibility/planet/accessibilityMenuPlanet.png",
    "../assets/menu/accessibility/background/accessibilityMenuBg.png",


    // BUTTONS
    "../assets/button/defaultNoneButton.png",
    "../assets/button/defaultHoverButton.png",
    "../assets/button/defaultClickedButton.png",

    "../assets/button/backNoneButton.png",
    "../assets/button/backHoverButton.png",
    "../assets/button/backClickedButton.png",

    // MENU/OPTIONS -> MUSIC
    "../assets/button/options/music/musicOnNoneButton.png",
    "../assets/button/options/music/musicOnHoverButton.png",
    "../assets/button/options/music/musicOnClickedButton.png",

    "../assets/button/options/music/musicOffNoneButton.png",
    "../assets/button/options/music/musicOffHoverButton.png",
    "../assets/button/options/music/musicOffClickedButton.png",

    // MENU/OPTIONS -> SOUND
    "../assets/button/options/sound/soundOnNoneButton.png",
    "../assets/button/options/sound/soundOnHoverButton.png",
    "../assets/button/options/sound/soundOnClickedButton.png",

    "../assets/button/options/sound/soundOffNoneButton.png",
    "../assets/button/options/sound/soundOffHoverButton.png",
    "../assets/button/options/sound/soundOffClickedButton.png",
};


enum FontAssets {
    DEFAULT_FONT,
    DYSLEXIC_FONT,
};

static const std::array<std::string, NUMBER_FONT_ASSETS> pathFontAssets = {
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
    "../assets/audio/sound/weapons/basic.wav",
    "../assets/audio/sound/weapons/charged.wav",
    "../assets/audio/sound/weapons/laser.wav",

    // EXPLOSIONS
    "../assets/audio/sound/explosions/smallExplosion1.wav",
    "../assets/audio/sound/explosions/smallExplosion2.wav",
    "../assets/audio/sound/explosions/largeExplosion1.wav",
    "../assets/audio/sound/explosions/largeExplosion2.wav",

    // POWERUPS
    "../assets/audio/sound/powerups/collectPowerup1.wav",
    "../assets/audio/sound/powerups/collectPowerup2.wav",

    // PLAYER
    "../assets/audio/sound/player/playerHit.wav",
    "../assets/audio/sound/player/playerDeath1.wav",
    "../assets/audio/sound/player/playerDeath2.wav",
    "../assets/audio/sound/player/playerDeath3.wav",

    // FORCE
    "../assets/audio/sound/force/forceAttach.wav",
    "../assets/audio/sound/force/forceDetach.wav",

    // BOSS
    "../assets/audio/sound/boss/bossRoar.wav",

    // UI
    "../assets/audio/sound/ui/select.wav",
    "../assets/audio/sound/ui/alert.wav",

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
