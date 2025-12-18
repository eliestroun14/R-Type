/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** RenderManager
*/

#ifndef SPRITEALLOCATOR_HPP_
#define SPRITEALLOCATOR_HPP_

class PlayerSpriteAllocator {
public:
    Assets allocate(uint32_t entityId) {
        for (Assets a : _players) {
            if (!_used[a]) {
                _used[a] = true;
                _byEntity[entityId] = a;
                return a;
            }
        }
        throw std::runtime_error("No free player sprite");
    }

    void release(uint32_t entityId) {
        auto it = _byEntity.find(entityId);
        if (it != _byEntity.end()) {
            _used[it->second] = false;
            _byEntity.erase(it);
        }
    }

private:
    std::array<Assets, 5> _players = {
        PLAYER_1, PLAYER_2, PLAYER_3, PLAYER_4, PLAYER_5
    };

    std::unordered_map<Assets, bool> _used;
    std::unordered_map<uint32_t, Assets> _byEntity;
};

#endif /* !SPRITEALLOCATOR_HPP_ */