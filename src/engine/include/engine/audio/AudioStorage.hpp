/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** AudioStorage
*/

#ifndef AUDIOSTORAGE_HPP_
#define AUDIOSTORAGE_HPP_

#include <SFML/Audio.hpp>
#include <common/constants/render/Assets.hpp>
#include <vector>
#include <memory>
#include <common/error/Error.hpp>
#include <common/error/ErrorMessages.hpp>

class AudioStorage {
public:
    AudioStorage() = default;

    void init()
    {
        // Reserve space to avoid useless reallocations
        this->_soundBuffers.reserve(NUMBER_AUDIO_ASSETS);

        for (size_t i = 0; i < NUMBER_AUDIO_ASSETS; i++) {
            auto buffer = std::make_shared<sf::SoundBuffer>();
            if (!buffer->loadFromFile(pathAudioAssets[i])) {
                std::cerr << "Failed to load audio: " << pathAudioAssets[i] << std::endl;
                throw Error(ErrorType::ResourceLoadFailure, ErrorMessages::RESOURCE_LOAD_FAILURE);
            }
            this->_soundBuffers.push_back(std::move(buffer));
            std::cout << "[AudioStorage] Loaded: " << pathAudioAssets[i] << std::endl;
        }
    }

    std::shared_ptr<sf::SoundBuffer> getSoundBuffer(AudioAssets id) const
    {
        if (id < 0 || id >= this->_soundBuffers.size())
            return nullptr;
        return this->_soundBuffers[id];
    }

private:
    std::vector<std::shared_ptr<sf::SoundBuffer>> _soundBuffers;
};

#endif /* !AUDIOSTORAGE_HPP_ */
