#pragma once

#include <memory>

namespace client {
namespace rendering {

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize(int width, int height, const std::string& title);
    void shutdown();

    void clear();
    void present();

    void beginFrame();
    void endFrame();

    // Drawing primitives
    void drawSprite(uint32_t textureId, float x, float y, float width, float height);
    void drawRectangle(float x, float y, float width, float height, uint32_t color);
    void drawText(const std::string& text, float x, float y, uint32_t color);

private:
    int m_width;
    int m_height;
    bool m_initialized;
};

} // namespace rendering
} // namespace client
