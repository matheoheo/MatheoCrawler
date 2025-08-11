#include "pch.h"
#include "BladeArtsShopCategory.h"
#include "Config.h"
#include "Utilities.h"
#include "Entity.h"
BladeArtsShopCategory::BladeArtsShopCategory(GameContext& gameContext, Entity& player)
    :IShopCategory(gameContext, player)
{
    mItemsPerRow = 3;
    mItems.reserve(mItemsPerRow);
	initAssignables();
}

void BladeArtsShopCategory::processEvents(const sf::Event event)
{
	processUpgradeButtonEvents(event);
}

void BladeArtsShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	updateItemsAndDescription(mousePosition);
}

void BladeArtsShopCategory::render()
{
	renderItems();
	renderItemDescription();
	renderStatsUpgradeText();
}

void BladeArtsShopCategory::onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	constexpr int maxUpgradeLevel = 21;
	const std::array<ItemInitData, 3> itemsData =
	{ {
		{TextureIdentifier::Attack1Icon, "Left Attack", StatType::LeftAttack, maxUpgradeLevel},
		{TextureIdentifier::Attack2Icon, "Right Attack", StatType::RightAttack, maxUpgradeLevel},
		{TextureIdentifier::Attack3Icon, "Thrust Attack", StatType::ThrustAttack, maxUpgradeLevel}
	} };

	createItems(itemsData);
	setDescriptionPos(pos);
	determineItemsBorderColor();
}

void BladeArtsShopCategory::updateItemPrice(ShopItem& item)
{
	int price = calculateItemCost(item);
	item.cost = price;
	std::string priceStr = std::to_string(price);
	item.itemCostText.setString(priceStr);
	Utilities::setTextOriginOnCenter(item.itemCostText);

	setItemCurrencySprite(item);
}

void BladeArtsShopCategory::upgrade(ShopItem& item)
{
	AttackData* properAttack = getAttackDataForItem(item);
	if (!properAttack)
		return;

	float increase = getStatIncrease(item.statType);

	switch (item.statType)
	{
	case StatType::LeftAttack:
		properAttack->speedMultiplier += increase;
		break;
	case StatType::RightAttack:
		properAttack->damageMultiplier += increase;
		break;
	case StatType::ThrustAttack:
		properAttack->damageMultiplier += increase;
		properAttack->speedMultiplier  += increase;
		break;
	}
}

std::string BladeArtsShopCategory::getItemDescriptionStr(const ShopItem& item) const
{
	switch (item.statType)
	{
	case StatType::LeftAttack: return "A wide sweep striking ahead and to the left. Control the battlefield with elegant agression.";
	case StatType::RightAttack: return "A precise arc to the right and forward. Perfect for flanking strikes.";
	case StatType::ThrustAttack: return "A focused lunge that pierces straight ahead - high damage, narrow reach.";
	}

    return "_default_blade_art_desc";
}

void BladeArtsShopCategory::createStatUpgradeTexts(const ShopItem& item)
{
	AttackData* properAttack = getAttackDataForItem(item);
	if (!properAttack)
		return;

	const std::string attSpeedStr = "Attack Speed Multiplier";
	const std::string dmgStr = "Attack Damage Multiplier";
	float increase = getStatIncrease(item.statType);

	const std::string currAttSpeedVal = std::format("{:.2f}", properAttack->speedMultiplier);
	const std::string currDmgVal      = std::format("{:.2f}", properAttack->damageMultiplier);
	const std::string nextAttSpeedVal = std::format("{:.2f}", properAttack->speedMultiplier + increase);
	const std::string nextDmgVal      = std::format("{:.2f}", properAttack->damageMultiplier + increase);

	if (item.statType == StatType::LeftAttack)
		makeUpgradeText(getNextUpgradeTextPos(), attSpeedStr, currAttSpeedVal, nextAttSpeedVal);
	else if (item.statType == StatType::RightAttack)
		makeUpgradeText(getNextUpgradeTextPos(), dmgStr, currDmgVal, nextDmgVal);
	else if (item.statType == StatType::ThrustAttack)
	{
		makeUpgradeText(getNextUpgradeTextPos(), attSpeedStr, currAttSpeedVal, nextAttSpeedVal);
		makeUpgradeText(getNextUpgradeTextPos(), dmgStr, currDmgVal, nextDmgVal);
	}
}

void BladeArtsShopCategory::createAssignableOptions()
{
	//This is hardcoded to correspond to UI Action Slots
	using Key = sf::Keyboard::Key;
	mAssignableOptions.emplace_back(Key::Num1, "1");
	mAssignableOptions.emplace_back(Key::Num2, "2");
	mAssignableOptions.emplace_back(Key::Num3, "3");
}

int BladeArtsShopCategory::getBaseItemPrice(const ShopItem& item) const
{
	switch (item.statType)
	{
	case StatType::LeftAttack:   return 510;
	case StatType::RightAttack:  return 560;
	case StatType::ThrustAttack: return 630;
	}

	return 1;
}

int BladeArtsShopCategory::calculateItemCost(const ShopItem& item) const
{
	constexpr int dungeonLevelMultiplier = 50;
	const int upgradeLevelMultiplier = 35;
	int baseCost = getBaseItemPrice(item);
	int upgradeLevel = getUpgradeLevel(item.itemName);
	int levelBonus = Config::difficulityLevel * dungeonLevelMultiplier;

	return baseCost + (upgradeLevel * upgradeLevelMultiplier) + dungeonLevelMultiplier;
}

float BladeArtsShopCategory::getStatIncrease(StatType type) const
{
	constexpr float leftAttackIncrease = 0.03f; //of attack speed multiplier
	constexpr float rightAttackIncrease = 0.03f; //of damage amount multiplier
	constexpr float thrustAttackIncrease = 0.02f; //of attack speed and damage amount
	switch (type)
	{
	case StatType::LeftAttack: return leftAttackIncrease;
	case StatType::RightAttack: return rightAttackIncrease;
	case StatType::ThrustAttack: return thrustAttackIncrease;
	default: break;
	}

	return 0.0f;
}

AttackData* BladeArtsShopCategory::getAttackDataForItem(const ShopItem& item) const
{
	using Pair = std::pair<StatType, AnimationIdentifier>;
	constexpr std::array<Pair, 3> mapping =
	{ {
		{StatType::LeftAttack, AnimationIdentifier::Attack1},
		{StatType::RightAttack, AnimationIdentifier::Attack2},
		{StatType::ThrustAttack, AnimationIdentifier::Attack3}
	} };
	auto& attackComp = player.getComponent<AttackComponent>();
	auto& attackMap = attackComp.cAttackDataMap;

	auto it = std::ranges::find(mapping, item.statType, &Pair::first);
	if (it == std::ranges::end(mapping) || !attackMap.contains(it->second))
		return nullptr;

	return &attackMap.at(it->second);
}
