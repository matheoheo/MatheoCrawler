#pragma once
#include "UIComponent.h"
#include "Entity.h"
class PlayerStatusUI :
    public UIComponent
{
public:
    PlayerStatusUI(GameContext& gameContext, Entity& player);
    // Inherited via UIComponent
    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void registerToEvents();
    void registerToPlayerGotHitEvent();
    void registerToUpdatePlayerStatusEvent();
    void createTwarf();
    void createHealthBar();
    void createHpText();
    void updateHpText();
    void positionHpText();
    void createManaBar();
    void createManaText();
    void updateManaText();
    //might be called after player got hit/casted spell/regenerated etc.
    void updateStatus();

    void createText(sf::Text& text);
    void updateText(sf::Text& text, int curr, int max);
    void positionText(sf::Text& text, sf::RectangleShape& rect);
private:
    sf::Sprite mTwarf; //player's icon.
    sf::RectangleShape mTwarfBorder;
    sf::Text mHpText;
    sf::Text mManaText;
};

