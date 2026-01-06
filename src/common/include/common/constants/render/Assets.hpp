/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** Assets
*/

#ifndef ASSETS_HPP_
#define ASSETS_HPP_

#define NUMBER_ASSETS 17
#define NUMBER_AUDIO_ASSETS 1


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
};


enum AudioAssets {
    // EXPLOSIONS
    EXPLOSION_AUDIO
};

static const std::array<std::string, NUMBER_AUDIO_ASSETS> pathAudioAssets = {
    // EXPLOSIONS
    "../assets/explosions/explosion.png",


};



#endif /* !ASSETS_HPP_ */
