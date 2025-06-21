#pragma once
#include "UIComponent.h"
class ActionSelectionUI :
    public UIComponent
{
public:
    using Key = sf::Keyboard::Key;
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
        Key activationKey;

        UIIconSlot(const sf::Texture& iconTexture, const sf::Font& font, Key key)
            :icon(iconTexture),
            keyText(font),
            activationKey(key)
        {}
    };
    struct SlotData {
        TextureIdentifier textureId;
        std::string keyLabel;
        sf::Keyboard::Key activationKey;
    };
    void registerToEvents();
    void registerToSelectAttackEvent();
    void registerToStartSpellCooldownUIEvent();
    void createAttackSlots();
    void createSpellSlots();

    void createSlotsList(std::span<const SlotData> dataSpan, sf::Vector2f pos);
    void renderSlot(const UIIconSlot& slot);

    UIIconSlot createSlot(TextureIdentifier textureId, const sf::Vector2f& size, const sf::Vector2f& pos, 
        const std::string& keyLabel, Key key) const;

    const UIIconSlot* getSlotBasedOnActivationKey(Key key) const;
private:
    struct UICooldown {
        sf::RectangleShape overlay;
        sf::Text timerText;
        int* cooldown = nullptr;

        UICooldown(const sf::Font& font)
            :timerText(font)
        {}
    };
    void createCooldown(const UIIconSlot& slot, int* cooldown);
    void updateCooldownText(UICooldown& cd);
    void removeFinishedCooldowns();

    void updateCooldowns();
    void renderCooldowns();
private:
    const sf::Font& mFont;
    const float mSlotSize;
    const sf::Color mInactiveBorderColor;
    const sf::Color mActiveBorderColor;
    std::vector<UIIconSlot> mIconSlots;
    std::vector<UICooldown> mCooldowns;
    int mCooldownFrames;

};

