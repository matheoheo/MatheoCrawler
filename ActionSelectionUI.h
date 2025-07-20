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
    struct UICooldown {
        sf::RectangleShape overlay;
        sf::Text timerText;
        int* cooldown = nullptr;

        UICooldown(const sf::Font& font)
            :timerText(font)
        {}
    };
    struct UIIconSlot {
        std::optional<sf::Sprite> icon;
        sf::RectangleShape border;
        sf::Text keyText;
        Key activationKey;
        std::optional<UICooldown> uiCooldown;

        UIIconSlot(const sf::Font& font, Key key)
            :keyText(font),
            activationKey(key)
        {}
    };
    struct SlotData {
        std::string keyLabel;
        sf::Keyboard::Key activationKey;
    };
    void registerToEvents();
    void registerToSelectAttackEvent();
    void registerToStartSpellCooldownUIEvent();
    void registerToReBindSpellActionEvent();
    void registerToRemoveActionBindEvent();
    void createAttackSlots();
    void createSpellSlots();

    void createSlotsList(std::span<const SlotData> dataSpan, sf::Vector2f pos);
    void setSlotIcon(UIIconSlot& slot, TextureIdentifier id);
    void renderSlot(const UIIconSlot& slot);

    UIIconSlot createSlot(const sf::Vector2f& pos, const sf::Vector2f& size,
        const std::string& keyLabel, Key key) const;

    UIIconSlot* getSlotBasedOnActivationKey(Key key);
    TextureIdentifier getTextureIDBasedOnSpell(SpellIdentifier spellID) const;
private:
    
    void createCooldown(UIIconSlot& slot, int* cooldown);
    void updateCooldownText(UICooldown& cd);
    void removeFinishedCooldowns();
    void updateCooldowns();
    void renderCooldowns();

    void setSlotCooldown(UIIconSlot& slot, int * cd);
private:
    const sf::Font& mFont;
    const float mSlotSize;
    const sf::Color mInactiveBorderColor;
    const sf::Color mActiveBorderColor;
    std::vector<UIIconSlot> mIconSlots;
    std::vector<UICooldown> mCooldowns;
    int mCooldownFrames;

};

