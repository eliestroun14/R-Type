#pragma once

#include <memory>
#include <vector>

namespace client {
namespace ui {

class UIElement {
public:
    virtual ~UIElement() = default;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual bool handleInput(float mouseX, float mouseY, bool clicked) = 0;
};

class UIManager {
public:
    void addElement(std::shared_ptr<UIElement> element);
    void removeElement(std::shared_ptr<UIElement> element);
    void clear();

    void update(float deltaTime);
    void render();
    bool handleInput(float mouseX, float mouseY, bool clicked);

private:
    std::vector<std::shared_ptr<UIElement>> m_elements;
};

} // namespace ui
} // namespace client
