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
    void createTwarf();
    void createHealthBar();
    void createHpText();
    void updateHpText();
    void positionHpText();
private:
    sf::Sprite mTwarf; //player's icon.
    sf::RectangleShape mTwarfBorder;
    sf::Text mHpText;
};

