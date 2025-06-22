#include "pch.h"
#include "Shop.h"
#include "StatisticsShopCategory.h"

Shop::Shop(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player)
{
	mActiveCategory = std::make_unique<StatisticsShopCategory>(mGameContext, player);
	mActiveCategory->create({ 400.f, 400.f }, { 600.f, 150.f });
}

void Shop::processEvents(const sf::Event event)
{
	mActiveCategory->processEvents(event);
}

void Shop::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	mActiveCategory->update(mousePosition, deltaTime);
}

void Shop::render()
{
	mActiveCategory->render();
}
