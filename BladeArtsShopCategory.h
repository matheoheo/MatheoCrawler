#pragma once
#include "IShopCategory.h"
class BladeArtsShopCategory :
    public IShopCategory
{
public:
    BladeArtsShopCategory(GameContext& gameContext, Entity& player);

    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
protected:
    // Inherited via IShopCategory
    virtual void onCreate(const sf::Vector2f& pos, const sf::Vector2f& categorySize) override;
    virtual void updateItemPrice(ShopItem& item) override;
    virtual void upgrade(ShopItem& item) override;
    virtual std::string getItemDescriptionStr(const ShopItem& item) const override;
    virtual void createStatUpgradeTexts(const ShopItem& item) override;
    virtual void createAssignableOptions() override;

    int getBaseItemPrice(const ShopItem& item) const;
    int calculateItemCost(const ShopItem& item) const;
    float getStatIncrease(StatType type) const;

    AttackData* getAttackDataForItem(const ShopItem& item) const;
private:

};

