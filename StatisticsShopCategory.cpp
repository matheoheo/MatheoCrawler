#include "pch.h"
#include "StatisticsShopCategory.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"
#include "TextButton.h"
#include "ShopData.h"

StatisticsShopCategory::StatisticsShopCategory(GameContext& gameContext, Entity& player)
	:IShopCategory(gameContext, player)
{
	mItemsPerRow = 5;
	mItems.reserve(mItemsPerRow);
}

void StatisticsShopCategory::processEvents(const sf::Event event)
{
	processUpgradeButtonEvents(event);
}

void StatisticsShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	updateItemsAndDescription(mousePosition);
}

void StatisticsShopCategory::render()
{
	renderItems();
	renderItemDescription();
	renderStatsUpgradeText();
}

void StatisticsShopCategory::onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	constexpr TextureIdentifier tmpId{ TextureIdentifier::Devoyer }; //placeholder
	const std::array<ItemInitData, 5> itemsData =
	{ {
		{.iconId = tmpId, .name = "Health Points",    .type = StatType::Health},
		{.iconId = tmpId, .name = "Attack Damage",    .type = StatType::Attack},
		{.iconId = tmpId, .name = "Attack Speed",	  .type = StatType::AttackSpeed},
		{.iconId = tmpId, .name = "Physical Defence", .type = StatType::Defence},
		{.iconId = tmpId, .name = "Magic Defence",	  .type = StatType::MagicDefence}
	} };

	createItems(itemsData);
	setDescriptionPos(pos);
}

int StatisticsShopCategory::calculateItemCost(StatType type, int level) const
{
	int baseCost = getStatTypeCost(type);
	int dungeonBonus = Config::difficulityLevel * 50;
	return baseCost + (level * 100) + dungeonBonus;
}

int StatisticsShopCategory::calculateItemCost(const ShopItem& item) const
{
	int upgradeLevel = getUpgradeLevel(item.itemName);
	return calculateItemCost(item.statType, upgradeLevel);
}

int StatisticsShopCategory::getStatTypeCost(StatType type) const
{
	switch (type)
	{
	case StatType::Attack:
		return 950;
	case StatType::AttackSpeed:
		return 700;
	case StatType::Defence:
		return 550;
	case StatType::MagicDefence:
		return 450;
	case StatType::Health:
		return 800;
	}
	return 0;
}

float StatisticsShopCategory::getIncreasePerLvl(StatType type) const
{
	constexpr int attackFactor = 3;
	constexpr int defenceFactor = 4;
	constexpr int magicDefFactor = 3;
	constexpr int healthFactor = 8;
	switch (type)
	{
	case StatType::Attack:
		return attackFactor * Config::difficulityLevel;
		break;
	case StatType::AttackSpeed:
		return 0.05f;
		break;
	case StatType::Defence:
		return defenceFactor * Config::difficulityLevel;
		break;
	case StatType::Health:
		return healthFactor * Config::difficulityLevel;
		break;
	case StatType::MagicDefence:
		return magicDefFactor * Config::difficulityLevel;
		break;
	}
	return 0.0f;
}

std::string StatisticsShopCategory::getStatValueStr(StatType type, const CombatStatsComponent& stats) const
{
	switch (type)
	{
	case StatType::Attack:
		return std::to_string(stats.cAttackDamage);
		break;
	case StatType::AttackSpeed:
		return std::format("{:.2f}", stats.cAttackSpeed);
		break;
	case StatType::Defence:
		return std::to_string(stats.cDefence);
		break;
	case StatType::Health:
		return std::to_string(stats.cHealth);
		break;
	case StatType::MagicDefence:
		return std::to_string(stats.cMagicDefence);
		break;
	}
	return "";
}

void StatisticsShopCategory::upgrade(ShopItem& item)
{
	float increase = getIncreasePerLvl(item.statType);
	int i_increase = static_cast<int>(increase);
	auto& stats = player.getComponent<CombatStatsComponent>();

	switch (item.statType)
	{
	case StatType::Attack:
		stats.cAttackDamage += i_increase;
		break;
	case StatType::AttackSpeed:
		stats.cAttackSpeed += increase;
		break;
	case StatType::Defence:
		stats.cDefence += i_increase;
		break;
	case StatType::Health:
		stats.cHealth += i_increase;
		stats.cMaxHealth += i_increase;
		break;
	case StatType::MagicDefence:
		stats.cMagicDefence += i_increase;
	};
}

void StatisticsShopCategory::updateItemPrice(ShopItem& item)
{
	int price = calculateItemCost(item);
	item.cost = price;
	std::string priceStr = std::to_string(price);
	item.itemCostText.setString(priceStr);
	Utilities::setTextOriginOnCenter(item.itemCostText);

	setItemCurrencySprite(item);
}

std::string StatisticsShopCategory::getItemDescriptionStr(const ShopItem& item) const
{
	switch (item.statType)
	{
	case StatType::Attack:
		return "Boosts the damage you deal with each attack, enabling you to defeat enemies more quickly and efficiently. ";
		break;
	case StatType::AttackSpeed:
		return "Improves how fast you can strike, letting you perform more attacks within a shorter time.";
		break;
	case StatType::Defence:
		return "Reduces damage taken from physical attacks, such as weapons.";
		break;
	case StatType::Health:
		return "Increases your character's health points, allowing you to withstand more damage before falling in battle.";
		break;
	case StatType::MagicDefence:
		return "Decreases the damage received from magical attacks and spells.";
		break;
	}
	return "";
}

void StatisticsShopCategory::createStatUpgradeTexts(const ShopItem& item)
{
	if (!mItemDescription)
		return;

	const auto& combStats = player.getComponent<CombatStatsComponent>();
	auto increase = getIncreasePerLvl(item.statType);
	auto tmpCombStats = combStats;
	switch (item.statType)
	{
	case StatType::Attack:
		tmpCombStats.cAttackDamage += static_cast<int>(increase);
		break;
	case StatType::AttackSpeed:
		tmpCombStats.cAttackSpeed += increase;
		break;
	case StatType::Defence:
		tmpCombStats.cDefence += static_cast<int>(increase);
		break;
	case StatType::Health:
		tmpCombStats.cHealth += static_cast<int>(increase);
		break;
	case StatType::MagicDefence:
		tmpCombStats.cMagicDefence += static_cast<int>(increase);
	};
	auto currentValueStr = getStatValueStr(item.statType, combStats);
	auto nextValueStr = getStatValueStr(item.statType, tmpCombStats);

	const auto& descText = mItemDescription.value();
	float posX = mDescriptionPos.x;
	float posY = descText.getPosition().y + descText.getLocalBounds().size.y + ShopUtils::getItemDescSize() * 2;

	makeUpgradeText({ posX, posY }, item.itemName, currentValueStr, nextValueStr);
}
