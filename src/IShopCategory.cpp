#include "pch.h"
#include "IShopCategory.h"
#include "Config.h"
#include "Entity.h"
#include "Utilities.h"

IShopCategory::IShopCategory(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mItemsPerRow(0),
	mDescCharSize(ShopUtils::getItemDescSize()),
	mCharSize(ShopUtils::getItemCharSize()),
	mAssignToText(gameContext.fonts.get(FontIdentifiers::UIFont), "Bind To Key:", mDescCharSize),
	mIsAssignableActive(false),
	mLastPressedItem(nullptr)
{
	float iconSize = Config::fWindowSize.x * 0.035f;
	mIconSize = { iconSize, iconSize };
	mAssignableButtons.reserve(mAssignableOptions.size());
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

void IShopCategory::createAssignableOptions()
{
	//to override, but not by all systems.
}

void IShopCategory::assignToSpellbook(const ShopItem& item)
{
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
	bool enoughGold = player.getComponent<PlayerResourcesComponent>().cGold >= item.cost;
	return enoughGold && !isUpgradeLevelReached(item);
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
	onItemUnhover();
	checkIfUpgradeLevelReached(item);
	notifyUIAfterBuy();
	determineItemsBorderColor();
	assignToSpellbook(item); //virtual function, only SpellcraftShopCategory overrides it.
}

void IShopCategory::createItemDescription(const std::string& descText)
{
	const sf::Font& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	mItemDescription = ShopUtils::createItemDescription(font, descText, mDescriptionPos, mCategorySize.x);
}

void IShopCategory::removeItemDescription()
{
	mItemDescription = {};
}

void IShopCategory::renderItemDescription()
{
	if (mItemDescription)
		mGameContext.window.draw(mItemDescription.value());
}

void IShopCategory::setDescriptionPos(const sf::Vector2f& categoryPos)
{
	const auto& first = mItems[0];
	mDescriptionPos.x = categoryPos.x;
	mDescriptionPos.y = first.interactionBounds.position.y + first.interactionBounds.size.y + mDescCharSize * 2;
}

void IShopCategory::createItems(std::span<const ItemInitData> itemsData)
{
	generateItems(itemsData);
	positionItems(mItemsPerRow);
	createItemsBounds();
	createOnUpgradeFunctionality();
}

void IShopCategory::generateItems(std::span<const ItemInitData> itemsData)
{
	for (const auto& data : itemsData)
	{
		tryMakeStatisticUpgradeData(data.name);
		mItems.emplace_back(ShopUtils::createItem(data, mGameContext.textures, mGameContext.fonts));
		mItems.back().maxUpgradeLevel = data.maxUpgrade;
	}
}

void IShopCategory::positionItems(int itemsPerRow)
{
	if (mItems.empty())
		return;

	float itemWidth = ShopUtils::getItemWidth(mItems[0]);
	float space = (mCategorySize.x - (itemWidth * itemsPerRow)) / (itemsPerRow + 1);
	sf::Vector2f itemPos{ mCategoryPos };

	itemPos.x += space;
	for (auto& item : mItems)
	{
		ShopUtils::positionItem(item, itemPos);
		updateItemPrice(item);
		itemPos.x += itemWidth + space;
	}
}

void IShopCategory::createItemsBounds()
{
	if (mItems.empty())
		return;

	float height = ShopUtils::getItemHeight(mItems[0]);

	auto updateLR = [](const sf::FloatRect& rect, float& left, float& right)
	{
		left = std::min(left, rect.position.x);
		right = std::max(right, rect.position.x + rect.size.x);
	};

	for (auto& item : mItems)
	{
		float left = std::numeric_limits<float>::max();
		float right = std::numeric_limits<float>::min();

		updateLR(item.itemVisual.getGlobalBounds(), left, right);
		updateLR(item.itemCostText.getGlobalBounds(), left, right);
		updateLR(item.itemNameText.getGlobalBounds(), left, right);

		const float width = right - left;
		const sf::Vector2f pos{ left, item.itemVisual.getPosition().y };
		const sf::Vector2f size{ width, height };

		item.interactionBounds = sf::FloatRect(pos, size);
	}
}

void IShopCategory::createOnUpgradeFunctionality()
{
	for (auto& item : mItems)
	{
		item.upgradeButton.setCallback([this, &item]() 
			{
				tryBuy(item);
			});
	}
}

void IShopCategory::setItemCurrencySprite(ShopItem& item)
{
	if (isUpgradeLevelReached(item))
	{
		item.currencySprite.setPosition({ -1000.f, -1000.f });
		return;
	}
	constexpr float margin = 3.f;
	//set currency sprite so it preceedes itemCostText
	const sf::Vector2f costPos{ item.itemCostText.getPosition() };
	const sf::Vector2f currSize{ item.currencySprite.getGlobalBounds().size };
	const sf::Vector2f currPos{ costPos.x - currSize.x - mCharSize, costPos.y - margin };

	item.currencySprite.setPosition(currPos);
}

void IShopCategory::renderItems()
{
	for (const auto& item : mItems)
		ShopUtils::renderItem(mGameContext.window, item);
}

void IShopCategory::tryMakeStatisticUpgradeData(const std::string& name)
{
	//This function will create upgrade data on the first entrance.
	//This is best place to put this in, try_emplace will not override anyway.
	auto& statsComp = player.getComponent<StatisticsUpgradeComponent>();
	auto& upgrades = statsComp.cUpgradesMap;

	upgrades.try_emplace(name, 1);
}

void IShopCategory::updateItemsAndDescription(const sf::Vector2f& mousePos)
{
	const ShopItem* hoveredItem = nullptr;
	for (auto& item : mItems)
	{
		item.upgradeButton.update(mousePos);
		item.upgradeButton.setActiveState(canBuy(item));
		if (item.interactionBounds.contains(mousePos))
		{
			hoveredItem = &item;
			break;
		}
	}

	if (hoveredItem && !mItemDescription)
	{
		onItemHover(*hoveredItem);
	}
	else if (!hoveredItem && mItemDescription)
	{
		onItemUnhover();
	}
}

void IShopCategory::onItemHover(const ShopItem& item)
{
	createItemDescription(getItemDescriptionStr(item));
	createStatUpgradeTexts(item);
}

void IShopCategory::onItemUnhover()
{
	removeItemDescription();
	mStatUpgradeTexts.clear();
}

void IShopCategory::renderStatsUpgradeText()
{
	for (const auto& text : mStatUpgradeTexts)
		mGameContext.window.draw(text);
}

void IShopCategory::makeUpgradeText(const sf::Vector2f& pos, const std::string& upgradeName, const std::string& currValue, const std::string& nextValue)
{
	if (!mItemDescription)
		return;

	constexpr sf::Color currStrColor{ 150, 200, 255 };
	constexpr sf::Color currValueColor{ 255, 255, 255 };
	constexpr sf::Color arrowColor{ 136, 204, 255 };
	constexpr sf::Color newValueColor{ 0, 204, 102 };

	const auto& combStats = player.getComponent<CombatStatsComponent>();
	std::string currStr = "Current " + upgradeName + ":";
	std::string arrowStr = "->";

	const sf::Font& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	auto createText = [this, &font](const sf::Color& color, const std::string& str, const sf::Vector2f& pos)
	{
		mStatUpgradeTexts.emplace_back(font, str, mDescCharSize);
		mStatUpgradeTexts.back().setPosition(pos);
		mStatUpgradeTexts.back().setFillColor(color);
	};

	const float wordsMargin = mDescCharSize * 0.3f;
	auto getNextPosX = [wordsMargin](const sf::Text& text)
	{
		return text.getPosition().x + text.getGlobalBounds().size.x + wordsMargin;
	};

	const auto& descText = mItemDescription.value();
	float posY = descText.getPosition().y + descText.getLocalBounds().size.y + ShopUtils::getItemDescSize() * 2;

	sf::Vector2f textPos{ pos };
	createText(currStrColor, currStr, textPos);

	textPos.x = getNextPosX(mStatUpgradeTexts.back());
	createText(currValueColor, currValue, textPos);

	textPos.x = getNextPosX(mStatUpgradeTexts.back());
	createText(arrowColor, arrowStr, textPos);

	textPos.x = getNextPosX(mStatUpgradeTexts.back());
	createText(newValueColor, nextValue, textPos);
}

sf::Vector2f IShopCategory::getNextUpgradeTextPos() const
{
	if (!mItemDescription)
		return {};
	float posX = 0.f;
	float posY = 0.f;
	if (mStatUpgradeTexts.empty())
	{
		const auto& descText = mItemDescription.value();
		posX = mDescriptionPos.x;
		posY = descText.getPosition().y + descText.getLocalBounds().size.y + ShopUtils::getItemDescSize() * 2;
	}
	else
	{
		const auto& last = mStatUpgradeTexts.back();
		posX = mDescriptionPos.x;
		posY = last.getPosition().y + last.getLocalBounds().size.y + ShopUtils::getItemDescSize();
	}

	return { posX, posY };
}

void IShopCategory::processUpgradeButtonEvents(const sf::Event event)
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

bool IShopCategory::isUpgradeLevelReached(const ShopItem& item) const
{
	if (!item.maxUpgradeLevel)
		return false;
	
	auto currUpgradeLevel = getUpgradeLevel(item.itemName);
	return currUpgradeLevel >= item.maxUpgradeLevel.value();
}

void IShopCategory::checkIfUpgradeLevelReached(ShopItem& item)
{
	if (isUpgradeLevelReached(item))
	{
		item.itemCostText.setString("MAXED");
		item.itemCostText.setFillColor({ 255, 100, 100 });
		item.itemCostText.setStyle(sf::Text::Bold);
		Utilities::setTextOriginOnCenter(item.itemCostText);
	}
}

void IShopCategory::makeItemsAssignable()
{
	for (auto& item : mItems)
		item.isAssignable = true;
}

void IShopCategory::initAssignables()
{
	createAssignableOptions();
	createAssignablePopup();
}

void IShopCategory::createAssignablePopup()
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	mAssignableBackground.setFillColor({ 32, 32, 40 });
	mAssignToText.setFillColor({ 200, 200, 220 });
	const sf::Vector2f backgroundSize{
		Config::fWindowSize.x * 0.12f,
		Config::fWindowSize.y * 0.08f
	};
	mAssignableBackground.setSize(backgroundSize);

	const sf::Vector2f textSize = mAssignToText.getGlobalBounds().size;
	mAssignToText.setOrigin(textSize * 0.5f);

	for (const auto& [key, val] : mAssignableOptions)
	{
		mAssignableButtons.emplace_back(font, "[" + val + "]", mDescCharSize);
		mAssignableButtons.back().setDefaultColor({ 100, 180, 255 });
	}
	setAssignableButtonsCallbacks();
}

bool IShopCategory::canAssign(const ShopItem& item)
{
	if (!item.isAssignable)
		return false;
	if (!item.minLevelToAssign)
		return true;

	auto upgradeLevel = getUpgradeLevel(item.itemName);
	return upgradeLevel >= item.minLevelToAssign.value();
}

void IShopCategory::layoutAssignPopup(const ShopItem& item)
{
	constexpr float margin = 5.f;
	auto itemPos = item.itemVisual.getPosition();
	auto backgroundSize = mAssignableBackground.getSize();
	const sf::Vector2f backgroundPos = { itemPos.x, itemPos.y - backgroundSize.y - 1.f };
	mAssignableBackground.setPosition(backgroundPos);

	sf::Vector2f assignTextPos{
		backgroundPos.x + backgroundSize.x / 2,
		backgroundPos.y + margin
	};

	mAssignToText.setPosition(assignTextPos);
	const float btnWidth = mAssignableButtons[0].getSize().x;
	const size_t btnCount = mAssignableButtons.size();

	float space = (backgroundSize.x - (btnWidth * btnCount)) / (btnCount + 1);
	sf::Vector2f btnPos{ backgroundPos.x, assignTextPos.y + mDescCharSize + margin };

	btnPos.x += space;
	for (auto& button : mAssignableButtons)
	{
		button.setPosition(btnPos);
		btnPos.x += mAssignableButtons.back().getSize().x + space;
	}
}

void IShopCategory::handleAssignPopupClick(const sf::Event event)
{
	if (auto data = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (data->button != sf::Mouse::Button::Left)
			return;

		const sf::Vector2f pos{
			static_cast<float>(data->position.x),
			static_cast<float>(data->position.y)
		};

		for (const auto& item : mItems)
		{
			if (!canAssign(item))
				continue;

			auto bounds = item.itemVisual.getGlobalBounds();
			if (bounds.contains(pos))
			{
				mIsAssignableActive = !mIsAssignableActive;
				if (mIsAssignableActive)
					layoutAssignPopup(item);
				mLastPressedItem = (mIsAssignableActive) ? &item : nullptr;
				return;
			}
		}
	}
}

bool IShopCategory::handleAssignButtonClick(const sf::Event event)
{
	if (!mIsAssignableActive)
		return false;
	for (auto& btn : mAssignableButtons)
	{
		if (btn.isPressed(event))
		{
			btn.invoke();
			return true;
		}
	}
	return false;
}

void IShopCategory::updateAssignPopupButtons(const sf::Vector2f& mousePos)
{
	if (!mIsAssignableActive)
		return;

	for (auto& btn : mAssignableButtons)
	{
		btn.update(mousePos);
	}
}

void IShopCategory::renderAssignablePopup()
{
	if (!mIsAssignableActive)
		return;

	mGameContext.window.draw(mAssignableBackground);
	mGameContext.window.draw(mAssignToText);

	for (const auto& btn : mAssignableButtons)
		btn.render(mGameContext.window);
}

void IShopCategory::setAssignableButtonsCallbacks()
{
}

void IShopCategory::determineItemsBorderColor()
{
	constexpr sf::Color assignable{ 60, 200, 120 };
	constexpr sf::Color notAssignable{ 90, 90, 90 };

	for (auto& item : mItems)
	{
		bool isAss = canAssign(item);
		item.border.setOutlineColor(isAss ? assignable : notAssignable);
	}
}
