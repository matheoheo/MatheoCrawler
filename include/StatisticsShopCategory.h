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
	int getStatTypeCost(StatType type) const;
	int calculateItemCost(StatType type, int level) const;
	int calculateItemCost(const ShopItem& item) const;
	
	float getIncreasePerLvl(StatType type) const;
	std::string getStatValueStr(StatType type, const CombatStatsComponent& stats) const;

	virtual void upgrade(ShopItem& item) override;
	virtual void updateItemPrice(ShopItem& item) override;
	virtual std::string getItemDescriptionStr(const ShopItem& item) const override;
	virtual void createStatUpgradeTexts(const ShopItem& item) override;
private:
};

