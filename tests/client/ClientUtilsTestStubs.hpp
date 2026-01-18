#pragma once

#include <functional>
#include <string>
#include <vector>

#include <engine/ecs/entity/Entity.hpp>

namespace testutils {

struct ButtonRecord {
    std::string label;
    std::function<void()> onClick;
    std::vector<Entity> entities;
};

extern std::vector<ButtonRecord> buttonRecords;
extern int movingBackgroundCount;

void resetClientUtilsStubs();
const ButtonRecord* findButton(const std::string& label);

} // namespace testutils
