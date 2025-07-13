#pragma once
#include "UIComponent.h"
#include "ShopData.h"

class IShopCategory : public UIComponent
{
public:
	IShopCategory(GameContext& gameContext, Entity& player);
	virtual ~IShopCategory() = default;

	// Inherited via UIComponent
	virtual void processEvents(const sf::Event event) override;
	virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
	virtual void render() override;

	void create(const sf::Vector2f& pos, const sf::Vector2f& categorySize);
protected:
	virtual void onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize) = 0;
	virtual void updateItemPrice(ShopItem& item) = 0;
	virtual void upgrade(ShopItem& item) = 0;
	virtual std::string getItemDescriptionStr(const ShopItem& item) const = 0;
	virtual void createStatUpgradeTexts(const ShopItem& item) = 0;
	virtual void createAssignableOptions();

	int getUpgradeLevel(const std::string& upgradeName) const;
	bool canBuy(const ShopItem& item) const;
	void upgradeStatisticLevel(const ShopItem& item);
	void notifyUIAfterBuy();
	void tryBuy(ShopItem& item);

	void createItemDescription(const std::string& descText);
	void removeItemDescription();
	void renderItemDescription();
	void setDescriptionPos(const sf::Vector2f& categoryPos);

	void createItems(std::span<const ItemInitData> itemsData);
	void generateItems(std::span<const ItemInitData> itemsData);
	void positionItems(int itemsPerRow);
	void createItemsBounds();
	void createOnUpgradeFunctionality();
	void setItemCurrencySprite(ShopItem& item);
	void renderItems();

	void tryMakeStatisticUpgradeData(const std::string& name);
	void updateItemsAndDescription(const sf::Vector2f& mousePos);

	void onItemHover(const ShopItem& item);
	void onItemUnhover();
	
	void renderStatsUpgradeText();
	void makeUpgradeText(const sf::Vector2f& pos, const std::string& upgradeName,
		const std::string& currValue, const std::string& nextValue);

	sf::Vector2f getNextUpgradeTextPos() const;
	void processUpgradeButtonEvents(const sf::Event event);
	bool isUpgradeLevelReached(const ShopItem& item) const;
	void checkIfUpgradeLevelReached(ShopItem& item);

	//sets .isAssignable attribute to true for all existing items.
	void makeItemsAssignable();
	void initAssignables();
	void createAssignablePopup();
	void layoutAssignPopup(const ShopItem& item);
	void handleAssignPopupClick(const sf::Event event);
	bool handleAssignButtonClick(const sf::Event event);
	void updateAssignPopupButtons(const sf::Vector2f& mousePos);
	void renderAssignablePopup();

protected:
	size_t mItemsPerRow;
	const unsigned int mDescCharSize;
	const unsigned int mCharSize;
	sf::Vector2f mIconSize;
	sf::Vector2f mCategoryPos;
	sf::Vector2f mCategorySize;
	std::optional<sf::Text> mItemDescription;
	std::vector<ShopItem> mItems;
	sf::Vector2f mDescriptionPos;
	std::vector<sf::Text> mStatUpgradeTexts;
	std::vector<std::pair<sf::Keyboard::Key, std::string>> mAssignableOptions; //neither map nor unordered_map keep the order i need.

	sf::Text mAssignToText; //for assignable spells.
	std::vector<TextButton> mAssignableButtons; //action slots that u can assign shop item to.
	sf::RectangleShape mAssignableBackground;
	bool mIsAssignableActive; //only if true we update and render assignable stuff.
};

