#include "pch.h"
#include "IShopCategory.h"
#include "Config.h"

IShopCategory::IShopCategory(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player)
{
	float iconSize = Config::fWindowSize.x * 0.035;
	mIconSize = { iconSize, iconSize };
}

void IShopCategory::processEvents(const sf::Event event)
{
}

void IShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
}

void IShopCategory::render()
{
}
