#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace client {
namespace rendering {

struct Sprite {
    uint32_t textureId;
    float width;
    float height;
};

class SpriteManager {
public:
    bool loadSprite(const std::string& name, const std::string& filepath);
    Sprite* getSprite(const std::string& name);
    void unloadSprite(const std::string& name);
    void unloadAll();

private:
    std::unordered_map<std::string, Sprite> m_sprites;
};

} // namespace rendering
} // namespace client
