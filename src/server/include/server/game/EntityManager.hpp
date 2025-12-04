#pragma once

#include <unordered_map>
#include <cstdint>

namespace server {
namespace game {

class EntityManager {
public:
    uint32_t createEntity();
    void destroyEntity(uint32_t entityId);
    bool exists(uint32_t entityId) const;

    // Component management
    template<typename T>
    void addComponent(uint32_t entityId, T component);

    template<typename T>
    T* getComponent(uint32_t entityId);

    template<typename T>
    void removeComponent(uint32_t entityId);

private:
    uint32_t m_nextEntityId = 1;
    std::unordered_map<uint32_t, bool> m_activeEntities;
};

} // namespace game
} // namespace server
