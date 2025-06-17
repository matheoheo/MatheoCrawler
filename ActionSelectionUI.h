#pragma once
#include "UIComponent.h"
class ActionSelectionUI :
    public UIComponent
{
public:
    ActionSelectionUI(GameContext& gameContext, Entity& player);
    // Inherited via UIComponent
    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    struct UIIconSlot {
        sf::Sprite icon;
        sf::RectangleShape border;
        sf::Text keyText;

        UIIconSlot(const sf::Texture& iconTexture, const sf::Font& font)
            :icon(iconTexture),
            keyText(font)
        {}
    };
    struct SlotData {
        TextureIdentifier textureId;
        std::string keyLabel;
    };
    void registerToEvents();
    void registerToSelectAttackEvent();
    void createAttackSlots();
    void renderSlot(const UIIconSlot& slot);

    UIIconSlot createSlot(TextureIdentifier textureId, const sf::Vector2f& size, const sf::Vector2f& pos, const std::string& keyLabel) const;
private:
    const sf::Font& mFont;
    const float mSlotSize;
    const sf::Color mInactiveBorderColor;
    const sf::Color mActiveBorderColor;
    std::vector<UIIconSlot> mIconSlots;
};

