#include "pch.h"
#include "StatisticsShopCategory.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"
#include "TextButton.h"

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
		bool buyable = canUpgrade(item);
		item.upgradeButton.setActiveState(buyable);

		if (item.interactionBounds.contains(mousePosition))
		{
			interactedItem = &item;
		}
	}

	if (interactedItem && mDescriptionLines.empty())
	{
		createDescription(*interactedItem);
	}
	else if (!interactedItem && !mDescriptionLines.empty())
	{
		clearDescription();
	}
}

void StatisticsShopCategory::render()
{
	for (const auto& item : mItems)
		renderItem(item);
	renderDescription();
}

void StatisticsShopCategory::create(const sf::Vector2f& pos, const sf::Vector2f& categorySize)
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
		mItems.emplace_back(createItem(data));
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
		positionItem(item, itemPos);
		updateItemPriceVisual(item);
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

int StatisticsShopCategory::getUpgradeLevel(const std::string& upgradeName) const
{
	auto& statsComp = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgrades = statsComp.cUpgradesMap;

	if (auto it = upgrades.find(upgradeName); it != std::end(upgrades))
		return it->second;

	return 0;
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

StatisticsShopCategory::ShopItem StatisticsShopCategory::createItem(const ItemInitData& itemInit)
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	const auto& iconTexture = mGameContext.textures.get(itemInit.iconId);
	const auto& currencyTexture = mGameContext.textures.get(TextureIdentifier::Suash);
	ShopItem item(font, iconTexture, currencyTexture, itemInit.type);
	item.itemName = itemInit.name;
	Utilities::scaleSprite(item.itemVisual, mIconSize);

	item.itemNameText.setCharacterSize(mCharSize);
	item.itemNameText.setString(item.itemName);
	item.itemNameText.setFillColor(sf::Color{ 230, 230, 230 });

	Utilities::scaleSprite(item.currencySprite, sf::Vector2f(mCharSize, mCharSize));
	item.itemCostText.setCharacterSize(mCharSize);
	item.itemCostText.setString("57893"); //placeholder
	item.itemCostText.setFillColor(sf::Color{ 255, 223, 50 });

	item.upgradeButton.setCharacterSize(mCharSize);
	item.upgradeButton.setOriginOnCenter();

	return item;
}

void StatisticsShopCategory::positionItem(ShopItem& item, const sf::Vector2f& pos)
{
	constexpr float margin = 5.f;
	item.itemVisual.setPosition(pos);

	const float sprCenter{ pos.x + item.itemVisual.getGlobalBounds().size.x / 2 };
	
	const sf::Vector2f itemNamePos{ sprCenter, pos.y + mIconSize.y + margin };
	item.itemNameText.setPosition(itemNamePos);
	Utilities::setTextOriginOnCenter(item.itemNameText);

	const sf::Vector2f itemCostPos{ sprCenter, itemNamePos.y + mCharSize + margin };
	item.itemCostText.setPosition(itemCostPos);
	Utilities::setTextOriginOnCenter(item.itemCostText);

	const sf::Vector2f buttonPos{ sprCenter, itemCostPos.y + mCharSize + margin };
	item.upgradeButton.setPosition(buttonPos);

}

void StatisticsShopCategory::renderItem(const ShopItem& item)
{
	auto& window = mGameContext.window;
	window.draw(item.itemVisual);
	window.draw(item.itemNameText);
	window.draw(item.itemCostText);
	window.draw(item.currencySprite);
	item.upgradeButton.render(window);
}

void StatisticsShopCategory::updateItemPriceVisual(ShopItem& item)
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
		return "Boosts the damage you deal with each attack, enabling you to defeat enemies more quickly and efficiently.";
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

void StatisticsShopCategory::createDescription(const ShopItem& item)
{
	mDescriptionLines.clear();

	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	auto desc = getStatDescription(item.statType);

	auto splitDesc = std::views::split(desc, ' ');
	auto start = std::ranges::begin(splitDesc);
	auto end = std::ranges::end(splitDesc);
	
	for (auto it = start; it != end; ++it)
	{
		std::string_view strView{ std::ranges::begin(*it), std::ranges::end(*it) };
		mDescriptionLines.emplace_back(font, std::string(strView), mDescCharSize);
		mDescriptionLines.back().setFillColor(mDescColor);
	}

	positionDescriptionLines();
	createCurrentStatText(item);
}

void StatisticsShopCategory::positionDescriptionLines()
{
	if (mDescriptionLines.empty())
		return;

	const float wordsMargin = mDescCharSize * 0.3f;
	auto getNextPosX = [wordsMargin](const sf::Text& text)
	{
		return text.getPosition().x + text.getGlobalBounds().size.x + wordsMargin;
	};
	sf::Vector2f pos{ mDescriptionPos };
	mDescriptionLines[0].setPosition(pos);
	const float xMax = mDescriptionPos.x + mCategoryWidth;

	for (size_t i = 1; i < mDescriptionLines.size(); ++i)
	{
		pos.x = getNextPosX(mDescriptionLines[i - 1]);
		float thisWidth = mDescriptionLines[i].getGlobalBounds().size.x;
		if (pos.x + thisWidth > xMax)
		{
			pos.x = mDescriptionPos.x;
			pos.y += mDescCharSize + wordsMargin;
		}
		mDescriptionLines[i].setPosition(pos);
	}
}

void StatisticsShopCategory::createCurrentStatText(const ShopItem& item)
{
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
		mDescriptionLines.emplace_back(font, str, mDescCharSize);
		mDescriptionLines.back().setPosition(pos);
		mDescriptionLines.back().setFillColor(color);
	};

	const float wordsMargin = mDescCharSize * 0.3f;
	auto getNextPosX = [wordsMargin](const sf::Text& text)
	{
		return text.getPosition().x + text.getGlobalBounds().size.x + wordsMargin;
	};
	sf::Vector2f pos{ mDescriptionPos.x, mDescriptionLines.back().getPosition().y + mDescCharSize*2 + wordsMargin };
	createText(currStrColor, currStr, pos);
	pos.x = getNextPosX(mDescriptionLines.back());
	createText(currValueColor, currValStr, pos);
	pos.x = getNextPosX(mDescriptionLines.back());
	createText(arrowColor, arrowStr, pos);
	pos.x = getNextPosX(mDescriptionLines.back());
	createText(newValueColor, nextValStr, pos);
}

void StatisticsShopCategory::renderDescription()
{
	for (const auto& desc : mDescriptionLines)
		mGameContext.window.draw(desc);
}

void StatisticsShopCategory::clearDescription()
{
	mDescriptionLines.clear();
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

bool StatisticsShopCategory::canUpgrade(const ShopItem& item) const
{
	const auto& goldComp = player.getComponent<PlayerResourcesComponent>();
	return goldComp.cGold >= item.cost;
}

void StatisticsShopCategory::upgradeStat(const ShopItem& item)
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

	auto& upgradeStats = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgMap = upgradeStats.cUpgradesMap;

	if (upgMap.contains(item.itemName))
		upgMap.at(item.itemName)++;

	mGameContext.eventManager.notify<UpdatePlayerStatusEvent>(UpdatePlayerStatusEvent());
	mGameContext.eventManager.notify<UpdatePlayerResourcesEvent>(UpdatePlayerResourcesEvent());

}

bool StatisticsShopCategory::tryBuy(ShopItem& item)
{
	if(!canUpgrade(item))
		return false;

	auto& goldComp = player.getComponent<PlayerResourcesComponent>();
	goldComp.cGold -= item.cost;
	upgradeStat(item);
	updateItemPriceVisual(item);
	clearDescription();
}
