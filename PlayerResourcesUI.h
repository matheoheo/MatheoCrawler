#pragma once
#include "UIComponent.h"
class PlayerResourcesUI :
    public UIComponent
{
public:
    PlayerResourcesUI(GameContext& gameContext, Entity& player);

    // Inherited via UIComponent
    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void registerToEvents();
    void createSuash();
    void createGoldText();
    void updateGoldText();
private:
    sf::Sprite mSuash; //gold icon
    sf::Text mGoldText;
};

