#include "pch.h"
#include "SpellcraftShopCategory.h"
#include "ShopData.h"
#include "SpellHolder.h"
#include "Entity.h"
#include "Utilities.h"
#include "HealingSpellUpgradeStrategy.h"
#include "RegenSpellUpgradeStrategy.h"
#include "ProjectileSpellUpgradeStrategy.h"
#include "AOEUpgradeStrategy.h"

SpellcraftShopCategory::SpellcraftShopCategory(GameContext& gameContext, Entity& player)
	:IShopCategory(gameContext, player)
{
	player.getComponent<PlayerResourcesComponent>().cGold = 1200000;
	mItemsPerRow = 4;
	mSpellCategories.reserve(mSpellTypesCount);
	mItems.reserve(mItemsPerRow);

	createStatTypeToSpellIdMap();
	createSpellUpgrades();
	initAssignables();
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
	handleAssignPopupClick(event);
	handleAssignButtonClick(event);
}

void SpellcraftShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	for (auto& button : mSpellCategories)
		button.update(mousePosition);

	updateItemsAndDescription(mousePosition);
	updateAssignPopupButtons(mousePosition);
}

void SpellcraftShopCategory::render()
{
	for (const auto& category : mSpellCategories)
		category.render(mGameContext.window);

	renderItems();
	renderItemDescription();
	renderStatsUpgradeText();
	renderAssignablePopup();
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

	setItemCurrencySprite(item);
}

void SpellcraftShopCategory::upgrade(ShopItem& item)
{
	auto stat = item.statType;

	if (mSpellUpgrades.contains(stat))
		mSpellUpgrades.at(stat)->upgrade();
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

void SpellcraftShopCategory::createAssignableOptions()
{
	//This is hardcoded to correspond to UI Action Slots
	using Key = sf::Keyboard::Key;
	mAssignableOptions.emplace_back(Key::Z, "Z");
	mAssignableOptions.emplace_back(Key::X, "X");
	mAssignableOptions.emplace_back(Key::C, "C");
	mAssignableOptions.emplace_back(Key::V, "V");
	mAssignableOptions.emplace_back(Key::B, "B");
}

void SpellcraftShopCategory::setAssignableButtonsCallbacks()
{
	for (auto& button : mAssignableButtons)
	{
		button.setCallback([this, &button]()
			{
				notifySpellBindEvent(button);
			});
	}
}

void SpellcraftShopCategory::assignToSpellbook(const ShopItem& item)
{
	if (!mStatTypeToSpellIdMap.contains(item.statType))
		return;

	auto& spells = player.getComponent<SpellbookComponent>().cSpells;
	auto spellId = mStatTypeToSpellIdMap.at(item.statType);
	if (spells.contains(spellId))
		return; //means this spell is already in spellbook

	spells.emplace(spellId, SpellInstance{
		.data = &SpellHolder::getInstance().get(spellId),
		.cooldownRemaining = 0,
		.durationRemaining = 0
		});

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
		{StatType::BloodballSpell,   SpellIdentifier::Bloodball},

		{StatType::FrostPillarSpell, SpellIdentifier::FrostPillar},
		{StatType::BladeDanceSpell,  SpellIdentifier::BladeDance}
	};
}

void SpellcraftShopCategory::createSpellUpgrades()
{
	using Stat = StatType;
	using ID = SpellIdentifier;
	using HealStrat = HealingSpellUpgradeStrategy;
	using RegStrat  = RegenSpellUpgradeStrategy;
	using ProjStrat = ProjectileSpellUpgradeStrategy;
	using AOEStrat = AOEUpgradeStrategy;

	mSpellUpgrades[Stat::QuickHealSpell] = std::make_unique<HealStrat>(ID::QuickHeal, getStatIncrease(Stat::QuickHealSpell));
	mSpellUpgrades[Stat::MajorHealSpell] = std::make_unique<HealStrat>(ID::MajorHeal, getStatIncrease(Stat::MajorHealSpell));
	
	mSpellUpgrades[Stat::HealthRegenSpell] = std::make_unique<RegStrat>(ID::HealthRegen, getStatIncrease(Stat::HealthRegenSpell));
	mSpellUpgrades[Stat::ManaRegenSpell]   = std::make_unique<RegStrat>(ID::ManaRegen, getStatIncrease(Stat::ManaRegenSpell));

	mSpellUpgrades[Stat::WaterballSpell] = std::make_unique<ProjStrat>(ID::WaterBall, getStatIncrease(Stat::WaterballSpell));
	mSpellUpgrades[Stat::PureProjSpell]  = std::make_unique<ProjStrat>(ID::PureProjectile, getStatIncrease(Stat::PureProjSpell));
	mSpellUpgrades[Stat::FireballSpell]  = std::make_unique<ProjStrat>(ID::Fireball, getStatIncrease(Stat::FireballSpell));
	mSpellUpgrades[Stat::BloodballSpell] = std::make_unique<ProjStrat>(ID::Bloodball, getStatIncrease(Stat::BloodballSpell));

	mSpellUpgrades[Stat::FrostPillarSpell] = std::make_unique<AOEStrat>(ID::FrostPillar, getStatIncrease(Stat::FrostPillarSpell));
	mSpellUpgrades[Stat::BladeDanceSpell] = std::make_unique<AOEStrat>(ID::BladeDance, getStatIncrease(Stat::BladeDanceSpell));

}

void SpellcraftShopCategory::setUpgradeLevelsLimit()
{
	using PairType = std::pair<StatType, int>;
	constexpr std::array<PairType, 2> limits =
	{ {
		{StatType::QuickHealSpell, 11},
		{StatType::MajorHealSpell, 16}
	} };

	for (auto& item : mItems)
	{
		auto it = std::ranges::find(limits, item.statType, &PairType::first);
		if (it != std::ranges::end(limits))
		{
			item.maxUpgradeLevel = it->second;
		}
	}
}

void SpellcraftShopCategory::setMinLevelToAssign()
{
	//For most of spells, we do not want them to be assignable at start.
	//User must upgrade them once first.
	//This function, defines which one require this.
	constexpr int minUpgradeLevel = 2;
	constexpr size_t count = 8;
	constexpr std::array<StatType, count> requireUpgrade =
	{
		StatType::MajorHealSpell, StatType::HealthRegenSpell, StatType::ManaRegenSpell,
		StatType::PureProjSpell, StatType::BloodballSpell, StatType::FireballSpell,
		StatType::FrostPillarSpell, StatType::BladeDanceSpell
	};

	for (auto& item : mItems)
	{
		auto it = std::ranges::find(requireUpgrade, item.statType);
		if (it == std::ranges::end(requireUpgrade))
			continue;

		item.minLevelToAssign = minUpgradeLevel;
	}
}

void SpellcraftShopCategory::notifySpellBindEvent(const TextButton& button)
{
	if (!mLastPressedItem)
		return;

	//We need to get the std::string from the button
	//It looks like this: [Z] [X] ... and so on
	//so we get the button string and then just take the key at index [1]
	auto buttonStr = button.getButtonString();
	std::string keyStr{ buttonStr[1] };

	//Now we need map this key in string to sf::Keyboard::Key object
	//We can do this by iterating over mAssignableOptions map
	sf::Keyboard::Key realKey{ sf::Keyboard::Key::Z }; //by default Z
	for (const auto& [key, val] : mAssignableOptions)
	{
		if (val == keyStr)
		{
			realKey = key;
			break;
		}
	}
	//now, we have key to which spell should be assigned to.
	
	//Also, based on mLastPressed item, we need to get proper SpellIdentifier
	auto type = mLastPressedItem->statType;
	if (!mStatTypeToSpellIdMap.contains(type))
		return;
	auto spellId = mStatTypeToSpellIdMap.at(type); // we have our spellId
	//now we can send bind event.
	mGameContext.eventManager.notify<BindSpellEvent>(BindSpellEvent(realKey, spellId));
}

void SpellcraftShopCategory::createSpellsCategories()
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	constexpr float margin = 15.f;
	using Init = TextButton::TextButtonInitData;
	const std::array<Init, mSpellTypesCount> data =
	{
		Init{.label = "Restoration", .callback = [this]() {
			onSpellCategoryPress(SpellcraftConfig::restorationId);
		}},
		Init{.label = "Projectiles", .callback = [this]() {
			onSpellCategoryPress(SpellcraftConfig::projectilesId);
		}},
		Init{.label = "Area Of Effect" , .callback = [this]() {
			onSpellCategoryPress(SpellcraftConfig::aoeId);
		}} //to initalize later
	};
	for (const auto& btnData : data)
	{
		mSpellCategories.emplace_back(font, btnData.label, mDescCharSize);
		mSpellCategories.back().setDefaultColor({ 100, 140, 255 });
		mSpellCategories.back().setCallback(btnData.callback);
	}

	sf::Vector2f pos{ mCategoryPos };
	float btnWidth = mSpellCategories[2].getSize().x; //we take last button size, because it is the longeset.
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
	switch (id)
	{
	case SpellcraftConfig::restorationId:
		createItems(getRestorationItemsInitData());
		break;
	case SpellcraftConfig::projectilesId:
		createItems(getProjectilesItemsInitData());
		break;
	case SpellcraftConfig::aoeId:
		createItems(getAreaOfEffectItemsInitData());
		break;
	default:
		break;
	}

	mSpellCategories.clear();
	setDescriptionPos(mCategoryPos);
	setUpgradeLevelsLimit();
	setMinLevelToAssign();
	makeItemsAssignable();
	determineItemsBorderColor();
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
	constexpr std::array<SpellShopMapping, 4> projData =
	{ {
		{SpellIdentifier::WaterBall,      StatType::WaterballSpell},
		{SpellIdentifier::PureProjectile, StatType::PureProjSpell},
		{SpellIdentifier::Fireball,		  StatType::FireballSpell},
		{SpellIdentifier::Bloodball,      StatType::BloodballSpell}
	} };

	return createItemInitData(projData);
}

std::vector<ItemInitData> SpellcraftShopCategory::getAreaOfEffectItemsInitData() const
{
	constexpr std::array<SpellShopMapping, 2> aoeData =
	{{
		{SpellIdentifier::FrostPillar, StatType::FrostPillarSpell},
		{SpellIdentifier::BladeDance,  StatType::BladeDanceSpell}
	} };

	return createItemInitData(aoeData);
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
		return 2200;
	case StatType::FireballSpell:
		return 3400;
	case StatType::BloodballSpell:
		return 2800;

	case StatType::FrostPillarSpell:
		return 4000;
	case StatType::BladeDanceSpell:
		return 5000;
	}
	return 0;
}

float SpellcraftShopCategory::getStatIncrease(StatType type) const
{
	//Those values are just how much the spell's heal/regen/damage value increases per upgrade
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

	case StatType::FrostPillarSpell:
		return 6;
	case StatType::BladeDanceSpell:
		return 3;
	}
	return 0;
}

