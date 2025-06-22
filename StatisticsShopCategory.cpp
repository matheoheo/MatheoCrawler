#include "pch.h"
#include "StatisticsShopCategory.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"
#include "TextButton.h"

StatisticsShopCategory::StatisticsShopCategory(GameContext& gameContext, Entity& player)
	:IShopCategory(gameContext, player),
	mCharSize(static_cast<unsigned int>(Config::getCharacterSize() / 3))
{
	mItems.reserve(5);
}

void StatisticsShopCategory::processEvents(const sf::Event event)
{
}

void StatisticsShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
}

void StatisticsShopCategory::render()
{
	for (const auto& item : mItems)
		renderItem(item);
}

void StatisticsShopCategory::create(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	const int itemsPerRow = 5;
	float padding = 10.f;
	float itemHeight = mIconSize.y + mCharSize * 2.f;
	float itemWidth = (categorySize.x - padding * (itemsPerRow + 1)) / itemsPerRow;

	mItemBoundsSize = { itemWidth, itemHeight };

	createItems(pos);
}

void StatisticsShopCategory::createItems(sf::Vector2f pos)
{
	constexpr TextureIdentifier tmpId{ TextureIdentifier::Devoyer }; //placeholder
	const std::array<ItemInitData, 5> itemsData =
	{{
		{.iconId = tmpId, .name = "Health Points",    .type = StatType::Health},
		{.iconId = tmpId, .name = "Attack Damage",    .type = StatType::Attack},
		{.iconId = tmpId, .name = "Attack Speed",	  .type = StatType::AttackSpeed},
		{.iconId = tmpId, .name = "Magic Defence",	  .type = StatType::MagicDefence},
		{.iconId = tmpId, .name = "Physical Defence", .type = StatType::Defence}
	}};

	for (const auto& data : itemsData)
	{
		tryMakeStatisticUpgradeData(data.name);
		mItems.emplace_back(createItem(data, pos, mCharSize));
		pos.x += mItemBoundsSize.x;
	}

}

void StatisticsShopCategory::tryMakeStatisticUpgradeData(const std::string& name)
{
	auto& statsComp = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgrades = statsComp.cUpgradesMap;

	upgrades.try_emplace(name, 1);
}

int StatisticsShopCategory::getItemCost(StatType type, int level) const
{
	constexpr float growthFactor = 1.4f;
	int baseCost = getStatTypeCost(type);
	return static_cast<int>(baseCost * std::pow(growthFactor, level));
}

int StatisticsShopCategory::getItemCost(const ShopItem& item) const
{
	int upgradeLevel = getUpgradeLevel(item.itemName);
	return getItemCost(item.statType, upgradeLevel);
}

int StatisticsShopCategory::getUpgradeLevel(const std::string& upgradeName) const
{
	auto& statsComp = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgrades = statsComp.cUpgradesMap;

	if (auto it = upgrades.find(upgradeName); it != std::end(upgrades))
		return it->second;

	return 10000;
}

int StatisticsShopCategory::getStatTypeCost(StatType type) const
{
	switch (type)
	{
	case StatType::Attack:
		return 650;
	case StatType::AttackSpeed:
		return 500;
	case StatType::Defence:
		return 450;
	case StatType::MagicDefence:
		return 300;
	case StatType::Health:
		return 550;
	}
	return 1000;
}

StatisticsShopCategory::ShopItem StatisticsShopCategory::createItem(const ItemInitData& itemInit, const sf::Vector2f& pos,
	unsigned int charSize)
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	const auto& texture = mGameContext.textures.get(itemInit.iconId);
	constexpr float margin = 5.f;
	ShopItem item(font, texture, itemInit.type);
	item.itemName = itemInit.name;
	item.itemVisual.setPosition(pos);
	Utilities::scaleSprite(item.itemVisual, mIconSize);

	const sf::Vector2f itemNamePos{ pos.x, pos.y + mIconSize.y + margin };
	item.itemNameText.setCharacterSize(mCharSize);
	item.itemNameText.setString(item.itemName);
	item.itemNameText.setFillColor(sf::Color{ 230, 230, 230 });
	item.itemNameText.setPosition(itemNamePos);

	const sf::Vector2f itemCostPos{ pos.x, itemNamePos.y + mCharSize + margin };
	item.itemCostText.setCharacterSize(charSize);
	item.itemCostText.setString("57893"); //placeholder
	item.itemCostText.setFillColor(sf::Color{ 255, 223, 50 });
	item.itemCostText.setPosition(itemCostPos);

	const sf::Vector2f buttonPos{ pos.x, itemCostPos.y + mCharSize + margin };
	item.upgradeButton.setPosition(buttonPos);

	return item;
}

void StatisticsShopCategory::renderItem(const ShopItem& item)
{
	auto& window = mGameContext.window;
	window.draw(item.itemVisual);
	window.draw(item.itemNameText);
	window.draw(item.itemCostText);
	item.upgradeButton.render(window);
}


/*
struct CombatStatsComponent : public IComponent
{
	int cHealth;
	int cMaxHealth;
	int cAttackDamage;
	int cAttackRange; //in tiles
	int cDefence;
	float cAttackSpeed; //attacks per second
	int cMana;
	int cMaxMana;
	int cMagicDefence;


	health, damage, defence, attackSpeed, magicDefence	
*/