#include "pch.h"
#include "SpellcraftShopCategory.h"
#include "ShopData.h"
#include "SpellHolder.h"
#include "Entity.h"
#include "Utilities.h"
#include "HealingSpellUpgradeStrategy.h"
#include "RegenSpellUpgradeStrategy.h"
#include "ProjectileSpellUpgradeStrategy.h"

SpellcraftShopCategory::SpellcraftShopCategory(GameContext& gameContext, Entity& player)
	:IShopCategory(gameContext, player)
{
	player.getComponent<PlayerResourcesComponent>().cGold = 1200000;
	mItemsPerRow = 4;
	mSpellCategories.reserve(mSpellTypesCount);
	mItems.reserve(mItemsPerRow);

	createStatTypeToSpellIdMap();
	createSpellUpgrades();
}

void SpellcraftShopCategory::processEvents(const sf::Event event)
{
	for (auto& button : mSpellCategories)
	{
		if (button.isPressed(event))
		{
			button.invoke();
			return;
		}
	}
	processUpgradeButtonEvents(event);
}

void SpellcraftShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	for (auto& button : mSpellCategories)
		button.update(mousePosition);
	updateItemsAndDescription(mousePosition);
}

void SpellcraftShopCategory::render()
{
	for (const auto& category : mSpellCategories)
		category.render(mGameContext.window);

	renderItems();
	renderItemDescription();
	renderStatsUpgradeText();
}

void SpellcraftShopCategory::onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	createSpellsCategories();
}

void SpellcraftShopCategory::updateItemPrice(ShopItem& item)
{
	int price = calculateSpellCost(item);
	item.cost = price;
	std::string priceStr = std::to_string(price);
	item.itemCostText.setString(priceStr);
	Utilities::setTextOriginOnCenter(item.itemCostText);

	//set currency sprite so it preceedes itemCostText
	const sf::Vector2f costPos{ item.itemCostText.getPosition() };
	const sf::Vector2f currSize{ item.currencySprite.getGlobalBounds().size };
	const sf::Vector2f currPos{ costPos.x - currSize.x - mCharSize, costPos.y - 3 };

	item.currencySprite.setPosition(currPos);
}

void SpellcraftShopCategory::upgrade(ShopItem& item)
{
	auto stat = item.statType;
	if (mSpellUpgrades.contains(stat))
	{
		mSpellUpgrades.at(stat)->upgrade();
	}
	/*
	auto increase = getStatIncrease(item.statType);
	int int_increase = static_cast<int>(increase);
	auto spellId = mStatTypeToSpellIdMap.at(item.statType);
	if (item.statType == StatType::QuickHealSpell || item.statType == StatType::MajorHealSpell)
	{
		SpellHolder::getInstance().get(spellId).healValue += increase;
	}
	else if (item.statType == StatType::HealthRegenSpell)
	{
		SpellHolder::getInstance().get(spellId).bonusHpRegen += int_increase;
	}
	else if (item.statType == StatType::ManaRegenSpell)
	{
		SpellHolder::getInstance().get(spellId).bonusManaRegen += int_increase;
	}
	else if (item.statType == StatType::WaterballSpell || item.statType == StatType::PureProjSpell ||
		item.statType == StatType::FireballSpell || item.statType == StatType::BloodballSpell)
	{
		SpellHolder::getInstance().get(spellId).projectile.value().minDmg += int_increase;
		SpellHolder::getInstance().get(spellId).projectile.value().maxDmg += int_increase;
	}*/
}

std::string SpellcraftShopCategory::getItemDescriptionStr(const ShopItem& item) const
{
	if (mStatTypeToSpellIdMap.contains(item.statType))
	{
		auto spellId = mStatTypeToSpellIdMap.at(item.statType);
		const auto& definition = SpellHolder::getInstance().getDefinition(spellId);
		const auto& info = definition.spellInfo;
		return info.shortDescription + " " + info.longDescription;
	}

	return std::string();
}

void SpellcraftShopCategory::createStatUpgradeTexts(const ShopItem& item)
{
	auto type = item.statType;
	if (mSpellUpgrades.contains(type))
	{
		auto& data = mSpellUpgrades.at(type);
		auto info = data->getSpellUpgradeInfo();
		makeUpgradeText(getNextUpgradeTextPos(), info.name + " " + info.unit, info.currValue, info.nextValue);
	}
}

void SpellcraftShopCategory::createStatTypeToSpellIdMap()
{
	mStatTypeToSpellIdMap =
	{
		{StatType::QuickHealSpell,   SpellIdentifier::QuickHeal},
		{StatType::MajorHealSpell,   SpellIdentifier::MajorHeal},
		{StatType::HealthRegenSpell, SpellIdentifier::HealthRegen},
		{StatType::ManaRegenSpell,   SpellIdentifier::ManaRegen},

		{StatType::WaterballSpell,   SpellIdentifier::WaterBall},
		{StatType::PureProjSpell,    SpellIdentifier::PureProjectile},
		{StatType::FireballSpell,    SpellIdentifier::Fireball},
		{StatType::BloodballSpell,   SpellIdentifier::Bloodball}
	};
}

void SpellcraftShopCategory::createSpellUpgrades()
{
	mSpellUpgrades[StatType::QuickHealSpell] = std::make_unique<HealingSpellUpgradeStrategy>
		(SpellIdentifier::QuickHeal, getStatIncrease(StatType::QuickHealSpell));
	mSpellUpgrades[StatType::MajorHealSpell] = std::make_unique<HealingSpellUpgradeStrategy>
		(SpellIdentifier::MajorHeal, getStatIncrease(StatType::MajorHealSpell));
	
	mSpellUpgrades[StatType::HealthRegenSpell] = std::make_unique<RegenSpellUpgradeStrategy>
		(SpellIdentifier::HealthRegen, getStatIncrease(StatType::HealthRegenSpell));
	mSpellUpgrades[StatType::ManaRegenSpell] = std::make_unique<RegenSpellUpgradeStrategy>
		(SpellIdentifier::ManaRegen, getStatIncrease(StatType::ManaRegenSpell));

	mSpellUpgrades[StatType::WaterballSpell] = std::make_unique<ProjectileSpellUpgradeStrategy>
		(SpellIdentifier::WaterBall, getStatIncrease(StatType::WaterballSpell));
	mSpellUpgrades[StatType::PureProjSpell] = std::make_unique<ProjectileSpellUpgradeStrategy>
		(SpellIdentifier::PureProjectile, getStatIncrease(StatType::PureProjSpell));
	mSpellUpgrades[StatType::FireballSpell] = std::make_unique<ProjectileSpellUpgradeStrategy>
		(SpellIdentifier::Fireball, getStatIncrease(StatType::FireballSpell));
	mSpellUpgrades[StatType::BloodballSpell] = std::make_unique<ProjectileSpellUpgradeStrategy>
		(SpellIdentifier::Bloodball, getStatIncrease(StatType::BloodballSpell));
}

void SpellcraftShopCategory::createSpellsCategories()
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	constexpr float margin = 15.f;
	using Init = TextButton::TextButtonInitData;
	const std::array<Init, mSpellTypesCount> data =
	{
		Init{.label = "Restoration", .callback = [this]() {
			onSpellCategoryPress(0);
		}},
		Init{.label = "Projectiles", .callback = [this]() {
			onSpellCategoryPress(1);
		}},
		Init{.label = "Effectives" , .callback = []() {}} //to initalize later
	};
	for (const auto& btnData : data)
	{
		mSpellCategories.emplace_back(font, btnData.label, mDescCharSize);
		mSpellCategories.back().setDefaultColor({ 100, 140, 255 });
		mSpellCategories.back().setCallback(btnData.callback);
	}

	sf::Vector2f pos{ mCategoryPos };
	float btnWidth = mSpellCategories[0].getSize().x;
	float space = (mCategorySize.x - (btnWidth * mSpellTypesCount)) / (mSpellTypesCount + 1);
	pos.x += space;

	for (auto& button : mSpellCategories)
	{
		button.setPosition(pos);
		pos.x += mSpellCategories.back().getSize().x + space;
	}

}

void SpellcraftShopCategory::onSpellCategoryPress(size_t id)
{
	mItems.clear();
	if (id == 0)
		createItems(getRestorationItemsInitData());
	else if (id == 1)
		createItems(getProjectilesItemsInitData());

	mSpellCategories.clear();
	setDescriptionPos(mCategoryPos);
}

std::vector<ItemInitData> SpellcraftShopCategory::getRestorationItemsInitData() const
{
	constexpr std::array<SpellShopMapping, 4> restData =
	{{
		{SpellIdentifier::QuickHeal,   StatType::QuickHealSpell}, 
		{SpellIdentifier::MajorHeal,   StatType::MajorHealSpell},
		{SpellIdentifier::HealthRegen, StatType::HealthRegenSpell},
		{SpellIdentifier::ManaRegen,   StatType::ManaRegenSpell}
	}};
	
	return createItemInitData(restData);
}

std::vector<ItemInitData> SpellcraftShopCategory::getProjectilesItemsInitData() const
{
	constexpr std::array<SpellShopMapping, 4> restData =
	{ {
		{SpellIdentifier::WaterBall,      StatType::WaterballSpell},
		{SpellIdentifier::PureProjectile, StatType::PureProjSpell},
		{SpellIdentifier::Fireball,		  StatType::FireballSpell},
		{SpellIdentifier::Bloodball,      StatType::BloodballSpell}
	} };

	return createItemInitData(restData);
}

std::vector<ItemInitData> SpellcraftShopCategory::createItemInitData(std::span<const SpellShopMapping> spellsData) const
{
	std::vector<ItemInitData> result;
	for (const auto& data : spellsData)
	{
		const SpellDefinition* definition = &SpellHolder::getInstance().getDefinition(data.spellId);
		const auto& info = definition->spellInfo;
		result.push_back(ItemInitData{ info.textureId, info.name, data.statType });
	}

	return result;
}

int SpellcraftShopCategory::calculateSpellCost(const ShopItem& item) const
{
	int upgradeLevel = getUpgradeLevel(item.itemName);
	int basicCost = getBasicItemCost(item.statType);
	int bonus = Config::difficulityLevel * 77;

	return basicCost + (upgradeLevel * 124) + bonus;
}

int SpellcraftShopCategory::getBasicItemCost(StatType type) const
{
	switch (type)
	{
	case StatType::QuickHealSpell:
		return 1000;
	case StatType::MajorHealSpell:
		return 1800;
	case StatType::HealthRegenSpell:
		return 1300;
	case StatType::ManaRegenSpell:
		return 1600;

	case StatType::WaterballSpell:
		return 1500;
	case StatType::PureProjSpell:
		return 2500;
	case StatType::FireballSpell:
		return 4500;
	case StatType::BloodballSpell:
		return 3000;
	}
	return 0;
}

float SpellcraftShopCategory::getStatIncrease(StatType type) const
{
	switch (type)
	{
	case StatType::QuickHealSpell:
		return 0.02f;
	case StatType::MajorHealSpell:
		return 0.02f;
	case StatType::HealthRegenSpell:
		return 3;
	case StatType::ManaRegenSpell:
		return 2;

	case StatType::WaterballSpell:
		return 4;
	case StatType::PureProjSpell:
		return 5;
	case StatType::FireballSpell:
		return 7;
	case StatType::BloodballSpell:
		return 6;
	}
	return 0;
}

