#pragma once
#include "IShopCategory.h"
#include "TextButton.h"
#include "ShopData.h"

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

protected:
	// Inherited via IShopCategory
	virtual void onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize) override;
private:
	void createItems();
	void positionItems(const sf::Vector2f& pos, const sf::Vector2f& categorySize);
	void createInteractionBounds();
	void tryMakeStatisticUpgradeData(const std::string& name);

	int getStatTypeCost(StatType type) const;
	int calculateItemCost(StatType type, int level) const;
	int calculateItemCost(const ShopItem& item) const;
	
	float calculateItemWidth() const;
	float calculateItemHeight() const;
	std::string getStatDescription(StatType type) const;
	void createCurrentStatText(const ShopItem& item);
	float getIncreasePerLvl(StatType type) const;
	std::string getStatValueStr(StatType type, const CombatStatsComponent& stats) const;
	virtual void upgrade(ShopItem& item) override;
	virtual void updateItemPrice(ShopItem& item) override;

	void onItemHover(const ShopItem& item);
	void onItemUnhover();
private:
	std::vector<ShopItem> mItems;
	const unsigned int mCharSize;
	const int mItemsPerRow;
	float mCategoryWidth;
	sf::Vector2f mDescriptionPos;
	std::vector<sf::Text> mStatDisplayTexts;
	const unsigned int mDescCharSize;
	const sf::Color mDescColor;
};

