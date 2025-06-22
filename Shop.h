#pragma once
#include "UIComponent.h"
#include "IShopCategory.h"

class Shop :
    public UIComponent
{
public:
    Shop(GameContext& gameContext, Entity& player);

    // Inherited via UIComponent
    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
private:

private:
    std::unique_ptr<IShopCategory> mActiveCategory;
};

