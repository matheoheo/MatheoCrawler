#include "pch.h"
#include "Shop.h"
#include "StatisticsShopCategory.h"
#include "Config.h"
#include "Utilities.h"

Shop::Shop(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mFont(gameContext.fonts.get(FontIdentifiers::UIFont)),
	mBackgroundSize({ Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.65f }),
	mCharSize(Config::getCharacterSize()),
	mShopNameText(mFont),
	mCategoryLabelText(mFont),
	mCategoryNameText(mFont),
	mPressedCategory(nullptr)
{
	mCategories.reserve(mCategoriesCount + 1); // +1, because we are gonna also add button that closes shop.
	createShopOverlay();
	createCategoryTexts();
	createCloseShopButton();
}

void Shop::processEvents(const sf::Event event)
{
	for (auto& category : mCategories)
	{
		if (category.isPressed(event))
		{
			category.invoke();
			return;
		}
	}
	if (mActiveCategory)
		mActiveCategory->processEvents(event);
}

void Shop::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	updateCategories(mousePosition);

	if (mActiveCategory)
		mActiveCategory->update(mousePosition, deltaTime);
}

void Shop::render()
{
	mGameContext.window.draw(mBackground);
	mGameContext.window.draw(mShopNameText);

	for (const auto& separator : mSeparators)
		mGameContext.window.draw(separator);
	for (const auto& category : mCategories)
		category.render(mGameContext.window);
	
	mGameContext.window.draw(mCategoryLabelText);
	mGameContext.window.draw(mCategoryNameText);

	if(mActiveCategory)
		mActiveCategory->render();
}

void Shop::createShopOverlay()
{
	constexpr sf::Color backgroundColor{ 30, 30, 30 };
	constexpr sf::Color outlineColor{ 80, 80, 80 };
	constexpr sf::Color shopTextColor{ 230, 230, 230 };
	const float padding = mCharSize * 0.2f;

	const sf::Vector2f backgroundPos{
		Config::fWindowSize.x * 0.5f - mBackgroundSize.x * 0.5f,
		Config::fWindowSize.y * 0.5f - mBackgroundSize.y * 0.5f
	};

	mBackground.setSize(mBackgroundSize);
	mBackground.setFillColor(backgroundColor);
	mBackground.setOutlineColor(outlineColor);
	mBackground.setOutlineThickness(3.f);
	mBackground.setPosition(backgroundPos);

	const sf::Vector2f shopNamePos{mBackground.getPosition().x + mBackground.getGeometricCenter().x,
		mBackground.getPosition().y + padding};

	mShopNameText.setCharacterSize(mCharSize);
	mShopNameText.setFillColor(shopTextColor);
	mShopNameText.setString("Shop Of Fallens");
	Utilities::setTextOriginOnCenter(mShopNameText);
	mShopNameText.setPosition(shopNamePos);

	const sf::Vector2f separatorPos{ backgroundPos.x, shopNamePos.y + mCharSize };
	const sf::Vector2f separatorSize{ mBackgroundSize.x, 3.f };
	addSeparator(separatorSize, separatorPos);

	constexpr float margin = 20.f;
	const sf::Vector2f categoryPos{separatorPos.x, separatorPos.y + margin };
	createCategories(categoryPos);
}

void Shop::createCategories(const sf::Vector2f& pos)
{
	const std::array<std::string, mCategoriesCount> buttonLabels =
	{
		"[Attributes]",
		"[Blade Arts]",
		"[Spellcraft]"
	};

	constexpr std::array<sf::Color, mCategoriesCount> colors =
	{
		sf::Color{100, 220, 100},
		sf::Color{220, 100, 100},
		sf::Color{100, 150, 250}
	};

	for (size_t i = 0; i < mCategoriesCount; ++i)
		addCategory(pos, buttonLabels[i], colors[i]);

	createCategoriesCallbacks();
	positionCategories(pos);
}

void Shop::createCategoriesCallbacks()
{
	const std::array<TextButton::ButtonCallback, mCategoriesCount> callbacks =
	{
		[this]() {
			mActiveCategory = std::make_unique<StatisticsShopCategory>(mGameContext, player);
			onCategoryPress(0);
		},
		[this]() {

		},
		[this]() {

		}
	};

	for (size_t i = 0; i < mCategoriesCount; ++i)
	{
		mCategories[i].setCallback(callbacks[i]);
	}
}

void Shop::positionCategories(const sf::Vector2f& pos)
{
	//after we created categories (category = TextButton) we need to sum all their widths
	//by achieveing that, we can achieve even spacing.
	float sumWidth = 0.f;
	for (const auto& category : mCategories)
		sumWidth += category.getSize().x;

	float spacing = (mBackgroundSize.x - sumWidth) / (mCategoriesCount + 1);

	sf::Vector2f buttonPos{ pos };
	buttonPos.x += spacing;

	for (auto& category : mCategories)
	{
		category.setPosition(buttonPos);
		buttonPos.x += getLastCategoryWidth() + spacing;
	}
	const auto& first = mCategories[0];
	const auto& last = mCategories.back();
	float firstX = first.getPosition().x;
	float lastX = last.getPosition().x + last.getSize().x;

	constexpr float margin = 20.f;
	const sf::Vector2f separatorSize{ lastX - firstX, 3.f };
	const sf::Vector2f seperatorPos{ pos.x + spacing, pos.y +  first.getSize().y + margin };
	addSeparator(separatorSize, seperatorPos);

	const float yMargin = mBackgroundSize.y * 0.1f;
	const float xMargin = mBackgroundSize.x * 0.05f;
	mActiveCategorySize.x = mBackgroundSize.x - xMargin;
	mActiveCategorySize.y = mBackgroundSize.y - mCategories.back().getSize().y - yMargin;

	mActiveCategoryPos.x = mBackground.getPosition().x + xMargin;
	mActiveCategoryPos.y = mCategories.back().getPosition().y + mCategories.back().getSize().y + yMargin;
}

void Shop::addSeparator(const sf::Vector2f& size, const sf::Vector2f& pos, const sf::Color& color)
{
	sf::RectangleShape separator;
	separator.setSize(size);
	separator.setPosition(pos);
	separator.setFillColor(color);

	mSeparators.push_back(std::move(separator));
}

void Shop::addCategory(const sf::Vector2f& pos, const std::string& label, const sf::Color& defaultColor)
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::UIFont);
	const unsigned int categoryCharSize = static_cast<unsigned int>(mCharSize / 1.5f);
	mCategories.emplace_back(font, label, categoryCharSize, pos);
	mCategories.back().setDefaultColor(defaultColor);
}

float Shop::getLastCategoryWidth() const
{
	if (mCategories.empty())
		return 0.0f;

	return mCategories.back().getSize().x;
}

void Shop::updateCategories(const sf::Vector2f& fMousePos)
{
	for (auto& category : mCategories)
		category.update(fMousePos);
}

void Shop::createPickedCategory()
{
	if (!mActiveCategory)
		return;
	mActiveCategory->create(mActiveCategoryPos, mActiveCategorySize);
}

void Shop::createCategoryTexts()
{
	const sf::Vector2f overlayPos = mBackground.getPosition();
	constexpr float margin = 5.f;
	unsigned int charSize = mCharSize / 2;

	const sf::Vector2f labelPos{overlayPos.x + margin, overlayPos.y + mBackgroundSize.y - charSize - margin };
	mCategoryLabelText.setCharacterSize(charSize);
	mCategoryLabelText.setString("Active Category: ");
	mCategoryLabelText.setPosition(labelPos);
	
	float nameXPos = labelPos.x + mCategoryLabelText.getGlobalBounds().size.x + 2 * margin;
	mCategoryNameText.setCharacterSize(charSize);
	mCategoryNameText.setPosition({ nameXPos, labelPos.y });

	updateCategoryNameText();

	const sf::Vector2f separatorSize{ mBackgroundSize.x, 3.f };
	const sf::Vector2f separatorPos{
		overlayPos.x,
		labelPos.y - margin
	};
	addSeparator(separatorSize, separatorPos);
}

void Shop::updateCategoryNameText()
{
	sf::Color textColor = (mPressedCategory) ? mPressedCategory->getDefaultColor() : sf::Color{ 200, 200, 200 };
	std::string nameStr = (mPressedCategory) ? mPressedCategory->getButtonString() : "None";

	mCategoryNameText.setFillColor(textColor);
	mCategoryNameText.setString(nameStr);
}

void Shop::onCategoryPress(size_t i)
{
	createPickedCategory();
	mPressedCategory = &mCategories[i];
	updateCategoryNameText();
}

void Shop::createCloseShopButton()
{
	constexpr float margin = 5.f;
	auto charSize = mCharSize / 2;
	const auto overlayPos{ mBackground.getPosition() };
	TextButton closeShopButton(mFont, "Close Shop", charSize);

	closeShopButton.setDefaultColor({ 200, 50, 50 });
	closeShopButton.setCallback([this]() {
		mGameContext.eventManager.notify<CloseShopEvent>(CloseShopEvent());
		});

	const sf::Vector2f buttonPos{
		(overlayPos.x + mBackgroundSize.x) - (closeShopButton.getSize().x + margin),
		(overlayPos.y + mBackgroundSize.y) - (charSize + margin)
	};
	closeShopButton.setPosition(buttonPos);
	mCategories.push_back(std::move(closeShopButton));
}
