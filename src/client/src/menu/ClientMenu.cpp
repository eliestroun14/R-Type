/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** ClientMenu
*/

#include <client/menu/ClientMenu.hpp>
#include <game/coordinator/Coordinator.hpp>

ClientMenu::ClientMenu(std::shared_ptr<gameEngine::GameEngine> engine, std::shared_ptr<Coordinator> coordinator)
    : _engine(engine), _coordinator(coordinator)
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

    auto& configs = _engine->getComponents<GameConfig>();
    for (auto& config : configs)
        if (config.has_value()) {
            // we set the GameConfig here, if we change something in settings about keybinds, changement will be set
            _engine->getRenderManager()->updateKeyBindings(config->_keybinds);
            break;
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
        0, MAIN_MENU_PLANET_SPRITE_SCALE, sf::IntRect(0, 0, MAIN_MENU_PLANET_SPRITE_WIDTH, MAIN_MENU_PLANET_SPRITE_HEIGHT), ZIndex::IS_GAME));


    // BUTTONSs
    addMenuEntity(createButton(*_engine, "PLAY", 40, sf::Color::White, {275, 350}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createPlayMenu();
                
                // Get the player ID and queue the ready event
                if (this->_coordinator) {
                    auto playerIds = this->_coordinator->getPlayablePlayerIds();
                    if (!playerIds.empty()) {
                        this->_coordinator->queuePlayerIsReady(playerIds[0]);
                    }
                }
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

    addMenuEntity(createButton(*_engine, "CREDITS", 30, sf::Color::White, {350, 900}, DEFAULT_BUTTON_MID_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                this->clearMenuEntities();
                this->createCreditsMenu();
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
                    auto& configs = _engine->getComponents<GameConfig>();
                    for (auto& config : configs) {
                        if (config.has_value()) {
                            config->musicEnabled = false;
                            break;
                        }
                    }
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
                    auto& configs = _engine->getComponents<GameConfig>();
                    for (auto& config : configs) {
                        if (config.has_value()) {
                            config->musicEnabled = true;
                            break;
                        }
                    }
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
                    auto& configs = _engine->getComponents<GameConfig>();
                    for (auto& config : configs) {
                        if (config.has_value()) {
                            config->soundEnabled = false;
                            break;
                        }
                    }
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
                    auto& configs = _engine->getComponents<GameConfig>();
                    for (auto& config : configs) {
                        if (config.has_value()) {
                            config->soundEnabled = true;
                            break;
                        }
                    }

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

    // TEXTS MOVEMENT
    addMenuEntity(createText(*this->_engine, "MOVEMENT:", 35, sf::Color::White, {200, 350}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "UP" , 25, sf::Color::White, {250, 420}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "DOWN", 25, sf::Color::White, {250, 570}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "LEFT", 25, sf::Color::White, {250, 720}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "RIGHT", 25, sf::Color::White, {250, 870}, 0, 1.5f));

    // TEXTS ACTIONS
    addMenuEntity(createText(*this->_engine, "ACTIONS:", 35, sf::Color::White, {650, 350}, 0, 1.5f));

    // addMenuEntity(createText(*this->_engine, "SHOOT", 25, sf::Color::White, {650, 420}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "SWITCH WEAPON", 25, sf::Color::White, {650, 490}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "USE POWERUP", 25, sf::Color::White, {650, 640}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "SPECIAL", 25, sf::Color::White, {650, 790}, 0, 1.5f));


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

    // MOVEMENT BUTTON KEYBINDS
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::MOVE_UP), GameAction::MOVE_UP, 30, {110, 450}, DEFAULT_BUTTON_MID_SCALE));
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::MOVE_DOWN), GameAction::MOVE_DOWN, 30, {110, 600}, DEFAULT_BUTTON_MID_SCALE));
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::MOVE_LEFT), GameAction::MOVE_LEFT, 30, {110, 750}, DEFAULT_BUTTON_MID_SCALE));
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::MOVE_RIGHT), GameAction::MOVE_RIGHT, 30, {110, 900}, DEFAULT_BUTTON_MID_SCALE));

    // addMenuEntity(createButton(*_engine, "UP", 30, sf::Color::White, {110, 450}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createButton(*_engine, "DOWN", 30, sf::Color::White, {110, 600}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createButton(*_engine, "LEFT", 30, sf::Color::White, {110, 750}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createButton(*_engine, "RIGHT", 30, sf::Color::White, {110, 900}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));


    // ACTIONS BUTTON KEYBINDS
    // addMenuEntity(createButton(*_engine, "SHOOT", 30, sf::Color::White, {510, 450}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createButton(*_engine, "SWITCH WEAPON", 30, sf::Color::White, {510, 600}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createButton(*_engine, "USE POWERUP", 30, sf::Color::White, {510, 750}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createButton(*_engine, "SPECIAL", 30, sf::Color::White, {510, 900}, DEFAULT_BUTTON_MID_SCALE,
    //     sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
    //     DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
    //     [this]() {
    //         this->_pendingActions.push([this]() {
    //             //TODO: do something
    //         });
    //     }
    // ));

    // addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::SHOOT), GameAction::SHOOT, 30, {510, 450}, DEFAULT_BUTTON_MID_SCALE));
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::SWITCH_WEAPON), GameAction::SWITCH_WEAPON, 30, {510, 520}, DEFAULT_BUTTON_MID_SCALE));
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::USE_POWERUP), GameAction::USE_POWERUP, 30, {510, 670}, DEFAULT_BUTTON_MID_SCALE));
    addMenuEntity(createRebindButton(*_engine, getKeybordKeyFromGameConfig(*this->_engine, GameAction::SPECIAL), GameAction::SPECIAL, 30, {510, 820}, DEFAULT_BUTTON_MID_SCALE));


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

    addMenuEntity(createText(*this->_engine, "DYSLEXIC FONTS", 40, sf::Color::White, {485, 400}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "DEFAULT FONT", 40, sf::Color::White, {485, 800}, 0, 1.5f));

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


    addMenuEntity(createButton(*_engine, "DEFAULT FONT", 30, sf::Color::White, {275, 850}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                auto& configs = _engine->getComponents<GameConfig>();

                for (auto& config : configs) {
                    if (config.has_value()) {
                        config->activeFont = FontAssets::DEFAULT_FONT;
                        break;
                    }
                }
            });
        }
    ));

    addMenuEntity(createButton(*_engine, "WEIGHTED", 35, sf::Color::White, {50, 450}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                auto& configs = _engine->getComponents<GameConfig>();

                for (auto& config : configs) {
                    if (config.has_value()) {
                        config->activeFont = FontAssets::DYSLEXIC_FONT;
                        break;
                    }
                }
            });
        }
    ));


    addMenuEntity(createButton(*_engine, "ORGANIC", 35, sf::Color::White, {500, 450}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                auto& configs = _engine->getComponents<GameConfig>();

                for (auto& config : configs) {
                    if (config.has_value()) {
                        config->activeFont = FontAssets::DYSLEXIC_FONT_2;
                        break;
                    }
                }
            });
        }
    ));


    addMenuEntity(createButton(*_engine, "CLEAR", 35, sf::Color::White, {50, 600}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                auto& configs = _engine->getComponents<GameConfig>();

                for (auto& config : configs) {
                    if (config.has_value()) {
                        config->activeFont = FontAssets::DYSLEXIC_FONT_3;
                        break;
                    }
                }
            });
        }
    ));


    addMenuEntity(createButton(*_engine, "THIN", 35, sf::Color::White, {500, 600}, DEFAULT_BUTTON_SCALE,
        sf::IntRect(0, 0, DEFAULT_BUTTON_SPRITE_WIDTH, DEFAULT_BUTTON_SPRITE_HEIGHT),
        DEFAULT_NONE_BUTTON, DEFAULT_HOVER_BUTTON, DEFAULT_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                auto& configs = _engine->getComponents<GameConfig>();

                for (auto& config : configs) {
                    if (config.has_value()) {
                        config->activeFont = FontAssets::DYSLEXIC_FONT_4;
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
                // Queue the not ready event before going back
                if (this->_coordinator) {
                    auto playerIds = this->_coordinator->getPlayablePlayerIds();
                    if (!playerIds.empty()) {
                        this->_coordinator->queuePlayerNotReady(playerIds[0]);
                    }
                }
                
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

void ClientMenu::createScoreMenu(uint32_t score)
{
    // Clear any existing menu entities first
    clearMenuEntities();
    
    // BACKGROUND
    addMenuEntity(createMovingBackground(*this->_engine, Assets::GAME_BG, {0, 0},
        GAME_BG_ROTATION, GAME_BG_SCALE,
        sf::IntRect(0, 0, GAME_BG_SPRITE_WIDTH, GAME_BG_SPRITE_HEIGHT),
        35.0, true, true));
    
    // TEXTS
    addMenuEntity(createText(*this->_engine, "CONGRATULATIONS", 80, sf::Color::White, {1000, 210}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "You survived to the first level", 50, sf::Color::White, {1000, 320}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "SCORE :", 40, sf::Color::White, {800, 550}, 0, 1.5f));
    
    // Format score with spaces for readability
    std::string scoreStr = std::to_string(score);
    addMenuEntity(createText(*this->_engine, scoreStr, 40, sf::Color::White, {1100, 550}, 0, 1.5f));

    addMenuEntity(createButton(*_engine, "EXIT", 40, sf::Color::White, {750, 750}, DEFAULT_BUTTON_SCALE,
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

void ClientMenu::createCreditsMenu()
{
    // BACKGROUND
    addMenuEntity(createMovingBackground(*this->_engine, Assets::EPIPECH_SMILE, {0, 0},
        0, 12.0f,
        sf::IntRect(0, 0, 218, 164),
        35.0, true, true));

    // TEXTS
    addMenuEntity(createText(*this->_engine, "OUR TEAM", 80, sf::Color::Magenta, {1000, 100}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "Manech (nitrached)", 20, sf::Color::Magenta, {250, 750}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "The king of Cleunay", 15, sf::Color::Magenta, {250, 800}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "Elie (TTV_Fr3nchFri3s)", 20, sf::Color::Magenta, {650, 750}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "The future chinese", 15, sf::Color::Magenta, {650, 800}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "Timeo (Pyro)", 20, sf::Color::Magenta, {1000, 750}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "The marmoset", 15, sf::Color::Magenta, {1000, 800}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "Pablo (Smoulmouc)", 20, sf::Color::Magenta, {1350, 750}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "The Goblin", 15, sf::Color::Magenta, {1350, 800}, 0, 1.5f));

    addMenuEntity(createText(*this->_engine, "Aymeric (Mic)", 20, sf::Color::Magenta, {1700, 750}, 0, 1.5f));
    addMenuEntity(createText(*this->_engine, "The temple enjoyer", 15, sf::Color::Magenta, {1700, 800}, 0, 1.5f));

    // IMAGES
    addMenuEntity(createImage(*this->_engine, Assets::PANECH, {100, 250},
        0, 4.0f, sf::IntRect(0, 0, 76, 101), ZIndex::IS_UI_HUD));

    addMenuEntity(createImage(*this->_engine, Assets::PELIE, {500, 250},
        0, 4.0f, sf::IntRect(0, 10, 76, 101), ZIndex::IS_UI_HUD));

    addMenuEntity(createImage(*this->_engine, Assets::PIMEO, {850, 250},
        0, 4.0f, sf::IntRect(0, 10, 76, 101), ZIndex::IS_UI_HUD));

    addMenuEntity(createImage(*this->_engine, Assets::TABLO, {1200, 250},
        0, 4.0f, sf::IntRect(0, 10, 76, 101), ZIndex::IS_UI_HUD));

    addMenuEntity(createImage(*this->_engine, Assets::PAYMERIC, {1550, 250},
        0, 4.0f, sf::IntRect(0, 10, 76, 101), ZIndex::IS_UI_HUD));


    // BUTTON
    addMenuEntity(createButton(*_engine, "back", 0, sf::Color::White, {30, 30}, BACK_BUTTON_SCALE,
        sf::IntRect(0, 0, BACK_BUTTON_SPRITE_WIDTH, BACK_BUTTON_SPRITE_HEIGHT),
        BACK_NONE_BUTTON, BACK_HOVER_BUTTON, BACK_CLICKED_BUTTON,
        [this]() {
            this->_pendingActions.push([this]() {
                // Queue the not ready event before going back
                if (this->_coordinator) {
                    auto playerIds = this->_coordinator->getPlayablePlayerIds();
                    if (!playerIds.empty()) {
                        this->_coordinator->queuePlayerNotReady(playerIds[0]);
                    }
                }
                
                this->clearMenuEntities();
                this->createMainMenu();
            });
        }
    ));
}
