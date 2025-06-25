#pragma once
#include "IShopCategory.h"
#include "TextButton.h"

struct CombatStatsComponent;
class StatisticsShopCategory :
    public IShopCategory
{
public:
	StatisticsShopCategory(GameContext& gameContext, Entity& player);

	// Inherited via UIComponent
	virtual void processEvents(const sf::Event event) override;
	virtual void update(const sf::Vector2f & mousePosition, const sf::Time & deltaTime) override;
	virtual void render() override;

	virtual void create(const sf::Vector2f& pos, const sf::Vector2f& categorySize) override;
private:
	enum class StatType {
		Health,
		Defence,
		MagicDefence,
		Attack,
		AttackSpeed
	};
	struct ShopItem {
			int cost = 1;
			std::string itemName;
			StatType statType;
			sf::Sprite itemVisual;
			sf::Sprite currencySprite;
			sf::Text   itemNameText;
			sf::Text   itemCostText;
			TextButton upgradeButton;
			sf::FloatRect interactionBounds;

			ShopItem(const sf::Font& font, const sf::Texture& itemTexture, const sf::Texture& currencyTexture, StatType type)
				:itemVisual(itemTexture),
				statType(type),
				currencySprite(currencyTexture),
				itemNameText(font),
				itemCostText(font),
				upgradeButton(font, "Upgrade")
			{}
		};
	struct ItemInitData {
			TextureIdentifier iconId;
			std::string name;
			StatType type;
		};


private:
	void createItems();
	void positionItems(const sf::Vector2f& pos, const sf::Vector2f& categorySize);
	void createInteractionBounds();
	void tryMakeStatisticUpgradeData(const std::string& name);

	int getUpgradeLevel(const std::string& upgradeName) const;
	int getStatTypeCost(StatType type) const;
	int calculateItemCost(StatType type, int level) const;
	int calculateItemCost(const ShopItem& item) const;

	ShopItem createItem(const ItemInitData& itemInit);
	void positionItem(ShopItem& item, const sf::Vector2f& pos);
	void renderItem(const ShopItem& item);
	
	void updateItemPriceVisual(ShopItem& item);
	float calculateItemWidth() const;
	float calculateItemHeight() const;
	std::string getStatDescription(StatType type) const;

	void createDescription(const ShopItem& item);
	void positionDescriptionLines();
	void createCurrentStatText(const ShopItem& item);
	void renderDescription();
	void clearDescription();
	float getIncreasePerLvl(StatType type) const;

	std::string getStatValueStr(StatType type, const CombatStatsComponent& stats) const;

	bool canUpgrade(const ShopItem& item) const;
	void upgradeStat(const ShopItem& item);
	bool tryBuy(ShopItem& item);
private:
	std::vector<ShopItem> mItems;
	const unsigned int mCharSize;
	const int mItemsPerRow;
	float mCategoryWidth;
	sf::Vector2f mDescriptionPos;
	std::vector<sf::Text> mDescriptionLines;
	const unsigned int mDescCharSize;
	const sf::Color mDescColor;
};

