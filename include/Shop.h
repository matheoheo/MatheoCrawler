#pragma once
#include "UIComponent.h"
#include "IShopCategory.h"
#include "TextButton.h"

class Shop :
    public UIComponent
{
public:
    Shop(GameContext& gameContext, Entity& player);

    // Inherited via UIComponent
    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void createShopOverlay();
    void createCategories(const sf::Vector2f& pos);
    void createCategoriesCallbacks();
    void positionCategories(const sf::Vector2f& pos);
    void addSeparator(const sf::Vector2f& size, const sf::Vector2f& pos, const sf::Color& color = sf::Color(50, 50, 50, 200));
    void addCategory(const sf::Vector2f& pos, const std::string& label, const sf::Color& defaultColor);
    float getLastCategoryWidth() const;
    void updateCategories(const sf::Vector2f& fMousePos);
    void createPickedCategory();

    void createCategoryTexts();
    void updateCategoryNameText();
    void onCategoryPress(size_t i = 0);

    void createCloseShopButton();
private:
    const sf::Font& mFont;
    const sf::Vector2f mBackgroundSize;
    const unsigned int mCharSize;
    sf::RectangleShape mBackground;
    sf::Text mShopNameText;
    std::vector<sf::RectangleShape> mSeparators;
    std::vector<TextButton> mCategories;
    std::unique_ptr<IShopCategory> mActiveCategory;
    sf::Vector2f mActiveCategoryPos;
    sf::Vector2f mActiveCategorySize;
    sf::Text mCategoryLabelText; //active category label text
    sf::Text mCategoryNameText; //active category name text
    const TextButton* mPressedCategory;
    static constexpr size_t mCategoriesCount = 3;
};

