#include "pch.h"
#include "StatisticsShopCategory.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"
#include "TextButton.h"
#include "ShopData.h"

StatisticsShopCategory::StatisticsShopCategory(GameContext& gameContext, Entity& player)
	:IShopCategory(gameContext, player),
	mCharSize(static_cast<unsigned int>(Config::getCharacterSize() / 3)),
	mItemsPerRow(5),
	mCategoryWidth(0.f),
	mDescCharSize(Config::getCharacterSize() / 2),
	mDescColor({ 200, 200, 200 })
{
	mItems.reserve(mItemsPerRow);
}

void StatisticsShopCategory::processEvents(const sf::Event event)
{
	for (auto& item : mItems)
	{
		if (item.upgradeButton.isPressed(event))
		{
			item.upgradeButton.invoke();
			return;
		}
	}
}

void StatisticsShopCategory::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	const ShopItem* interactedItem = nullptr;

	for (auto& item : mItems)
	{
		item.upgradeButton.update(mousePosition);
		item.upgradeButton.setActiveState(canBuy(item));
		if (item.interactionBounds.contains(mousePosition))
		{
			interactedItem = &item;
		}
	}

	if (interactedItem && !mItemDescription)
	{
		onItemHover(*interactedItem);
	}
	else if (!interactedItem && mItemDescription)
	{
		onItemUnhover();
	}
}

void StatisticsShopCategory::render()
{
	for (const auto& item : mItems)
		ShopUtils::renderItem(mGameContext.window, item);
	renderItemDescription();

	for (const auto& statText : mStatDisplayTexts)
		mGameContext.window.draw(statText);
}

void StatisticsShopCategory::onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	mCategoryWidth = categorySize.x;
	createItems();
	positionItems(pos, categorySize);
	createInteractionBounds();

	const auto& first = mItems[0];
	mDescriptionPos.x = pos.x;
	mDescriptionPos.y = first.interactionBounds.position.y + first.interactionBounds.size.y + mCharSize;
}

void StatisticsShopCategory::createItems()	
{
	constexpr TextureIdentifier tmpId{ TextureIdentifier::Devoyer }; //placeholder
	const std::array<ItemInitData, 5> itemsData =
	{{
		{.iconId = tmpId, .name = "Health Points",    .type = StatType::Health},
		{.iconId = tmpId, .name = "Attack Damage",    .type = StatType::Attack},
		{.iconId = tmpId, .name = "Attack Speed",	  .type = StatType::AttackSpeed},
		{.iconId = tmpId, .name = "Physical Defence", .type = StatType::Defence},
		{.iconId = tmpId, .name = "Magic Defence",	  .type = StatType::MagicDefence}
	}};

	for (const auto& data : itemsData)
	{
		tryMakeStatisticUpgradeData(data.name);
		mItems.emplace_back(ShopUtils::createItem(data, mGameContext.textures, mGameContext.fonts));
	}

	//add upgrade functionality
	for (auto& item : mItems)
	{
		item.upgradeButton.setCallback([this, &item]() {
				tryBuy(item);
			});
	}
}

void StatisticsShopCategory::positionItems(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
{
	float itemWidth = calculateItemWidth();
	float space = (categorySize.x - (itemWidth * mItemsPerRow)) / (mItemsPerRow + 1);
	sf::Vector2f itemPos{ pos };

	itemPos.x += space;
	for (auto& item : mItems)
	{
		ShopUtils::positionItem(item, itemPos);
		updateItemPrice(item);
		itemPos.x += itemWidth + space;
	}
}

void StatisticsShopCategory::createInteractionBounds()
{
	float height = calculateItemHeight();

	auto updateLR = [](const sf::FloatRect& rect, float& left, float& right)
	{
		left = std::min(left, rect.position.x);
		right = std::max(right, rect.position.x + rect.size.x);
	};

	for (auto& item : mItems)
	{
		float left = 100000.f;
		float right = -100000.f;
		updateLR(item.itemVisual.getGlobalBounds(), left, right);
		updateLR(item.itemCostText.getGlobalBounds(), left, right);
		updateLR(item.itemNameText.getGlobalBounds(), left, right);

		const float width = right - left;
		const sf::Vector2f pos{ left, item.itemVisual.getPosition().y };
		const sf::Vector2f size{ width, height };

		std::cout << "Bounds pos: (" << pos.x << "," << pos.y << ")\n";
		std::cout << "Bounds size:(" << size.x << "," << size.y << ")\n";
		item.interactionBounds = sf::FloatRect(pos, size);
	}
}

void StatisticsShopCategory::tryMakeStatisticUpgradeData(const std::string& name)
{
	//This function will create upgrade data on the first entrance.
	//This is best place to put this in, try_emplace will not override anyway.
	auto& statsComp = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgrades = statsComp.cUpgradesMap;

	upgrades.try_emplace(name, 1);
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

float StatisticsShopCategory::calculateItemWidth() const
{
	if(mItems.empty())
		return 0.0f;
	
	auto& first = mItems[0];
	constexpr float margin = 10.f;
	float max = 0.f;

	auto updateBounds = [&](const sf::FloatRect& bounds)
	{
		max = std::max(max, bounds.size.x);
	};

	updateBounds(first.itemNameText.getGlobalBounds());
	updateBounds(first.itemCostText.getGlobalBounds());
	updateBounds(first.itemVisual.getGlobalBounds());

	float res = max + margin;
	return res;
}

float StatisticsShopCategory::calculateItemHeight() const
{
	if (mItems.empty())
		return 0.f;

	constexpr float margin = 5.f;
	auto& first = mItems[0];
	auto sprPos = first.itemVisual.getPosition();
	auto btnPos = first.upgradeButton.getPosition();
	auto btnHeight = first.upgradeButton.getSize().y;

	return ((btnPos.y + btnHeight) - sprPos.y) + margin;
}

std::string StatisticsShopCategory::getStatDescription(StatType type) const
{
	switch (type)
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

void StatisticsShopCategory::createCurrentStatText(const ShopItem& item)
{
	if (!mItemDescription)
		return;

	constexpr sf::Color currStrColor{150, 200, 255};
	constexpr sf::Color currValueColor{255, 255, 255};
	constexpr sf::Color arrowColor{ 136, 204, 255 };
	constexpr sf::Color newValueColor{0, 204, 102};

	const auto& combStats = player.getComponent<CombatStatsComponent>();
	std::string currStr = "Current " + item.itemName + ":";
	std::string currValStr = getStatValueStr(item.statType, combStats);
	std::string arrowStr = "->";

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

	std::string nextValStr = getStatValueStr(item.statType, tmpCombStats);
	const sf::Font& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	auto createText = [this, &font](const sf::Color& color, const std::string& str, const sf::Vector2f& pos)
	{
		mStatDisplayTexts.emplace_back(font, str, mDescCharSize);
		mStatDisplayTexts.back().setPosition(pos);
		mStatDisplayTexts.back().setFillColor(color);
	};

	const float wordsMargin = mDescCharSize * 0.3f;
	auto getNextPosX = [wordsMargin](const sf::Text& text)
	{
		return text.getPosition().x + text.getGlobalBounds().size.x + wordsMargin;
	};

	const auto& descText = mItemDescription.value();
	float posY = descText.getPosition().y + descText.getLocalBounds().size.y + ShopUtils::getItemDescSize() * 2;
	sf::Vector2f pos{ mDescriptionPos.x, posY };

	createText(currStrColor, currStr, pos);
	pos.x = getNextPosX(mStatDisplayTexts.back());
	createText(currValueColor, currValStr, pos);
	pos.x = getNextPosX(mStatDisplayTexts.back());
	createText(arrowColor, arrowStr, pos);
	pos.x = getNextPosX(mStatDisplayTexts.back());
	createText(newValueColor, nextValStr, pos);
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

	//set currency sprite so it preceedes itemCostText
	const sf::Vector2f costPos{ item.itemCostText.getPosition() };
	const sf::Vector2f currSize{ item.currencySprite.getGlobalBounds().size };
	const sf::Vector2f currPos{ costPos.x - currSize.x - mCharSize, costPos.y - 3 };

	item.currencySprite.setPosition(currPos);
}

void StatisticsShopCategory::onItemHover(const ShopItem& item)
{
	createItemDescription(mDescriptionPos, getStatDescription(item.statType));
	createCurrentStatText(item);
}

void StatisticsShopCategory::onItemUnhover()
{
	removeItemDescription();
	mStatDisplayTexts.clear();
}
