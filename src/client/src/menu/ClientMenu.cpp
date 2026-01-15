/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ClientMenu
*/

#include <client/menu/ClientMenu.hpp>

ClientMenu::ClientMenu(std::shared_ptr<gameEngine::GameEngine> engine)
    : _engine(engine)
{
    _musicOn = true;
    _soundOn = true;
}

ClientMenu::~ClientMenu()
{
}

void ClientMenu::update() {

    // std::cout << "#################### DEBUG ##################" << std::endl;
    // std::cout << "###################" << this->_menuEntities.size() << " ###################" << std::endl;
    // std::cout << "#################### DEBUG ##################" << std::endl;


    while (!_pendingActions.empty()) {
        _pendingActions.front()();
        _pendingActions.pop();
    }
}

void ClientMenu::createMainMenu()
{
    // BACKGROUND
    // addMenuEntity(createImage(*this->_engine, Assets::MAIN_MENU_BG,
    //     sf::Vector2f(0, 0), MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
    //     sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
    //     ZIndex::IS_BACKGROUND));

    addMenuEntity(createMovingBackground(*this->_engine, Assets::MAIN_MENU_BG, {0, 0},
        MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
        sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
        35.0, true, true));

    // IMAGES
    addMenuEntity(createImage(*this->_engine, Assets::LOGO_RTYPE, {150, 100},
        LOGO_RTYPE_ROTATION, LOGO_RTYPE_SCALE, sf::IntRect(0, 0, LOGO_RTYPE_SPRITE_WIDTH, LOGO_RTYPE_SPRITE_HEIGHT), ZIndex::IS_UI_HUD));

    // ANIMATED IMAGES
    Animation planetAnimation(MAIN_MENU_PLANET_SPRITE_WIDTH, MAIN_MENU_PLANET_SPRITE_HEIGHT, MAIN_MENU_PLANET_ANIMATION_CURRENT,
        MAIN_MENU_PLANET_ANIMATION_ELAPSED_TIME, MAIN_MENU_PLANET_ANIMATION_DURATION, MAIN_MENU_PLANET_ANIMATION_START,
        MAIN_MENU_PLANET_ANIMATION_END, MAIN_MENU_PLANET_ANIMATION_LOOPING);

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::MAIN_MENU_PLANET, planetAnimation, {-200, -1300},
        0, MAIN_MENU_PLANET_SPRITE_SCALE, sf::IntRect(0, 0, MAIN_MENU_PLANET_SPRITE_WIDTH, MAIN_MENU_PLANET_SPRITE_HEIGHT), ZIndex::IS_BACKGROUND));


    // BUTTONSs
    addMenuEntity(createButton(*_engine, "PLAY", 40, sf::Color::White, {275, 350}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createPlayMenu();
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "OPTIONS", 40, sf::Color::White, {275, 550}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createOptionMenu();
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "EXIT", 40, sf::Color::White, {275, 750}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                if (this->_engine)
                    this->_engine->closeWindow();
            });
        }
    ));
}

void ClientMenu::createOptionMenu()
{
    // BACKGROUND
    // addMenuEntity(createImage(*this->_engine, Assets::OPTION_MENU_BG,
    //     sf::Vector2f(0, 0), MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
    //     sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
    //     ZIndex::IS_BACKGROUND));

    if (_currentBackgroundAsset != Assets::OPTION_MENU_BG) {
         addMenuEntity(createMovingBackground(*this->_engine, Assets::OPTION_MENU_BG, {0, 0},
            MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
            sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
            35.0, true, true));
        _currentBackgroundAsset = Assets::OPTION_MENU_BG;
    }


    // TEXT
    addMenuEntity(createText(*this->_engine, "OPTIONS", 90, sf::Color::White, {485, 210}, 0, 1.5f));

    // ANIMATED IMAGES
    Animation planetAnimation(OPTION_MENU_PLANET_SPRITE_WIDTH, OPTION_MENU_PLANET_SPRITE_HEIGHT, OPTION_MENU_PLANET_ANIMATION_CURRENT,
        OPTION_MENU_PLANET_ANIMATION_ELAPSED_TIME, OPTION_MENU_PLANET_ANIMATION_DURATION, OPTION_MENU_PLANET_ANIMATION_START,
        OPTION_MENU_PLANET_ANIMATION_END, OPTION_MENU_PLANET_ANIMATION_LOOPING);

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::OPTION_MENU_PLANET, planetAnimation, {-200, -1300},
        0, OPTION_MENU_PLANET_SPRITE_SCALE, sf::IntRect(0, 0, OPTION_MENU_PLANET_SPRITE_WIDTH, OPTION_MENU_PLANET_SPRITE_HEIGHT), ZIndex::IS_BACKGROUND));

    // BUTTONSs
    addMenuEntity(createButton(*_engine, "back", 0, sf::Color::White, {30, 30}, BACK_BUTTON_SCALE,
        sf::IntRect(0, 0, BACK_BUTTON_SPRITE_WIDTH, BACK_BUTTON_SPRITE_HEIGHT),
        BACK_NONE_BUTTON, BACK_HOVER_BUTTON, BACK_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createMainMenu();
            });
        }
    ));

    if (this->_musicOn) {
        addMenuEntity(createButton(*_engine, "mOn", 0, sf::Color::White, {275, 350}, MUSIC_BUTTON_SCALE,
            sf::IntRect(0, 0, MUSIC_BUTTON_SPRITE_WIDTH, MUSIC_BUTTON_SPRITE_HEIGHT),
            MUSIC_ON_NONE_BUTTON, MUSIC_ON_HOVER_BUTTON, MUSIC_ON_CLICKED_BUTTON,
            [this]() {
                this->_pendingActions.push([this]() {
                    //TODO: set music
                    this->_musicOn = false;
                    this->clearMenuEntities(true);
                    this->createOptionMenu();
                });
            }
        ));
    } else {
        addMenuEntity(createButton(*_engine, "mOff", 0, sf::Color::White, {275, 350}, MUSIC_BUTTON_SCALE,
            sf::IntRect(0, 0, MUSIC_BUTTON_SPRITE_WIDTH, MUSIC_BUTTON_SPRITE_HEIGHT),
            MUSIC_OFF_NONE_BUTTON, MUSIC_OFF_HOVER_BUTTON, MUSIC_OFF_CLICKED_BUTTON,
            [this]() {
                this->_pendingActions.push([this]() {
                    //TODO: set music
                    this->_musicOn = true;
                    this->clearMenuEntities(true);
                    this->createOptionMenu();
                });
            }
        ));
    }

    if (this->_soundOn) {
        addMenuEntity(createButton(*_engine, "sOn", 0, sf::Color::White, {575, 350}, SOUND_BUTTON_SCALE,
            sf::IntRect(0, 0, SOUND_BUTTON_SPRITE_WIDTH, SOUND_BUTTON_SPRITE_HEIGHT),
            SOUND_ON_NONE_BUTTON, SOUND_ON_HOVER_BUTTON, SOUND_ON_CLICKED_BUTTON,
            [this]() {
                this->_pendingActions.push([this]() {
                    //TODO: set sound

                    this->_soundOn = false;
                    this->clearMenuEntities(true);
                    this->createOptionMenu();
                });
            }
        ));
    } else {
        addMenuEntity(createButton(*_engine, "sOff", 0, sf::Color::White, {575, 350}, SOUND_BUTTON_SCALE,
            sf::IntRect(0, 0, SOUND_BUTTON_SPRITE_WIDTH, SOUND_BUTTON_SPRITE_HEIGHT),
            SOUND_OFF_NONE_BUTTON, SOUND_OFF_HOVER_BUTTON, SOUND_OFF_CLICKED_BUTTON,
            [this]() {
                this->_pendingActions.push([this]() {
                    //TODO: set sound

                    this->_soundOn = true;
                    this->clearMenuEntities(true);
                    this->createOptionMenu();
                });
            }
        ));
    }

    addMenuEntity(createButton(*_engine, "KEYBINDS", 35, sf::Color::White, {275, 550}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createKeybindsMenu();
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "ACCESSIBILITY", 30, sf::Color::White, {275, 750}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createAccessibilityMenu();
            });
        }
    ));
}

void ClientMenu::createKeybindsMenu()
{
    // BACKGROUND
    // addMenuEntity(createImage(*this->_engine, Assets::KEYBINDS_MENU_BG,
    //     sf::Vector2f(0, 0), MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
    //     sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
    //     ZIndex::IS_BACKGROUND));

    addMenuEntity(createMovingBackground(*this->_engine, Assets::KEYBINDS_MENU_BG, {0, 0},
        MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
        sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
        35.0, true, true));

    // TEXT
    addMenuEntity(createText(*this->_engine, "KEYBINDS", 85, sf::Color::White, {485, 210}, 0, 1.5f));

    // TEXTS DEPLACEMENT
    addMenuEntity(createText(*this->_engine, "MOVEMENT:", 35, sf::Color::White, {200, 350}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "UP   " , 25, sf::Color::White, {120, 450}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "DOWN ", 25, sf::Color::White, {120, 550}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "LEFT ", 25, sf::Color::White, {120, 650}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "RIGHT", 25, sf::Color::White, {120, 750}, 0, 1.5f));

    // TEXTS DEPLACEMENT
    // addMenuEntity(createText(*this->_engine, "SHOOT", 25, sf::Color::White, {200, 750}, 0, 1.5f));
    // addMenuEntity(createText(*this->_engine, "SWITCH WEAPON", 25, sf::Color::White, {700, 750}, 0, 1.5f));
    // addMenuEntity(createText(*this->_engine, "USE POWERUP", 25, sf::Color::White, {250, 950}, 0, 1.5f));
    // addMenuEntity(createText(*this->_engine, "SPECIAL", 25, sf::Color::White, {700, 950}, 0, 1.5f));


    // ANIMATED IMAGES
    Animation planetAnimation(OPTION_MENU_PLANET_SPRITE_WIDTH, OPTION_MENU_PLANET_SPRITE_HEIGHT, OPTION_MENU_PLANET_ANIMATION_CURRENT,
        OPTION_MENU_PLANET_ANIMATION_ELAPSED_TIME, OPTION_MENU_PLANET_ANIMATION_DURATION, OPTION_MENU_PLANET_ANIMATION_START,
        OPTION_MENU_PLANET_ANIMATION_END, OPTION_MENU_PLANET_ANIMATION_LOOPING);

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::KEYBINDS_MENU_PLANET, planetAnimation, {-200, -1300},
        0, OPTION_MENU_PLANET_SPRITE_SCALE, sf::IntRect(0, 0, OPTION_MENU_PLANET_SPRITE_WIDTH, OPTION_MENU_PLANET_SPRITE_HEIGHT), ZIndex::IS_GAME));

    // BUTTONSs
    addMenuEntity(createButton(*_engine, "back", 0, sf::Color::White, {30, 30}, BACK_BUTTON_SCALE,
        sf::IntRect(0, 0, BACK_BUTTON_SPRITE_WIDTH, BACK_BUTTON_SPRITE_HEIGHT),
        BACK_NONE_BUTTON, BACK_HOVER_BUTTON, BACK_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createOptionMenu();
            });
        }
    ));


    // DEPLACEMENT BUTTON KEYBINDS
    addMenuEntity(createButton(*_engine, "UP", 20, sf::Color::White, {230, 400}, DEFAULT_BUTTON_MID_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                //TODO: do something
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "DOWN", 20, sf::Color::White, {230, 500}, DEFAULT_BUTTON_MID_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                //TODO: do something
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "LEFT", 20, sf::Color::White, {230, 600}, DEFAULT_BUTTON_MID_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                //TODO: do something
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "RIGHT", 20, sf::Color::White, {230, 700}, DEFAULT_BUTTON_MID_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                //TODO: do something
            });
        }
    ));


}


void ClientMenu::createAccessibilityMenu()
{
    // BACKGROUND
    // addMenuEntity(createImage(*this->_engine, Assets::KEYBINDS_MENU_BG,
    //     sf::Vector2f(0, 0), MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
    //     sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
    //     ZIndex::IS_BACKGROUND));

    addMenuEntity(createMovingBackground(*this->_engine, Assets::KEYBINDS_MENU_BG, {0, 0},
        MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
        sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
        35.0, true, true));

    // TEXT
    addMenuEntity(createText(*this->_engine, "ACCESSIBILITY", 80, sf::Color::White, {485, 210}, 0, 1.5f));

    // ANIMATED IMAGES
    Animation planetAnimation(OPTION_MENU_PLANET_SPRITE_WIDTH, OPTION_MENU_PLANET_SPRITE_HEIGHT, OPTION_MENU_PLANET_ANIMATION_CURRENT,
        OPTION_MENU_PLANET_ANIMATION_ELAPSED_TIME, OPTION_MENU_PLANET_ANIMATION_DURATION, OPTION_MENU_PLANET_ANIMATION_START,
        OPTION_MENU_PLANET_ANIMATION_END, OPTION_MENU_PLANET_ANIMATION_LOOPING);

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::KEYBINDS_MENU_PLANET, planetAnimation, {-200, -1300},
        0, OPTION_MENU_PLANET_SPRITE_SCALE, sf::IntRect(0, 0, OPTION_MENU_PLANET_SPRITE_WIDTH, OPTION_MENU_PLANET_SPRITE_HEIGHT), ZIndex::IS_BACKGROUND));

    // BUTTONSs
    addMenuEntity(createButton(*_engine, "back", 0, sf::Color::White, {30, 30}, BACK_BUTTON_SCALE,
        sf::IntRect(0, 0, BACK_BUTTON_SPRITE_WIDTH, BACK_BUTTON_SPRITE_HEIGHT),
        BACK_NONE_BUTTON, BACK_HOVER_BUTTON, BACK_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createOptionMenu();
            });
        }
    ));


    addMenuEntity(createButton(*_engine, "CHANGE FONT", 35, sf::Color::White, {275, 550}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                auto& configs = _engine->getComponents<GameConfig>();

                for (auto& config : configs) {
                    if (config.has_value()) {
                        if (config->activeFont == FontAssets::DEFAULT_FONT)
                            config->activeFont = FontAssets::DYSLEXIC_FONT;
                        else
                            config->activeFont = FontAssets::DEFAULT_FONT;
                        break;
                    }
                }
            });
        }
    ));
}



void ClientMenu::createPlayMenu()
{
    // BACKGROUND
    // addMenuEntity(createImage(*this->_engine, Assets::MAIN_MENU_BG,
    //     sf::Vector2f(0, 0), MAIN_MENU_BG_ROTATION, MAIN_MENU_BG_SCALE,
    //     sf::IntRect(0, 0, MAIN_MENU_BG_SPRITE_WIDTH, MAIN_MENU_BG_SPRITE_HEIGHT),
    //     ZIndex::IS_BACKGROUND));

    addMenuEntity(createMovingBackground(*this->_engine, Assets::GAME_BG, {0, 0},
        GAME_BG_ROTATION, GAME_BG_SCALE,
        sf::IntRect(0, 0, GAME_BG_SPRITE_WIDTH, GAME_BG_SPRITE_HEIGHT),
        35.0, true, true));

    // TEXTS

    addMenuEntity(createText(*this->_engine, "Waiting for players", 60, sf::Color::White,
        {950, 500}, 0, 1.5f));

    // ANIMATED ENTITIES

    Animation spiralAnimation(SPIRAL_BULLET_SPRITE_WIDTH, SPIRAL_BULLET_SPRITE_HEIGHT, SPIRAL_BULLET_ANIMATION_CURRENT,
        SPIRAL_BULLET_ANIMATION_ELAPSED_TIME, SPIRAL_BULLET_ANIMATION_DURATION, SPIRAL_BULLET_ANIMATION_START,
        SPIRAL_BULLET_ANIMATION_END, SPIRAL_BULLET_ANIMATION_LOOPING);

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::SPIRAL_BULLET, spiralAnimation,
        {300, 455}, 0, SPIRAL_BULLET_SCALE,
        sf::IntRect(0, 0, SPIRAL_BULLET_SPRITE_WIDTH, SPIRAL_BULLET_SPRITE_HEIGHT), ZIndex::IS_UI_HUD));

    addMenuEntity(createAnimatedImage(*this->_engine, Assets::SPIRAL_BULLET, spiralAnimation,
        {1510, 455}, 0, SPIRAL_BULLET_SCALE,
        sf::IntRect(0, 0, SPIRAL_BULLET_SPRITE_WIDTH, SPIRAL_BULLET_SPRITE_HEIGHT), ZIndex::IS_UI_HUD));


    // BUTTONSs
    addMenuEntity(createButton(*_engine, "back", 0, sf::Color::White, {30, 30}, BACK_BUTTON_SCALE,
        sf::IntRect(0, 0, BACK_BUTTON_SPRITE_WIDTH, BACK_BUTTON_SPRITE_HEIGHT),
        BACK_NONE_BUTTON, BACK_HOVER_BUTTON, BACK_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createMainMenu();
            });
        }
    ));

}


void ClientMenu::clearMenuEntities(bool keepBackground)
{
    if (!_engine) return;

    for (auto it = _menuEntities.begin(); it != _menuEntities.end(); ) {
        Entity entity = *it;

        // Si on doit garder le background, on vérifie si l'entité en est un
        // On peut vérifier via le composant ScrollingBackground
        if (keepBackground && _engine->hasComponent<ScrollingBackground>(entity)) {
            ++it; // On passe à l'entité suivante sans rien supprimer
            continue;
        }

        // --- Nettoyage habituel ---
        if (_engine->hasComponent<ButtonComponent>(entity)) {
            auto& btn = _engine->getComponentEntity<ButtonComponent>(entity);
            btn->onClick = nullptr;
            _engine->removeComponent<ButtonComponent>(entity);
        }

        if (_engine->hasComponent<Transform>(entity))
            _engine->removeComponent<Transform>(entity);

        if (_engine->hasComponent<Sprite>(entity))
            _engine->removeComponent<Sprite>(entity);

        if (_engine->hasComponent<Text>(entity))
            _engine->removeComponent<Text>(entity);

        // On retire l'entité du vecteur et on récupère l'itérateur suivant
        it = _menuEntities.erase(it);
    }

    // Si on a tout supprimé (pas de background gardé), on remet à zéro l'asset actuel
    if (!keepBackground) {
        _currentBackgroundAsset = Assets::MAIN_MENU_BG;
    }
}
