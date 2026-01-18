/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** TextureStorage
*/

#ifndef TEXTURESTORAGE_HPP_
#define TEXTURESTORAGE_HPP_

#include <SFML/Graphics.hpp>
#include <common/constants/render/Assets.hpp>
#include <vector>
#include <memory>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>

class TextureStorage {
    public:
        TextureStorage() = default;

        void init()
        {
            // reserve the place to avoir useless reallocations
            this->_textures.reserve(NUMBER_ASSETS);


            for (size_t i = 0; i < NUMBER_ASSETS; i++) {
                auto texture = std::make_shared<sf::Texture>();

                if (!texture->loadFromFile(pathAssets[i]))
                    throw Error(ErrorType::GraphicsInitializationFailed, ErrorMessages::GRAPHICS_INITIALIZATION_FAILED);

                this->_textures.push_back(std::move(texture));
            }
        }

        std::shared_ptr<sf::Texture> getTexture(Assets id) const
        {
            if (id < 0 || id >= this->_textures.size())
                return nullptr;
            return this->_textures[id];
        }

    private:
        std::vector<std::shared_ptr<sf::Texture>> _textures;
};

#endif /* !TEXTURESTORAGE_HPP_ */
