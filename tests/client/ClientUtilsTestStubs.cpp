/* #include "ClientUtilsTestStubs.hpp"

#include <game/utils/ClientUtils.hpp>
#include <engine/ecs/component/Components.hpp>

namespace testutils {

std::vector<ButtonRecord> buttonRecords;
int movingBackgroundCount = 0;

void resetClientUtilsStubs()
{
    buttonRecords.clear();
    movingBackgroundCount = 0;
}

const ButtonRecord* findButton(const std::string& label)
{
    for (const auto& record : buttonRecords) {
        if (record.label == label) {
            return &record;
        }
    }
    return nullptr;
}

} // namespace testutils

uint32_t getCurrentTimeMs()
{
    return 0;
}

uint32_t generateClientId()
{
    return 42;
}

Entity createText(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos,
    float rotation, float scale)
{
    Entity entity = engine.createEntity("Text:" + label);
    engine.addComponent(entity, Text(label.c_str(), textColor, textSize, ZIndex::IS_UI_HUD));
    engine.addComponent(entity, Transform(pos.x, pos.y, rotation, scale));
    return entity;
}

Entity createImage(gameEngine::GameEngine& engine, Assets assetId,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rectSprite, ZIndex zIndex)
{
    Entity entity = engine.createEntity("Image");
    engine.addComponent(entity, Sprite(assetId, zIndex, rectSprite));
    engine.addComponent(entity, Transform(pos.x, pos.y, rotation, scale));
    return entity;
}

Entity createAnimatedImage(gameEngine::GameEngine& engine, Assets assetId, Animation animation,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rectSprite, ZIndex zIndex)
{
    Entity entity = engine.createEntity("AnimatedImage");
    engine.addComponent(entity, Sprite(assetId, zIndex, rectSprite));
    engine.addComponent(entity, Transform(pos.x, pos.y, rotation, scale));
    (void)animation;
    return entity;
}

Entity createMovingBackground(gameEngine::GameEngine& engine, Assets assetId,
    sf::Vector2f pos, float rotation, float scale, sf::IntRect rectSprite,
    float speed, bool isHorizontal, bool shouldRepeat)
{
    Entity entity = engine.createEntity("Background");
    engine.addComponent(entity, Sprite(assetId, ZIndex::IS_BACKGROUND, rectSprite));
    engine.addComponent(entity, Transform(pos.x, pos.y, rotation, scale));
    engine.addComponent(entity, ScrollingBackground(speed, isHorizontal, shouldRepeat));
    testutils::movingBackgroundCount += 1;
    return entity;
}

std::vector<Entity> createButton(gameEngine::GameEngine& engine, std::string label,
    unsigned int textSize, sf::Color textColor, sf::Vector2f pos, float scale, sf::IntRect rectSprite,
    Assets noneAssetId, Assets hoverAssetId, Assets clickedAssetId,
    std::function<void ()> onClick)
{
    Entity buttonEntity = engine.createEntity("Button:" + label);
    ButtonTextures textures { noneAssetId, hoverAssetId, clickedAssetId };
    engine.addComponent(buttonEntity, ButtonComponent(textures, onClick));
    engine.addComponent(buttonEntity, Sprite(noneAssetId, ZIndex::IS_UI_HUD, rectSprite));
    engine.addComponent(buttonEntity, Transform(pos.x, pos.y, 0.0f, scale));

    Entity textEntity = engine.createEntity("ButtonText:" + label);
    engine.addComponent(textEntity, Text(label.c_str(), textColor, textSize, ZIndex::IS_UI_HUD));
    engine.addComponent(textEntity, Transform(pos.x, pos.y, 0.0f, 1.0f));

    testutils::buttonRecords.push_back({label, onClick, {buttonEntity, textEntity}});

    return {buttonEntity, textEntity};
}

std::vector<Entity> createRebindButton(gameEngine::GameEngine& engine, std::string label,
    GameAction action, int textSize, sf::Vector2f pos, float scale)
{
    (void)action;
    sf::IntRect dummyRect(0, 0, 1, 1);
    return createButton(engine, label, static_cast<unsigned int>(textSize), sf::Color::White,
        pos, scale, dummyRect, Assets::DEFAULT_NONE_BUTTON, Assets::DEFAULT_HOVER_BUTTON, Assets::DEFAULT_CLICKED_BUTTON,
        []() {});
}
 */