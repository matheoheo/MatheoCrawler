#include "pch.h"
#include "IShopCategory.h"
#include "Config.h"
#include "Entity.h"
#include "ShopData.h"

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

void IShopCategory::create(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	mCategoryPos = pos;
	mCategorySize = categorySize;

	onCreate(pos, categorySize);
}


int IShopCategory::getUpgradeLevel(const std::string& upgradeName) const
{
	auto& statsComp = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgrades = statsComp.cUpgradesMap;

	if (auto it = upgrades.find(upgradeName); it != std::end(upgrades))
		return it->second;

	return 0;
}

bool IShopCategory::canBuy(const ShopItem& item) const
{
	const auto& goldComp = player.getComponent<PlayerResourcesComponent>();
	return goldComp.cGold >= item.cost;
}

void IShopCategory::upgradeStatisticLevel(const ShopItem& item)
{
	auto& upgradeStats = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgMap = upgradeStats.cUpgradesMap;

	if (upgMap.contains(item.itemName))
		++upgMap.at(item.itemName);
}

void IShopCategory::notifyUIAfterBuy()
{
	mGameContext.eventManager.notify<UpdatePlayerStatusEvent>(UpdatePlayerStatusEvent());
	mGameContext.eventManager.notify<UpdatePlayerResourcesEvent>(UpdatePlayerResourcesEvent());
}

void IShopCategory::tryBuy(ShopItem& item)
{
	if (!canBuy(item))
		return;

	auto& goldComp = player.getComponent<PlayerResourcesComponent>();
	goldComp.cGold -= item.cost;
	upgrade(item);
	upgradeStatisticLevel(item);
	updateItemPrice(item);

	notifyUIAfterBuy();
}

void IShopCategory::createItemDescription(const sf::Vector2f& pos, const std::string& descText)
{
	const sf::Font& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	mItemDescription = ShopUtils::createItemDescription(font, descText, pos, mCategorySize.x);
}

void IShopCategory::removeItemDescription()
{
	mItemDescription = std::nullopt;
}

void IShopCategory::renderItemDescription()
{
	if (mItemDescription)
		mGameContext.window.draw(mItemDescription.value());
}
