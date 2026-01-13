/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** FontStorage
*/

#ifndef FONTSTORAGE_HPP_
#define FONTSTORAGE_HPP_

#include <SFML/Graphics.hpp>
#include <common/constants/render/Assets.hpp>
#include <vector>
#include <memory>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>

class FontStorage {
    public:
        FontStorage() = default;

        void init() {
            this->_fonts.reserve(NUMBER_FONT_ASSETS);


            for (size_t i = 0; i < NUMBER_FONT_ASSETS; i++) {
                auto font = std::make_shared<sf::Font>();

                if (!font->loadFromFile(pathFontAssets[i]))
                    throw Error(ErrorType::GraphicsInitializationFailed, ErrorMessages::GRAPHICS_INITIALIZATION_FAILED);

                this->_fonts.push_back(std::move(font));
            }
        }

        std::shared_ptr<sf::Font> getFont(FontAssets id) const {
            if (id < 0 || id >= this->_fonts.size())
                return nullptr;
            return this->_fonts[id];
        }

    private:
        void loadFont(FontAssets id) {
            auto font = std::make_shared<sf::Font>();
            if (!font->loadFromFile(pathFontAssets[id]))
                throw Error(ErrorType::GraphicsInitializationFailed, "Failed to load font");
            _fonts[id] = std::move(font);
        }

        std::vector<std::shared_ptr<sf::Font>> _fonts;
};

#endif /* !FONTSTORAGE_HPP_ */
