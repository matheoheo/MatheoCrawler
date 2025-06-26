#pragma once
#include "IShopCategory.h"
class SpellcraftShopCategory :
    public IShopCategory
{
public:
	SpellcraftShopCategory(GameContext& gameContext, Entity& player);

	// Inherited via UIComponent
	virtual void processEvents(const sf::Event event) override;
	virtual void update(const sf::Vector2f & mousePosition, const sf::Time & deltaTime) override;
	virtual void render() override;

protected:
    // Inherited via IShopCategory
    virtual void onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize) override;
private:

private:

};

