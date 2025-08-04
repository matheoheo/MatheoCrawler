#pragma once
#include "IShopCategory.h"
#include "ISpellUpgradeStrategy.h"
class SpellcraftShopCategory :
    public IShopCategory
{
public:
	SpellcraftShopCategory(GameContext& gameContext, Entity& player);

	// Inherited via UIComponent
	virtual void processEvents(const sf::Event event) override;
	virtual void update(const sf::Vector2f & mousePosition, const sf::Time & deltaTime) override;
	virtual void render() override;

protected:
    // Inherited via IShopCategory
    virtual void onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize) override;
	virtual void updateItemPrice(ShopItem& item) override;
	virtual void upgrade(ShopItem& item) override;
	virtual std::string getItemDescriptionStr(const ShopItem& item) const override;
	virtual void createStatUpgradeTexts(const ShopItem& item);
	virtual void createAssignableOptions();
	virtual void setAssignableButtonsCallbacks();

	void createStatTypeToSpellIdMap();
	void createSpellUpgrades();
	void setUpgradeLevelsLimit();
	void setMinLevelToAssign();
	void notifySpellBindEvent(const TextButton& button);
private:
	struct SpellShopMapping {
		SpellIdentifier spellId;
		StatType statType;
	};
	struct SpellcraftConfig
	{
		static constexpr size_t restorationId = 0;
		static constexpr size_t projectilesId = 1;
		static constexpr size_t aoeId = 2;
	};

	//creates buttons to differentiate between different classes of spells - Healing/Attacking, etc.
	void createSpellsCategories();
	void onSpellCategoryPress(size_t id);
	std::vector<ItemInitData> getRestorationItemsInitData() const;
	std::vector<ItemInitData> getProjectilesItemsInitData() const;
	std::vector<ItemInitData> getAreaOfEffectItemsInitData() const;
	std::vector<ItemInitData> createItemInitData(std::span<const SpellShopMapping> spellsData) const;

	int calculateSpellCost(const ShopItem& item) const;
	int getBasicItemCost(StatType type) const;
	float getStatIncrease(StatType type) const;
private:
	std::vector<TextButton> mSpellCategories;
	std::unordered_map<StatType, SpellIdentifier> mStatTypeToSpellIdMap;
	std::unordered_map<StatType, std::unique_ptr<ISpellUpgradeStrategy>> mSpellUpgrades;
	static constexpr size_t mSpellTypesCount = static_cast<size_t>(SpellType::Count);
	
};
