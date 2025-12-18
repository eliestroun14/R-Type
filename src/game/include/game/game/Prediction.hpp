#pragma once

#include <vector>
#include <cstdint>

namespace client {
namespace game {

struct PredictedInput {
    uint32_t sequence;
    uint8_t actions;
    uint32_t timestamp;
};

class Prediction {
public:
    void addInput(const PredictedInput& input);
    void reconcile(uint32_t acknowledgedSequence);

    void predictMovement(float deltaTime);
    void rewindAndReplay(uint32_t fromSequence);

private:
    std::vector<PredictedInput> m_inputHistory;
    uint32_t m_nextSequence = 0;
};

} // namespace game
} // namespace client
