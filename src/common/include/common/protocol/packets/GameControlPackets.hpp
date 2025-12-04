#pragma once

#include "../Packet.hpp"

namespace common {
namespace protocol {
namespace packets {

struct GameStart {
    uint32_t gameId;
    uint8_t numPlayers;
    uint8_t difficulty;
};

struct GamePause {
    uint32_t playerId;
};

struct GameResume {
    uint32_t resumeTime;
};

struct GameEnd {
    bool victory;
    uint32_t finalScore;
    uint32_t duration;
};

} // namespace packets
} // namespace protocol
} // namespace common
