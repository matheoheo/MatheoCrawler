#pragma once
#include "IShopCategory.h"
#include "TextButton.h"

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
			sf::Text   itemNameText;
			sf::Text   itemCostText;
			TextButton upgradeButton;
			sf::FloatRect interactionBounds;

			ShopItem(const sf::Font& font, const sf::Texture& texture, StatType type)
				:itemVisual(texture),
				statType(type),
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
	void createItems(sf::Vector2f pos);
	void tryMakeStatisticUpgradeData(const std::string& name);

	int getUpgradeLevel(const std::string& upgradeName) const;
	int getStatTypeCost(StatType type) const;
	int getItemCost(StatType type, int level) const;
	int getItemCost(const ShopItem& item) const;

	ShopItem createItem(const ItemInitData& itemInit, const sf::Vector2f& pos, unsigned int charSize);

	void renderItem(const ShopItem& item);
private:
	std::vector<ShopItem> mItems;
	sf::Vector2f mItemBoundsSize;
	const unsigned int mCharSize;
};

