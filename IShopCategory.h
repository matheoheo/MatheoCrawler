#pragma once
#include "UIComponent.h"
struct ShopItem;

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

	int getUpgradeLevel(const std::string& upgradeName) const;
	bool canBuy(const ShopItem& item) const;
	void upgradeStatisticLevel(const ShopItem& item);
	void notifyUIAfterBuy();
	void tryBuy(ShopItem& item);

	void createItemDescription(const sf::Vector2f& pos, const std::string& descText);
	void removeItemDescription();
	void renderItemDescription();
protected:
	sf::Vector2f mIconSize;
	sf::Vector2f mCategoryPos;
	sf::Vector2f mCategorySize;
	std::optional<sf::Text> mItemDescription;
};

