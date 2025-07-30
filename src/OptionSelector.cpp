#include "pch.h"
#include "OptionSelector.h"
#include "Config.h"

OptionSelector::OptionSelector(const sf::Font& font, const std::string& label)
	:mCharSize(Config::getCharacterSize()),
	mLabel(font, label, mCharSize),
	mValue(font, "", mCharSize),
	mLeftArrow(font, "<", mCharSize),
	mRightArrow(font, ">", mCharSize),
	mCurrValueIndex(0)
{
	init();
}

void OptionSelector::init()
{
	mLabel.setFillColor({ 180, 200, 200 });
	mValue.setFillColor({ 80, 200, 250 });
	mValue.setOutlineColor({ 20, 40, 60 });
	mValue.setOutlineThickness(1.5f);

	constexpr sf::Color arrowColor{ sf::Color::White };
	mLeftArrow.setDefaultColor(arrowColor);
	mRightArrow.setDefaultColor(arrowColor);

	mLeftArrow.setCallback([this]() {
		selectPrevious();
		updateValueText();
	});

	mRightArrow.setCallback([this]() {
		selectNext();
		updateValueText();
	});
	mButtons.push_back(&mLeftArrow);
	mButtons.push_back(&mRightArrow);
}

void OptionSelector::addValue(const std::string& option)
{
	mValueOptions.push_back(option);
	updateValueText();
}

void OptionSelector::processClick(const sf::Event event)
{
	for (auto& btn : mButtons)
	{
		if (btn->isPressed(event))
		{
			btn->invoke();
			return;
		}
	}
}

void OptionSelector::update(const sf::Vector2f& mousePos)
{
	for (auto& btn : mButtons)
	{
		btn->update(mousePos);
	}
}

void OptionSelector::render(sf::RenderWindow& window)
{
	window.draw(mLabel);
	window.draw(mValue);
	mLeftArrow.render(window);
	mRightArrow.render(window);
}

void OptionSelector::setLabelPosition(const sf::Vector2f& pos)
{
	mLabel.setPosition(pos);
}

void OptionSelector::setValuePosition(const sf::Vector2f& pos)
{
	mValue.setPosition(pos);
}

void OptionSelector::setArrowPosition(const sf::Vector2f& pos)
{
	constexpr float margin = 5.f;
	mLeftArrow.setPosition(pos);
	const auto leftWidth = mLeftArrow.getSize().x;
	mRightArrow.setPosition({ pos.x + leftWidth + margin, pos.y });
}

void OptionSelector::setPositions(const sf::Vector2f& labelPos, const sf::Vector2f& valuePos, const sf::Vector2f& arrowPos)
{
	setLabelPosition(labelPos);
	setValuePosition(valuePos);
	setArrowPosition(arrowPos);
}

void OptionSelector::setValue(std::string_view str)
{
	auto it = std::ranges::find(mValueOptions, str);
	if (it != std::ranges::end(mValueOptions))
	{
		mCurrValueIndex = std::distance(std::begin(mValueOptions), it);
		updateValueText();
	}
}

std::string OptionSelector::getValue() const
{
	if (mCurrValueIndex < mValueOptions.size())
		return mValueOptions[mCurrValueIndex];

	return "";
}

float OptionSelector::getLabelWidth() const
{
	return mLabel.getGlobalBounds().size.x;
}

float OptionSelector::getLongestValueWidth()
{
	if (mValueOptions.empty())
		return 0.f;

	auto longest = std::ranges::max_element(mValueOptions, {}, &std::string::size);

	//create temporary string, it copies from mValue so all attributes are also taken.
	sf::Text tmpText(mValue);
	tmpText.setString("[" +  * longest + "]"); //values are shown inside '[]'

	return tmpText.getGlobalBounds().size.x;
}

void OptionSelector::selectNext()
{
	mCurrValueIndex = (mCurrValueIndex + 1) % mValueOptions.size();
}

void OptionSelector::selectPrevious()
{
	mCurrValueIndex = (mCurrValueIndex + mValueOptions.size() - 1) % mValueOptions.size();
}

void OptionSelector::updateValueText()
{
	if (mCurrValueIndex >= mValueOptions.size())
		return;

	const auto& str = mValueOptions[mCurrValueIndex];
	mValue.setString("[" + str + "]");
}
